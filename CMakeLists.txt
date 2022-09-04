cmake_minimum_required(VERSION 3.22)
project(DMGB)

set(CMAKE_CXX_STANDARD 20)

include_directories(Base)

add_executable(DMGB
        Base/Parser.cpp
        Base/Parser.hpp
        main.cpp
        Base/Common.hpp
        Cartridge/Cartridge.cpp Cartridge/Cartridge.hpp
        CPU/Cpu.cpp CPU/Cpu.hpp
        Console/Console.cpp Console/Console.hpp CPU/ALU.cpp CPU/ALU.hpp Base/Utility.cpp Base/Utility.hpp)
