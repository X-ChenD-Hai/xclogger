#include <gtest/gtest.h>

#include <asyncostreamlogsubmitstream.hpp>
#include <ostream>
#include <zmqlogsubmitstream.hpp>

#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <logger.hpp>

// XCLOG_ENABLE_ASYNCSTREAMLOGSUBMITSTREAM()
XCLOG_ENABLE_ASYNCSTREAMLOGSUBMITSTREAM_WITH_PROXY(&std::cout, [](auto msg,
                                                                  auto& os) {
    os << msg.role << " " << msg.label << " " << msg.file << " " << msg.function
       << " " << msg.time << " " << msg.process_id << " " << msg.thread_id
       << " " << msg.line << " " << msg.level << " " << msg.messages[0]
       << std::endl;
})

class IpcLogTest : public ::testing::Test {
   protected:
    ~IpcLogTest() {}
};
TEST_F(IpcLogTest, log2) {
    XLOG(0, data1, data3) << "hello ipc log level= " << 0;
    XLOG(1, data2, data33) << "hello ipc log level= " << 1;
    XLOG(2, data1, data3) << "hello ipc log level= " << 2;
    XLOG(3, data1, data2) << "hello ipc log level= " << 3;
    XLOG(4, data2, data1) << "hello ipc log level= " << 4;
    XLOG(1, data3, data2) << "hello ipc log level= " << 1;
    XLOG(3, data4, data3) << "hello ipc log level= " << 3;
    XLOG(3, data4, data3) << "hello ipc log level= " << 3;
    std::cout << "exit" << std::endl;
}