project(CPU)

add_library(${PROJECT_NAME}
        Arithmetic.cpp
        Unary.cpp
        BitOperations.cpp
        CpuUtility.cpp
        Load.cpp
        Store.cpp
        JumpAndStack.cpp
        Misc.cpp
        Instructions.cpp
        Timer.cpp
        Cpu.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "../Base" "../Bus")