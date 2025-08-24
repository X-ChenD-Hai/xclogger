#pragma once
#include <libipc/ipc.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "./message.hpp"

namespace xclogger {
class AsyncIpcLogSubmitStream {
   private:
    ipc::channel channel_;
    std::thread ipc_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<Message> msg_queue_;
    bool stop_flag_ = false;

   public:
    AsyncIpcLogSubmitStream(const std::string& channel_name)
        : channel_(channel_name.c_str(), ipc::sender) {
        ipc_thread_ = std::thread([this]() {
            while (true) {
                Message msg;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this]() {
                        return stop_flag_ || !msg_queue_.empty();
                    });
                    if (stop_flag_ && msg_queue_.empty()) break;
                    msg = msg_queue_.front();
                    msg_queue_.pop();
                }
                auto data = Message::encode(msg);
                while (!channel_.recv_count());
                channel_.send(data.data(), data.size());
            }
        });
    }
    AsyncIpcLogSubmitStream& operator<<(const Message& msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            msg_queue_.push(msg);
        }
        cv_.notify_one();
        return *this;
    }
    ~AsyncIpcLogSubmitStream() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_flag_ = true;
        }
        cv_.notify_one();
        if (ipc_thread_.joinable()) ipc_thread_.join();
    }
};
}  // namespace xclogger

#define XCLOG_ENABLE_ASYNCIPCLOGSUBMITSTREAM()                 \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                      \
        static ::xclogger::AsyncIpcLogSubmitStream ipc_stream{ \
            ::std::string(kChannelName)};                      \
        return &ipc_stream;                                    \
    }
