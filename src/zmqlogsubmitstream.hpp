#pragma once
#include <string>
#include <zmq.hpp>

#include "./message.hpp"

namespace xclogger {
class ZmqLogSubmitStream {
   private:
    zmq::context_t ctx_{1};
    zmq::socket_t pub_{ctx_, zmq::socket_type::pub};

   public:
    ZmqLogSubmitStream(const std::string& addr) {
        pub_.bind("tcp://127.0.0.1:5555");
    }
    ZmqLogSubmitStream& operator<<(const Message& msg) {
        auto data = Message::encode(msg);
        pub_.send(zmq::str_buffer("hello log"), zmq::send_flags::dontwait);
        ;
        return *this;
    }
    ~ZmqLogSubmitStream() {
        pub_.close();
        ctx_.close();
    }
};
}  // namespace xclogger

#define XCLOG_ENABLE_ZMQLOGSUBMITSTREAM(addr)                   \
    XCLOG_SUBMIT_STREAM_INSTENCE_IMPT() {                       \
        static ::xclogger::ZmqLogSubmitStream zmq_stream{addr}; \
        return &zmq_stream;                                     \
    }
