// req_client.cpp
#include <iostream>
#include <zmq.hpp>


int main() {
    zmq::context_t ctx(1);
    zmq::socket_t req(ctx, zmq::socket_type::req);
    req.connect("tcp://127.0.0.1:5556");  // 连 Python REP 端口

    std::string msg = "hello";
    req.send(zmq::buffer(msg), zmq::send_flags::none);
    std::cout << "[C++] sent: " << msg << std::endl;

    zmq::message_t reply;
    req.recv(reply, zmq::recv_flags::none);
    std::cout << "[C++] recv ack: "
              << std::string(static_cast<char*>(reply.data()), reply.size())
              << std::endl;
    return 0;
}