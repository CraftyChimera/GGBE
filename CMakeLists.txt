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
        Console/Console.cpp Console/Console.hpp CPU/Arithmetic.cpp
        CPU/Arithmetic.hpp CPU/Unary.cpp
        CPU/Unary.hpp CPU/Bit_Operations.cpp
        CPU/Bit_Operations.hpp CPU/Utility.cpp
        CPU/Utility.hpp CPU/Load.cpp CPU/Load.hpp
        CPU/Store.cpp CPU/Store.hpp
        CPU/Jump_and_Stack.cpp CPU/Jump_and_Stack.hpp
        CPU/Misc.cpp CPU/Misc.hpp
        CPU/Instructions.cpp CPU/Instructions.hpp)