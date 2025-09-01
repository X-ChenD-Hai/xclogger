// req_client.cpp
#include <iostream>
#include <nlohmann/json.hpp>
#include <zmq.hpp>
namespace json = nlohmann;

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t req(ctx, zmq::socket_type::req);
    req.connect("tcp://127.0.0.1:5555");  // 连 Python REP 端口
    json::json j = {{"name", "Alice"}, {"age", 25}};
    req.send(zmq::buffer(j.dump()), zmq::send_flags::none);
    std::cout << "[C++] sent: " << j.dump() << std::endl;

    zmq::message_t reply;
    if (req.recv(reply, zmq::recv_flags::none))
        ;
    std::cout << "[C++] recv ack: "
              << std::string(static_cast<char*>(reply.data()), reply.size())
              << std::endl;
    return 0;
}