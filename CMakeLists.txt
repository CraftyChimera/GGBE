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
        Console/Console.cpp Console/Console.hpp CPU/Arithmetic.cpp CPU/Arithmetic.hpp Base/Utility.cpp Base/Utility.hpp CPU/Unary.cpp CPU/Unary.hpp CPU/Bit_Test.cpp CPU/Bit_Test.hpp)
