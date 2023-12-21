//
// Created by drake on 23/8/22.
//

#ifndef GGBE_PARSER_HPP
#define GGBE_PARSER_HPP

#include "Constants.hpp"
#include <string>
#include <fstream>

vector<byte> read_file(const std::string &path, bool is_boot = false);

#endif //GGBE_PARSER_HPP