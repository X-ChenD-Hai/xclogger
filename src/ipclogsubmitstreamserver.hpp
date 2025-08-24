#pragma once
#include <libipc/ipc.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "./message.hpp"

namespace xclogger {
class IpcLogSubmitStreamServer {
   private:
    std::string channel_name_;
    bool stop_ = false;
    std::thread ipc_thread_;
    std::queue<std::vector<char>> data_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::function<void(Message&)> receive_callback_;

   public:
    IpcLogSubmitStreamServer(
        const std::string& channel_name,
        std::function<void(Message&)> receive_callback = {})
        : channel_name_(channel_name), receive_callback_(receive_callback) {}
    void stop() {
        stop_ = true;
        cv_.notify_all();
    }
    bool isRunning() const { return !stop_; }

   private:
    void ipc_listen() {
        while (!stop_) {
            try {
                ipc::channel channel(channel_name_.c_str(), ipc::receiver);
                while (!stop_) {
                    auto buf = channel.recv(50);
                    if (!buf.empty()) {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            data_queue_.emplace((char*)buf.data(),
                                                (char*)buf.data() + buf.size());
                        }
                        cv_.notify_one();
                    }
                }
            } catch (...) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

   public:
    void listen() {
        stop_ = false;
        ipc_thread_ = std::thread(&IpcLogSubmitStreamServer::ipc_listen, this);
        while (!stop_) {
            std::queue<std::vector<char>> data_queue;
            auto lk = std::unique_lock<std::mutex>(mutex_);
            cv_.wait(lk, [this]() { return !data_queue_.empty() || stop_; });
            if (stop_) break;
            std::swap(data_queue, data_queue_);
            lk.unlock();
            while (!data_queue.empty()) {
                auto& data = data_queue.front();
                try {
                    Message msg = Message::decode(data.data(), data.size());
                    receive(msg);
                } catch (...) {
                }
                data_queue.pop();
            }
        }
    }
    void setReceiveCallback(std::function<void(Message&)> callback) {
        receive_callback_ = callback;
    }

   public:
    virtual void receive(Message& msg) {
        receive_callback_ ? receive_callback_(msg) : void();
    }
    virtual ~IpcLogSubmitStreamServer() {
        stop();
        if (ipc_thread_.joinable()) ipc_thread_.join();
    }
};
}  // namespace xclogger
