#include <gtest/gtest.h>

#include <asynczmqlogsubmitstream.hpp>
#include <string>
#include <thread>
#include <zmqlogsubmitstream.hpp>

#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <logger.hpp>

XCLOG_ENABLE_ASYNCZMQLOGSUBMITSTREAM("tcp://127.0.0.1:5552")

class IpcLogTest : public ::testing::Test {
   protected:
    ~IpcLogTest() {}
};
TEST_F(IpcLogTest, log2) {
    XLOG(0, data1, data3) << "hello ipc log";
    XLOG(0, data2, data33) << "hello ipc log";
    XLOG(0, data1, data3) << "hello ipc log";
    XLOG(0, data1, data2) << "hello ipc log";
    XLOG(0, data2, data1) << "hello ipc log";
    XLOG(0, data3, data2) << "hello ipc log";
    XLOG(0, data4, data3) << "hello ipc log";
    std::cout << "exit" << std::endl;
}