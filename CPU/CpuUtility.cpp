//
// Created by drake on 14/9/22.
//

#include "CpuUtility.hpp"

FlagStatus add_flag_status_change(Flag bit, bool status) {
    return {bit, status};
}

vector<FlagStatus> batch_fill(std::array<bool, 4> statuses) {
    vector<FlagStatus> result;
    //z,n,h,c order
    result.emplace_back(add_flag_status_change(Flag::z, statuses[0]));
    result.emplace_back(add_flag_status_change(Flag::n, statuses[1]));
    result.emplace_back(add_flag_status_change(Flag::h, statuses[2]));
    result.emplace_back(add_flag_status_change(Flag::c, statuses[3]));

    return result;
}

word get_signed_offset(byte unsigned_offset) {
    word res = unsigned_offset;
    bool negative = unsigned_offset & (1 << 7);

    if (negative)
        return res + 0xFF00;

    return res;
}