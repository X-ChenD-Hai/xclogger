#include <gtest/gtest.h>

#include <asyncipclogsubmitstream.hpp>
#include <ipclogsubmitstream.hpp>
#include <ipclogsubmitstreamserver.hpp>
#include <string>
#include <thread>

#include "message.hpp"

#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <logger.hpp>
constexpr auto kChannelName = "demo-channel";

XCLOG_ENABLE_ASYNCIPCLOGSUBMITSTREAM()

class IpcLogTest : public ::testing::Test {
   protected:
    ~IpcLogTest() {}
};
TEST_F(IpcLogTest, log) {
    auto server = xclogger::IpcLogSubmitStreamServer(
        kChannelName, [](const xclogger::Message& msg) {
            std::cout << "IpcLogSubmitStreamServer recv: " << msg << std::endl;
        });
    auto t = std::thread([&]() { server.listen(); });
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server.stop();
    t.join();
}