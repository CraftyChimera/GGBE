//
// Created by drake on 23/8/22.
//
#include "Parser.hpp"
#include <iterator>

vector<byte> read_file(const std::string &path, bool is_boot) {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path, std::ios::binary | std::ios::ate);

    stream.seekg(std::ios::beg);
    stream.exceptions(std::ios_base::badbit);
    if (!stream.good()) {
        if (is_boot)
            return {};
        std::cout << "Error while loading rom\n Check if rom exists\n";
        exit(-1);
    }
    vector<byte> out;
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        std::string temp(buf, 0, stream.gcount());
        for (auto x: temp)
            out.push_back(static_cast<uint8_t>(x));
    }
    std::string temp(buf, 0, stream.gcount());
    for (auto x: temp)
        out.push_back(static_cast<uint8_t>(x));
    return out;
}