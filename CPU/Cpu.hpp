//
// Created by drake on 27/8/22.
//

#ifndef GGBE_CPU_HPP
#define GGBE_CPU_HPP

#include "CpuUtility.hpp"
#include "Timer.hpp"
#include "Instructions.hpp"
#include <fstream>
#include <sstream>

class MMU;

class Console;

class CPU {

public:

    bool is_boot;

    bool start_logging;

    std::vector<bool> keys_pressed;

private:
    word SP, PC;
    std::array<byte, 9> reg_mapper;
    vector<byte> fetched;

    Console *console;
    MMU *mem_ptr;
    std::ofstream write_file;
    vector<byte> boot_data;

    bool IME;
    byte interrupt_data;

    Timer timer;
    Instructions current_instruction;
    int dma_cycles;

    bool halt_bug;

    int counter;

    bool halt_mode;

    int interrupt_buffer;

public:
    explicit CPU(Console *console);

    void run_boot_rom();

    void run_instruction_cycle();

private:
    [[maybe_unused]] void debug();

    void set_interrupt_master_flag();

    void handle_interrupts();

    std::string get_string_to_write();

    void push(byte to_push);

    byte pop();

    byte read(word address, bool tick_components = true);

    void write(word address, byte value, bool tick_components = true);

    byte get(Reg reg_index);

    word get(DReg reg_index);

    void set(Reg reg_index, byte value);

    void set(DReg reg_index, word value);

    void set_pc(word address, bool flush = true);

    void set_flags(vector<FlagStatus> &new_flags);

    void fetch();

    void decode_and_execute();

    bool is_interrupt_pending();

    void tick_components();

    bool get_carry();


    // ARITHMETIC

    struct arithmetic_op_args {
        byte src_value;
        byte value;
        bool carry;

        arithmetic_op_args(byte src_value, byte value, bool carry);
    };

    arithmetic_op_args arithmetic_get_args(arithmetic::addr_modes &addr_mode);

    void arithmetic_dispatch(int op_id, arithmetic::addr_modes &addr_mode);

    bool arithmetic_check_and_deal_with_edge_cases(arithmetic::addr_modes &addr_mode);

    void arithmetic_set_result(FlagStatusResponseWrapper &result);

    word ADD_16(word src, word addend);

    word ADD_TO_SP(word src_value, byte signed_offset);

    static FlagStatusResponseWrapper ADD(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper ADC(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper AND(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper CP(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper OR(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper SBC(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper SUB(arithmetic_op_args &arg);

    static FlagStatusResponseWrapper XOR(arithmetic_op_args &arg);

    const std::function<FlagStatusResponseWrapper(arithmetic_op_args &)> arithmetic_op_codes[8] = {ADD, ADC, AND, CP,
                                                                                                   OR, SBC, SUB, XOR};


    // UNARY

    struct unary_op_args {
        std::variant<Reg, word> location;
        byte value;
        bool carry;

        unary_op_args(Reg reg, byte value, bool carry);

        unary_op_args(word address, byte value, bool carry);

    };

    unary_op_args get_unary_args_cb(unary::addr_modes &addr_mode);

    unary_op_args get_unary_args_non_cb();

    unary_op_args get_unary_args(int op_id, unary::addr_modes &addr_mode);

    void unary_dispatch(int op_id, unary::addr_modes &addr_mode);

    bool unary_check_and_deal_with_edge_cases(int op_id, unary::addr_modes &addr_mode);

    void unary_set_result(FlagStatusResponseWrapper &result, unary_op_args &args);

    void DI_r16(int op_id);

    static FlagStatusResponseWrapper INC(unary_op_args &arg);

    static FlagStatusResponseWrapper DEC(unary_op_args &arg);

    static FlagStatusResponseWrapper SWAP(unary_op_args &arg);

    // msb/lsb is 0
    static FlagStatusResponseWrapper SLA(unary_op_args &arg);

    static FlagStatusResponseWrapper SRL(unary_op_args &arg);

    // msb/lsb is carry bit
    static FlagStatusResponseWrapper RL(unary_op_args &arg);

    static FlagStatusResponseWrapper RR(unary_op_args &arg);

    // lsb/msb is bit-7/bit-0 - Rotate bits
    static FlagStatusResponseWrapper RLC(unary_op_args &arg);

    static FlagStatusResponseWrapper RRC(unary_op_args &arg);

    // duplicate msb/lsb
    static FlagStatusResponseWrapper SRA(unary_op_args &arg);

    //Special RL,RR,RLC,RRC variants for A
    static FlagStatusResponseWrapper RLA(unary_op_args &arg);

    static FlagStatusResponseWrapper RRA(unary_op_args &arg);

    static FlagStatusResponseWrapper RLCA(unary_op_args &arg);

    static FlagStatusResponseWrapper RRCA(unary_op_args &arg);

    static FlagStatusResponseWrapper RL_helper(unary_op_args &arg, byte lsb);

    static FlagStatusResponseWrapper RR_helper(unary_op_args &arg, byte msb);

    const std::function<FlagStatusResponseWrapper(unary_op_args &)> unary_op_codes[14] = {INC, DEC, SWAP, RL, RLA, RLC,
                                                                                          RLCA,
                                                                                          RR, RRA, RRC, RRCA, SLA, SRL,
                                                                                          SRA};


    // BIT OP

    struct bit_operations_op_args {
        byte test_bit;
        byte value;
        std::variant<Reg, word> location;

        bit_operations_op_args(byte test_bit, byte value, Reg reg);

        bit_operations_op_args(byte test_bit, byte value, word address);
    };

    bit_operations_op_args bit_operations_get_args(bit_op::addr_modes addressing_mode);

    void bit_operations_dispatch(int op_id, bit_op::addr_modes addr_mode);

    void bit_operations_set_result(int op_id, FlagStatusResponseWrapper &result, bit_operations_op_args &args);

    static FlagStatusResponseWrapper BIT(bit_operations_op_args &);

    static FlagStatusResponseWrapper RES(bit_operations_op_args &);

    static FlagStatusResponseWrapper SET(bit_operations_op_args &);

    const std::function<FlagStatusResponseWrapper(bit_operations_op_args &)> bit_operations_op_codes[3] = {BIT, RES,
                                                                                                           SET};


    // LOAD

    void load_dispatch(int op_id, load::addr_modes addr_mode);


    // STORE

    void store_dispatch(int op_id, store::addr_modes addr_mode);


    // JUMP+STACK

    struct jump_stack_op_args {
        word jump_address;
        int condition;

        jump_stack_op_args();
    };

    jump_stack_op_args jump_stack_get_args(jump_stack::addr_modes addressing_mode);

    void jump_stack_dispatch(int op_id, jump_stack::addr_modes addr_mode);

    bool jump_stack_check_and_deal_with_edge_cases(int op_id);

    void jump_execute_stack_set_result(int op_id, jump_stack_op_args &args);

    void JP(jump_stack_op_args &args);

    void JPC(jump_stack_op_args &args);

    void CALL(jump_stack_op_args &args);

    void CALLC(jump_stack_op_args &args);

    void RET(jump_stack_op_args &args);

    void RETC(jump_stack_op_args &args);

    void RETI(jump_stack_op_args &args);

    void POP(jump_stack_op_args &args);

    void PUSH(jump_stack_op_args &args);


    // Misc

    void misc_dispatch(int op_id, misc::addr_modes &addr_mode);

    void DAA();

    void CPL();

    void CCF();

    void SCF();

    void HALT();

    void STOP();

    void DI();

    void EI();

    void NOP();
};


#endif //GGBE_CPU_HPP