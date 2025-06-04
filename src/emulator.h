/*  @file   emulator.h
 *  @brief  rem8C++ emulator interface.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <cstdint>
#include <vector>


class rem8Cpp {
  public:
    // Operational methods
    rem8Cpp();

    void cycle();
    const std::vector<uint8_t>& get_screen() const;
    void get_screen_rgb(std::vector<unsigned char>& buffer) const;

    void set_program_counter(uint16_t addr);
    void load_rom(uint16_t addr, std::vector<char> data, size_t size);

    void update_timers();

    void set_key(uint8_t key);
    void unset_key(uint8_t key);

    // Diagnositc methods
    std::size_t width() const;
    std::size_t height() const;

    uint16_t program_counter() const;
    uint8_t read_memory(uint16_t addr) const;

    uint8_t data_register(uint8_t reg) const;
    uint16_t I_register() const;
    uint16_t stack_pointer() const;
    uint8_t key(uint8_t key) const;
    bool key_pressed() const;
    uint8_t sound_timer() const;
    uint8_t delay_timer() const;

  private:
    std::size_t width_;
    std::size_t height_;
    std::vector<uint8_t> screen_;

    uint8_t data_registers_[0x10];
    uint16_t I_register_;
    uint16_t program_counter_;
    uint16_t stack_pointer_;
    uint16_t sprite_addr_;
    uint8_t key_[0x10];
    uint8_t key_binds_[0x10];
    bool key_pressed_;
    uint8_t sound_timer_;
    uint8_t delay_timer_;

    std::vector<uint8_t> memory_;

    void _stack_push_pc();
    void _stack_pull_pc();

    void _sprite_set(uint16_t loc);
    char _sprite_draw(uint8_t X, uint8_t Y, char height);

    void _instr_0NNN();
    void _instr_00E0();
    void _instr_00EE();
    void _instr_1NNN(uint8_t msb, uint8_t lsb);
    void _instr_2NNN(uint8_t msb, uint8_t lsb);
    void _instr_3XNN(uint8_t msb, uint8_t lsb);
    void _instr_4XNN(uint8_t msb, uint8_t lsb);
    void _instr_5XY0(uint8_t msb, uint8_t lsb);
    void _instr_6XNN(uint8_t msb, uint8_t lsb);
    void _instr_7XNN(uint8_t msb, uint8_t lsb);
    void _instr_8XY0(uint8_t msb, uint8_t lsb);
    void _instr_8XY1(uint8_t msb, uint8_t lsb);
    void _instr_8XY2(uint8_t msb, uint8_t lsb);
    void _instr_8XY3(uint8_t msb, uint8_t lsb);
    void _instr_8XY4(uint8_t msb, uint8_t lsb);
    void _instr_8XY5(uint8_t msb, uint8_t lsb);
    void _instr_8XY6(uint8_t msb, uint8_t lsb);
    void _instr_8XY7(uint8_t msb, uint8_t lsb);
    void _instr_8XYE(uint8_t msb, uint8_t lsb);
    void _instr_9XY0(uint8_t msb, uint8_t lsb);
    void _instr_ANNN(uint8_t msb, uint8_t lsb);
    void _instr_BNNN(uint8_t msb, uint8_t lsb);
    void _instr_CXNN(uint8_t msb, uint8_t lsb);
    void _instr_DXYN(uint8_t msb, uint8_t lsb);
    void _instr_EX9E(uint8_t msb);
    void _instr_EXA1(uint8_t msb);
    void _instr_FX07(uint8_t msb);
    void _instr_FX0A(uint8_t msb);
    void _instr_FX15(uint8_t msb);
    void _instr_FX18(uint8_t msb);
    void _instr_FX1E(uint8_t msb);
    void _instr_FX29(uint8_t msb);
    void _instr_FX33(uint8_t msb);
    void _instr_FX55(uint8_t msb);
    void _instr_FX65(uint8_t msb);

};

