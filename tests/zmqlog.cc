#include <gtest/gtest.h>

#include <xclogger/asynczmqlogsubmitstream.hpp>
#include <string>
#include <xclogger/zmqlogsubmitstream.hpp>

#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <xclogger/logger.hpp>

XCLOG_ENABLE_ASYNCZMQLOGSUBMITSTREAM("tcp://127.0.0.1:5553")

class IpcLogTest : public ::testing::Test {
   protected:
    ~IpcLogTest() {}
};
TEST_F(IpcLogTest, log2) {
    XLOG(0, data1, data3) << "MAin ipc log level= " << 0;
    XLOG(1, data2, data33) << "Date ipc log level= " << 1;
    XLOG(2, data1, data3) << "Sql ipc log level= " << 2;
    XLOG(3, data1, data2) << "Win ipc log level= " << 3;
    XLOG(4, data2, data1) << "Mac ipc log level= " << 4;
    XLOG(1, data3, data2) << "OS X ipc log level= " << 1;
    XLOG(3, data4, data3) << "Linux ipc log level= " << 3;
    XLOG(3, data4, data3) << "Ubuntu ipc log level= " << 3;
    std::cout << "exit" << std::endl;
}