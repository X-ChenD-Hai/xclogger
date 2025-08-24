#pragma once
#include <libipc/ipc.h>

#include <string>

#include "./message.hpp"

namespace xclogger {
class IpcLogSubmitStream {
   private:
    ipc::channel channel_;

   public:
    IpcLogSubmitStream(const std::string& channel_name)
        : channel_(channel_name.c_str(), ipc::sender) {}
    IpcLogSubmitStream& operator<<(const Message& msg) {
        auto data = Message::encode(msg);
        while (!channel_.recv_count());
        channel_.send(data.data(), data.size());
        return *this;
    }
};
}  // namespace xclogger

#define XCLOG_ENABLE_IPCLOGSUBMITSTREAM()                 \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                 \
        static ::xclogger::IpcLogSubmitStream ipc_stream{ \
            ::std::string(kChannelName)};                 \
        return &ipc_stream;                               \
    }
