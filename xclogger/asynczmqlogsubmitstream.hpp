#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <zmq.hpp>

#include "./message.hpp"

namespace xclogger {
class AsyncZmqLogSubmitStream {
   private:
    std::thread ipc_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<Message> msg_queue_;
    bool stop_flag_ = false;

   public:
    AsyncZmqLogSubmitStream(const std::string& addr) {
        ipc_thread_ = std::thread([this, addr]() {
            zmq::context_t ctx{1};
            zmq::socket_t req{ctx, zmq::socket_type::req};
            req.connect(addr);
            req.set(zmq::sockopt::rcvtimeo, 2000);
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
                std::vector<char> data = Message::encode(msg);
                req.send(zmq::const_buffer(data.data(), data.size()),
                         zmq::send_flags::dontwait);
                zmq::message_t reply;
                if (req.recv(reply, zmq::recv_flags::none)) {
                } else {
                }
            }
            req.close();
            ctx.close();
        });
    }
    AsyncZmqLogSubmitStream& operator<<(const Message& msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            msg_queue_.push(msg);
        }
        cv_.notify_one();
        return *this;
    }
    ~AsyncZmqLogSubmitStream() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_flag_ = true;
        }
        cv_.notify_one();
        if (ipc_thread_.joinable()) ipc_thread_.join();
    }
};
}  // namespace xclogger

#define XCLOG_ENABLE_ASYNCZMQLOGSUBMITSTREAM(addr)                   \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                            \
        static ::xclogger::AsyncZmqLogSubmitStream zmq_stream{addr}; \
        return &zmq_stream;                                          \
    }
