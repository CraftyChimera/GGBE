//
// Created by drake on 23/8/22.
//
#include "Parser.hpp"

vector<byte> read_file(const std::string &path) {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path);
    stream.exceptions(std::ios_base::badbit);
    if (!stream.good()) {
        std::cout << "Error while loading rom\n Check if rom exists\n";
        exit(-1);
    }
    vector<byte> out;
    auto buf = string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        string temp(buf, 0, stream.gcount());
        for (auto x: temp)
            out.push_back(static_cast<byte>(x));
    }
    string temp(buf, 0, stream.gcount());
    for (auto x: temp)
        out.push_back(static_cast<byte>(x));
    return out;
}