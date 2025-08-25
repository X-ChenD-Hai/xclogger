#include <gtest/gtest.h>

#include <asynczmqlogsubmitstream.hpp>
#include <string>
#include <thread>
#include <zmqlogsubmitstream.hpp>

#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <logger.hpp>

XCLOG_ENABLE_ASYNCZMQLOGSUBMITSTREAM("tcp://127.0.0.1:5555")

class IpcLogTest : public ::testing::Test {
   protected:
    ~IpcLogTest() {}
};
TEST_F(IpcLogTest, log) {
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    XLOG(0, data, data) << "hello ipc log";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}