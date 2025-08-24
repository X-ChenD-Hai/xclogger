#include <libipc/ipc.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

constexpr const char* kChannelName = "demo-channel";

void producer(const char* id) {
    ipc::channel ch(kChannelName, ipc::sender);
    for (int i = 0; i < 3; ++i) {
        std::string msg = std::string("from ") + id + " - " + std::to_string(i);
        while (!ch.recv_count());
        ch.send(msg.c_str(), msg.size());
    }
}

void consumer(const char* name) {
    ipc::channel ch(kChannelName, ipc::receiver);
    while (true) {
        auto buf = ch.recv();
        if (buf.empty()) break;
        std::cout << "[" << name
                  << "] recv: " << std::string{(char*)buf.data(), buf.size()}
                  << std::endl;
    }
}

int main() {
    ipc::shm::remove(kChannelName);  // 清理残留共享内存（Windows 必需）
    std::thread c1(consumer, "C1");
    producer("P1");
    std::thread p2(producer, "P2");

    // p1.join();
    p2.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    c1.detach();
    return 0;
}