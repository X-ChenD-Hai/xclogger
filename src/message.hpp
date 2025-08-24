#pragma once
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace xclogger {
class ImplCatgory;
struct Message {
    std::string role;
    std::string label;
    std::string file;
    std::string function;
    size_t time;
    size_t process_id;
    size_t thread_id;
    int line;
    int level = 0;
    std::vector<std::string> messages;
    static void __encode_string(std::vector<char>& data,
                                const std::string& str) {
        size_t size = str.size();
        data.insert(data.end(), (char*)&size, (char*)&size + sizeof(size_t));
        data.insert(data.end(), str.begin(), str.end());
    }
    static std::vector<char> encode(const Message& msg) {
        std::vector<char> data;
        for (auto& it : {msg.role, msg.label, msg.file, msg.function})
            __encode_string(data, it);
        for (auto it : {msg.time, msg.process_id, msg.thread_id})
            data.insert(data.end(), (char*)&it, (char*)&it + sizeof(size_t));
        for (auto it : {msg.line, msg.level})
            data.insert(data.end(), (char*)&it, (char*)&it + sizeof(int));
        std::for_each(
            msg.messages.begin(), msg.messages.end(),
            [&data](const std::string& str) { __encode_string(data, str); });
        return data;
    }
    static Message decode(const char* data, size_t size) {
        Message msg;
        size_t offset = 0;
        auto decode_string = [&](std::string& str) {
            if (offset + sizeof(size_t) > size)
                throw std::runtime_error("decode string failed");
            size_t str_size = *(size_t*)(data + offset);
            offset += sizeof(size_t);
            if (offset + str_size > size)
                throw std::runtime_error("decode string failed");
            str = std::string(data + offset, str_size);
            offset += str_size;
        };
        for (auto& it : {&msg.role, &msg.label, &msg.file, &msg.function})
            decode_string(*it);
        for (auto it : {&msg.time, &msg.process_id, &msg.thread_id}) {
            if (offset + sizeof(size_t) > size)
                throw std::runtime_error("decode size_t failed");
            *it = *(size_t*)(data + offset);
            offset += sizeof(size_t);
        }
        for (auto it : {&msg.line, &msg.level}) {
            if (offset + sizeof(int) > size)
                throw std::runtime_error("decode int failed");
            *it = *(int*)(data + offset);
            offset += sizeof(int);
        }
        while (offset < size) {
            std::string str;
            decode_string(str);
            msg.messages.emplace_back(std::move(str));
        }
        return msg;
    }
};
template <class T = ImplCatgory>
inline std::ostream& operator<<(std::ostream& o, const Message& msg) {
    o << "[" << msg.file << ":" << msg.line << "]"
      << "[" << msg.function << "]"
      << "[" << msg.role << "]"
      << "[" << msg.label << "]"
      << "[" << msg.process_id << "]"
      << "[" << msg.thread_id << "]"
      << "[" << msg.time << "us]"
      << "[level=" << msg.level << "] ";
    for (const auto& m : msg.messages) o << m << " ";
    std::cout << std::endl;
    return o;
}
}  // namespace xclogger
#define XCLOG_MSG_OSTREAM_IMPT(out_arg, msg_arg)                \
    template <>                                                 \
    std::ostream& xclogger::operator<< <xclogger::ImplCatgory>( \
        std::ostream & out_arg, const Message& msg_arg)