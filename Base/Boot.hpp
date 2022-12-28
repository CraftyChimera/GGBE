//
// Created by drake on 27/12/22.
//

#ifndef DMGB_BOOT_HPP
#define DMGB_BOOT_HPP

#include "Constants.hpp"
#include "../MMU/Mmu.hpp"

namespace Boot {
    void boot(MMU *mem_ptr);
}
#endif //DMGB_BOOT_HPP
