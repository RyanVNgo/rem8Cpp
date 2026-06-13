
#pragma once

#include <cstdint>
#include <vector>


//---------------------------------------------------
// rem8Cpp
//---------------------------------------------------

class rem8Cpp {
    public:
        rem8Cpp();

        auto cycle() noexcept -> void;

        [[nodiscard]] auto get_screen() const noexcept -> const std::vector<uint8_t>&;

        auto get_screen_rgb(std::vector<unsigned char>& buffer) const noexcept -> void;

        auto set_program_counter(uint16_t addr) noexcept -> void;

        auto load_rom(uint16_t addr, std::vector<char> data, size_t size) noexcept -> void;

        auto update_timers() noexcept -> void;

        auto set_key(uint8_t key) noexcept -> void;

        auto unset_key(uint8_t key) noexcept -> void;

        [[nodiscard]] auto width() const noexcept -> std::size_t;

        [[nodiscard]] auto height() const noexcept -> std::size_t;

        [[nodiscard]] auto program_counter() const noexcept -> uint16_t;

        [[nodiscard]] auto read_memory(uint16_t addr) const noexcept -> uint8_t;

        [[nodiscard]] auto data_register(uint8_t reg) const noexcept -> uint8_t;

        [[nodiscard]] auto I_register() const noexcept -> uint16_t;

        [[nodiscard]] auto stack_pointer() const noexcept -> uint16_t;

        [[nodiscard]] auto key(uint8_t key) const noexcept -> uint8_t;

        [[nodiscard]] auto key_pressed() const noexcept -> bool;

        [[nodiscard]] auto sound_timer() const noexcept -> uint8_t;

        [[nodiscard]] auto delay_timer() const noexcept -> uint8_t ;

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

        auto stack_push_pc() noexcept -> void;
        auto stack_pull_pc() noexcept -> void;

        auto sprite_set(uint16_t loc) noexcept -> void;
        [[nodiscard]] auto sprite_draw(uint8_t X, uint8_t Y, char height) noexcept -> char;

        auto msb_reg_idx(uint8_t msb) const noexcept -> uint8_t;
        auto lsb_reg_idx(uint8_t lsb) const noexcept -> uint8_t;

        auto instr_0NNN() noexcept -> void;
        auto instr_00E0() noexcept -> void;
        auto instr_00EE() noexcept -> void;
        auto instr_1NNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_2NNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_3XNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_4XNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_5XY0(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_6XNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_7XNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY0(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY1(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY2(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY3(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY4(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY5(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY6(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XY7(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_8XYE(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_9XY0(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_ANNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_BNNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_CXNN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_DXYN(uint8_t msb, uint8_t lsb) noexcept -> void;
        auto instr_EX9E(uint8_t msb) noexcept -> void;
        auto instr_EXA1(uint8_t msb) noexcept -> void;
        auto instr_FX07(uint8_t msb) noexcept -> void;
        auto instr_FX0A(uint8_t msb) noexcept -> void;
        auto instr_FX15(uint8_t msb) noexcept -> void;
        auto instr_FX18(uint8_t msb) noexcept -> void;
        auto instr_FX1E(uint8_t msb) noexcept -> void;
        auto instr_FX29(uint8_t msb) noexcept -> void;
        auto instr_FX33(uint8_t msb) noexcept -> void;
        auto instr_FX55(uint8_t msb) noexcept -> void;
        auto instr_FX65(uint8_t msb) noexcept -> void;

};

