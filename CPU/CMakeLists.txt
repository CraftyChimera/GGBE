project(CPU)

add_library(${PROJECT_NAME}
        Cpu.cpp
        Arithmetic.cpp
        Unary.cpp
        Bit_Operations.cpp
        Utility.cpp
        Load.cpp
        Store.cpp
        Jump_and_Stack.cpp
        Misc.cpp
        Instructions.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC "../Base" "../MMU")