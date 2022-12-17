//
// Created by drake on 14/9/22.
//

#include "Utility.hpp"

Flag_Status set(Flag bit, bool status) {
    return {bit, status};
}

vector<Flag_Status> batch_fill(std::array<bool, 4> statuses) {
    vector<Flag_Status> result;
    //z,n,h,c order
    result.emplace_back(set(Flag::z, statuses[0]));
    result.emplace_back(set(Flag::n, statuses[1]));
    result.emplace_back(set(Flag::h, statuses[2]));
    result.emplace_back(set(Flag::c, statuses[3]));

    return result;
}