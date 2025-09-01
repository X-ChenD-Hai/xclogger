#include <gtest/gtest.h>

#include <memory>
#include <ostream>
#include <xclogger/message.hpp>


#define XCLOG_FUNCTION_TRACK
#define XCLOG_FILE_TRACK
#define XCLOG_LINE_TRACK

#include <xclogger/logger.hpp>

XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() { return std::shared_ptr<std::ostream>(); }
class IpcLogTest : public ::testing::Test {
   protected:
    template <class OS>
    xclogger::Message msg(const OS& os) {
        return os.msg_;
    }
};
TEST_F(IpcLogTest, log) { XLOG(0, data, data) << "hello"; }
TEST_F(IpcLogTest, message) {
    auto os = (XLOG(0, data, data) << "hello" << " world");
    auto _msg = msg(os);
    auto data = xclogger::Message::encode(_msg);
    auto new_msg = xclogger::Message::decode(data.data(), data.size());
    EXPECT_EQ(_msg.role, new_msg.role);
    EXPECT_EQ(_msg.label, new_msg.label);
    EXPECT_EQ(_msg.file, new_msg.file);
    EXPECT_EQ(_msg.function, new_msg.function);
    EXPECT_EQ(_msg.time, new_msg.time);
    EXPECT_EQ(_msg.line, new_msg.line);
    EXPECT_EQ(_msg.level, new_msg.level);
    EXPECT_EQ(_msg.messages.size(), new_msg.messages.size());
    for (size_t i = 0; i < _msg.messages.size(); ++i)
        EXPECT_EQ(_msg.messages[i], new_msg.messages[i]);
}