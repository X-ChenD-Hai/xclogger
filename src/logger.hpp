#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <process.h>
#define GET_PID() _getpid()
#else
#include <unistd.h>
#define GET_PID() getpid()
#endif
#include <thread>

#include "./message.hpp"

class IpcLogTest;
namespace xclogger {
class ImplCatgory;
template <class SubmitStream_ptr>
class LogStream {
    friend class ::IpcLogTest;

   private:
    SubmitStream_ptr submit_stream_;
    Message msg_;

   public:
    LogStream(SubmitStream_ptr submit_stream, const char* file, int line,
              const char* function, int level, const char* role = "",
              const char* label = "")
        : submit_stream_(submit_stream),
          msg_(role, label, file, function,
               std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count(),
               GET_PID(), std::this_thread::get_id()._Get_underlying_id(), line,
               level, {}) {};
    ~LogStream() {
        if (submit_stream_) {
            if constexpr (std::is_same_v<SubmitStream_ptr,
                                         std::shared_ptr<std::ostream>>)
                xclogger::operator<< <ImplCatgory>(std::cout, msg_);
            else
                (*submit_stream_) << msg_;
        } else
            xclogger::operator<< <ImplCatgory>(std::cout, msg_);
    }
    template <typename T>
    LogStream& operator<<(const T& msg) {
        msg_.messages.emplace_back((std::stringstream() << msg).str());
        return *this;
    }
};
template <class Category = xclogger::ImplCatgory>
auto SubmitStreamInstance() {
    return std::shared_ptr<std::ostream>();
}
}  // namespace xclogger

#ifdef XCLOG_FUNCTION_TRACK
#define XCLOG_FUNCTION_NAME __FUNCTION__
#else
#define XCLOG_FUNCTION_NAME ""
#endif
#ifdef XCLOG_FILE_TRACK
#define XCLOG_FILE_NAME __FILE__
#else
#define XCLOG_FILE_NAME ""
#endif
#ifdef XCLOG_LINE_TRACK
#define XCLOG_LINE_NAME __LINE__
#else
#define XCLOG_LINE_NAME 0
#endif

#define XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() \
    template <>                             \
    auto ::xclogger::SubmitStreamInstance<::xclogger::ImplCatgory>()

#define XLOG(level, role, label)                                             \
    ::xclogger::LogStream(                                                   \
        ::xclogger::SubmitStreamInstance<::xclogger::ImplCatgory>(),         \
        XCLOG_FILE_NAME, XCLOG_LINE_NAME, XCLOG_FUNCTION_NAME, level, #role, \
        #label)
#define LOG(level) XLOG(level, , )
