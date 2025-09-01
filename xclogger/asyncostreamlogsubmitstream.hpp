#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>

#include "./message.hpp"

namespace xclogger {
class AsyncOstreamLogSubmitStream {
   private:
    std::thread collect_thread_;
    std::thread submit_thread_;
    std::mutex collect_mutex_;
    std::mutex ready_queue_mutex_;
    std::condition_variable collect_cv_;
    std::condition_variable ready_cv_;
    std::queue<Message> msg_collect_queue_;
    std::queue<Message> msg_ready_queue_;
    std::atomic<bool> stop_flag_{false};
    std::ostream* os_;
    std::function<void(const Message&, std::ostream&)> proxy_func_;

   public:
    AsyncOstreamLogSubmitStream(
        std::ostream* os = &std::cout,
        std::function<void(const Message&, std::ostream&)> proxy_func = nullptr)
        : os_(os) {
        // 设置默认的代理函数
        if (!proxy_func) {
            proxy_func_ = [](const Message& msg, std::ostream& stream) {
                stream << msg;
            };
        } else {
            proxy_func_ = proxy_func;
        }

        // 收集线程
        collect_thread_ = std::thread([this]() { collectThreadFunc(); });

        // 提交线程
        submit_thread_ = std::thread([this]() { submitThreadFunc(); });
    }

    // 禁止拷贝和移动
    AsyncOstreamLogSubmitStream(const AsyncOstreamLogSubmitStream&) = delete;
    AsyncOstreamLogSubmitStream& operator=(const AsyncOstreamLogSubmitStream&) =
        delete;

    AsyncOstreamLogSubmitStream& operator<<(const Message& msg) {
        {
            std::lock_guard<std::mutex> lock(collect_mutex_);
            msg_collect_queue_.push(msg);
        }
        collect_cv_.notify_one();
        return *this;
    }

    // 移动语义支持
    AsyncOstreamLogSubmitStream& operator<<(Message&& msg) {
        {
            std::lock_guard<std::mutex> lock(collect_mutex_);
            msg_collect_queue_.push(std::move(msg));
        }
        collect_cv_.notify_one();
        return *this;
    }

    ~AsyncOstreamLogSubmitStream() {
        // 设置停止标志
        stop_flag_.store(true, std::memory_order_release);

        // 通知所有等待的线程
        collect_cv_.notify_one();
        ready_cv_.notify_one();

        // 等待线程结束
        if (collect_thread_.joinable()) {
            collect_thread_.join();
        }
        if (submit_thread_.joinable()) {
            submit_thread_.join();
        }

        // 清空剩余的消息（可选）
        flushRemainingMessages();
    }

    // 刷新剩余消息（确保所有消息都被处理）
    void flush() {
        // 等待所有队列为空
        while (true) {
            std::unique_lock<std::mutex> collect_lock(collect_mutex_);
            std::unique_lock<std::mutex> ready_lock(ready_queue_mutex_);

            if (msg_collect_queue_.empty() && msg_ready_queue_.empty()) {
                break;
            }

            // 短暂释放锁，让线程有机会处理消息
            collect_lock.unlock();
            ready_lock.unlock();
            std::this_thread::yield();
        }
    }

   private:
    void collectThreadFunc() {
        while (!stop_flag_.load(std::memory_order_acquire)) {
            std::queue<Message> temp_queue;

            // 从收集队列获取消息
            {
                std::unique_lock<std::mutex> lock(collect_mutex_);
                collect_cv_.wait(lock, [this]() {
                    return stop_flag_.load(std::memory_order_acquire) ||
                           !msg_collect_queue_.empty();
                });

                if (stop_flag_.load(std::memory_order_acquire)) {
                    break;
                }

                // 交换到临时队列，尽快释放锁
                temp_queue.swap(msg_collect_queue_);
            }

            // 将消息转移到就绪队列
            if (!temp_queue.empty()) {
                std::lock_guard<std::mutex> lock(ready_queue_mutex_);
                while (!temp_queue.empty()) {
                    msg_ready_queue_.push(std::move(temp_queue.front()));
                    temp_queue.pop();
                }
                ready_cv_.notify_one();
            }
        }

        // 线程结束前处理剩余消息
        processRemainingMessagesInCollect();
    }

    void submitThreadFunc() {
        while (!stop_flag_.load(std::memory_order_acquire)) {
            Message msg;
            bool has_msg = false;

            // 从就绪队列获取消息
            {
                std::unique_lock<std::mutex> lock(ready_queue_mutex_);
                ready_cv_.wait(lock, [this]() {
                    return stop_flag_.load(std::memory_order_acquire) ||
                           !msg_ready_queue_.empty();
                });

                if (stop_flag_.load(std::memory_order_acquire) &&
                    msg_ready_queue_.empty()) {
                    break;
                }

                if (!msg_ready_queue_.empty()) {
                    msg = std::move(msg_ready_queue_.front());
                    msg_ready_queue_.pop();
                    has_msg = true;
                }
            }

            // 处理消息（不在锁内执行）
            if (has_msg && os_) {
                try {
                    proxy_func_(msg, *os_);
                } catch (...) {
                    // 异常处理：避免因为单个消息处理失败导致整个线程崩溃
                    // 可以在这里添加错误日志
                }
            }
        }

        // 线程结束前处理剩余消息
        processRemainingMessagesInSubmit();
    }

    void processRemainingMessagesInCollect() {
        std::queue<Message> temp_queue;
        {
            std::lock_guard<std::mutex> lock(collect_mutex_);
            temp_queue.swap(msg_collect_queue_);
        }

        if (!temp_queue.empty()) {
            std::lock_guard<std::mutex> lock(ready_queue_mutex_);
            while (!temp_queue.empty()) {
                msg_ready_queue_.push(std::move(temp_queue.front()));
                temp_queue.pop();
            }
            ready_cv_.notify_one();
        }
    }

    void processRemainingMessagesInSubmit() {
        while (true) {
            Message msg;
            bool has_msg = false;

            {
                std::lock_guard<std::mutex> lock(ready_queue_mutex_);
                if (!msg_ready_queue_.empty()) {
                    msg = std::move(msg_ready_queue_.front());
                    msg_ready_queue_.pop();
                    has_msg = true;
                }
            }

            if (!has_msg) break;

            if (os_) {
                try {
                    proxy_func_(msg, *os_);
                } catch (...) {
                    // 异常处理
                }
            }
        }
    }

    void flushRemainingMessages() {
        processRemainingMessagesInCollect();
        processRemainingMessagesInSubmit();
    }
};
}  // namespace xclogger

#define XCLOG_ENABLE_ASYNCSTREAMLOGSUBMITSTREAM()                        \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                                \
        static ::xclogger::AsyncOstreamLogSubmitStream ostream_stream{}; \
        return &ostream_stream;                                          \
    }
#define XCLOG_ENABLE_ASYNCSTREAMLOGSUBMITSTREAM_WITH_PROXY(os, proxy_func) \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                                  \
        static ::xclogger::AsyncOstreamLogSubmitStream ostream_stream{     \
            os, proxy_func};                                               \
        return &ostream_stream;                                            \
    }
