
#include "emulator.h"

#include <cstdio>
#include <climits>
#include <cstring>
#include <cstdlib>


#define REM8CPP_SCREEN_WIDTH  0x40
#define REM8CPP_SCREEN_HEIGHT 0x20

#define REM8CPP_MAX_ADDR      0x0FFF
#define REM8CPP_START_ADDR    0x0200

#define FONT_SET_ADDR         0x0000

#define SPRITE_WIDTH          5
#define INSTR_SIZE            2

#define KEY_ON                0x1
#define KEY_OFF               0x0


//---------------------------------------------------
// rem8Cpp
//---------------------------------------------------

rem8Cpp::rem8Cpp() 
    : width_(REM8CPP_SCREEN_WIDTH),
    height_(REM8CPP_SCREEN_HEIGHT),
    screen_(width_ * height_, 0x00),
    program_counter_(0x200),
    stack_pointer_(0x200 - 0x01),
    sprite_addr_(FONT_SET_ADDR),
    key_pressed_(false),
    sound_timer_(0x00),
    delay_timer_(0x00),
    memory_(REM8CPP_MAX_ADDR, 0x00)
{ 
    sprite_set(sprite_addr_);
    key_binds_[0x1] = '1'; key_binds_[0x2] = '2'; key_binds_[0x3] = '3'; key_binds_[0xC] = '4';
    key_binds_[0x4] = 'q'; key_binds_[0x5] = 'w'; key_binds_[0x6] = 'e'; key_binds_[0xD] = 'r';
    key_binds_[0x7] = 'a'; key_binds_[0x8] = 's'; key_binds_[0x9] = 'd'; key_binds_[0xE] = 'f';
    key_binds_[0xA] = 'z'; key_binds_[0x0] = 'x'; key_binds_[0xB] = 'c'; key_binds_[0xF] = 'v';
    memset(key_, 0x00, sizeof(uint8_t) * 0x10);
}

auto rem8Cpp::cycle() noexcept -> void {
    uint8_t msb = memory_[program_counter_++];
    uint8_t lsb = memory_[program_counter_++];

    switch (msb & 0xF0) {
        case 0x00:
            switch (msb << 8 | lsb) {
                case 0x00E0:
                    instr_00E0(); break;
                case 0x00EE:
                    instr_00EE(); break;
                default:
                    instr_0NNN(); break;
            }
            break;
        case 0x10:
            instr_1NNN(msb, lsb); break;
        case 0x20:
            instr_2NNN(msb, lsb); break;
        case 0x30:
            instr_3XNN(msb, lsb); break;
        case 0x40:
            instr_4XNN(msb, lsb); break;
        case 0x50:
            instr_5XY0(msb, lsb); break;
        case 0x60:
            instr_6XNN(msb, lsb); break;
        case 0x70:
            instr_7XNN(msb, lsb); break;
        case 0x80:
            switch (lsb & 0x0F) {
                case 0x00:
                    instr_8XY0(msb, lsb); break;
                case 0x01:
                    instr_8XY1(msb, lsb); break;
                case 0x02:
                    instr_8XY2(msb, lsb); break;
                case 0x03:
                    instr_8XY3(msb, lsb); break;
                case 0x04:
                    instr_8XY4(msb, lsb); break;
                case 0x05:
                    instr_8XY5(msb, lsb); break;
                case 0x06:
                    instr_8XY6(msb, lsb); break;
                case 0x07:
                    instr_8XY7(msb, lsb); break;
                case 0x0E:
                    instr_8XYE(msb, lsb); break;
                default: break;
            }
            break;
        case 0x90:
            instr_9XY0(msb, lsb); break;
        case 0xA0:
            instr_ANNN(msb, lsb); break;
        case 0xB0:
            instr_BNNN(msb, lsb); break;
        case 0xC0:
            instr_CXNN(msb, lsb); break;
        case 0xD0:
            instr_DXYN(msb, lsb); break;
        case 0xE0:
            switch (lsb) {
                case 0x9E:
                    instr_EX9E(msb); break;
                case 0xA1:
                    instr_EXA1(msb); break;
                default: break;
            }
            break;
        case 0xF0:
            switch (lsb) {
                case 0x07:
                    instr_FX07(msb); break;
                case 0x0A:
                    instr_FX0A(msb); break;
                case 0x15:
                    instr_FX15(msb); break;
                case 0x18:
                    instr_FX18(msb); break;
                case 0x1E:
                    instr_FX1E(msb); break;
                case 0x29:
                    instr_FX29(msb); break;
                case 0x33:
                    instr_FX33(msb); break;
                case 0x55:
                    instr_FX55(msb); break;
                case 0x65:
                    instr_FX65(msb); break;
                default: break;
            }
            break;
        default: break;
    }
}

auto rem8Cpp::get_screen() const noexcept -> const std::vector<uint8_t>& {
    return screen_;
}

auto rem8Cpp::get_screen_rgb(std::vector<unsigned char>& buffer) const noexcept -> void {
    for (std::size_t i = 0; i < screen_.size(); i++) {
        memset(buffer.data() + i * 3, (screen_[i] & 0x01) * UCHAR_MAX, sizeof(unsigned char) * 3);
    }
}

auto rem8Cpp::set_program_counter(uint16_t addr) noexcept -> void {
    if (addr >= REM8CPP_MAX_ADDR) return;
    program_counter_ = addr;
}

auto rem8Cpp::load_rom(uint16_t addr, std::vector<char> data, size_t size) noexcept -> void {
    if (addr + size >= REM8CPP_MAX_ADDR) return;
    memset(memory_.data(), 0x00, sizeof(uint8_t) * memory_.size());
    memcpy(&memory_[addr], data.data(), size);
}

auto rem8Cpp::update_timers() noexcept -> void {
    if (delay_timer_ > 0) delay_timer_--;
    if (sound_timer_ > 0) sound_timer_--;
}

auto rem8Cpp::set_key(uint8_t key) noexcept -> void {
    for (int i = 0; i < 16; i++) {
        if (key_binds_[i] == key) {
            key_[i] = KEY_ON;
            key_pressed_ = true;
            break;
        }
    }
}

auto rem8Cpp::unset_key(uint8_t key) noexcept -> void {
    for (int i = 0; i < 16; i++) {
        if (key_binds_[i] == key) {
            key_[i] = KEY_OFF;
            key_pressed_ = false;
            break;
        }
    }
}


// Diagnositc methods

auto rem8Cpp::width() const noexcept -> std::size_t {
    return width_;
}

auto rem8Cpp::height() const noexcept -> std::size_t {
    return height_;
}

auto rem8Cpp::program_counter() const noexcept -> uint16_t {
    return program_counter_;
}

auto rem8Cpp::read_memory(uint16_t addr) const noexcept -> uint8_t {
    return memory_[addr];
}

auto rem8Cpp::data_register(uint8_t reg) const noexcept -> uint8_t {
    if (reg > 0x10) return 0xFF;
    return data_registers_[reg];
}

auto rem8Cpp::I_register() const noexcept -> uint16_t {
    return I_register_;
}

auto rem8Cpp::stack_pointer() const noexcept -> uint16_t {
    return stack_pointer_;
}

auto rem8Cpp::key(uint8_t key) const noexcept -> uint8_t {
    for (int i = 0; i < 16; i++) {
        if (key_binds_[i] == key) {
            return key_[i];
        }
    }
    return 0xFF;
}

auto rem8Cpp::key_pressed() const noexcept -> bool {
    return key_pressed_;
}

auto rem8Cpp::sound_timer() const noexcept -> uint8_t {
    return sound_timer_;
}

auto rem8Cpp::delay_timer() const noexcept -> uint8_t {
    return delay_timer_;
}


// Private methods - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

auto rem8Cpp::stack_push_pc() noexcept -> void {
    memory_[stack_pointer_] = program_counter_ & 0xFF;
    stack_pointer_--;
    memory_[stack_pointer_] = (program_counter_ >> 8) & 0xFF;
    stack_pointer_--;
}

auto rem8Cpp::stack_pull_pc() noexcept -> void {
    stack_pointer_++;
    uint8_t msb = memory_[stack_pointer_];
    stack_pointer_++;
    uint8_t lsb = memory_[stack_pointer_];
    program_counter_ = (msb << 8) | lsb;
}

auto rem8Cpp::sprite_set(uint16_t loc) noexcept -> void {
    uint8_t sprite_data[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
        0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
        0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
        0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
        0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
        0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
        0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
        0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
        0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
        0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
        0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
        0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
        0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
        0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
        0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
        0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
    };
    memcpy(&memory_[loc], sprite_data, sizeof(sprite_data));
}

auto rem8Cpp::sprite_draw(uint8_t X, uint8_t Y, char height) noexcept -> char {
    char unset = 0;

    std::size_t X_pos = X;
    std::size_t  Y_pos = Y;
    if (X >= width_ || Y >= height_) {
        X_pos = X % width_;
        Y_pos = Y % height_;
    }

    for (int y = 0; y < height; y++) {
        uint8_t sprite_row = memory_[I_register_ + y];
        if (Y_pos + y >= height_) break;
        for (int x = 0; x < 8; x++) {
            if (X_pos + x >= width_) continue;
            uint8_t init_val = screen_[X_pos + x + (Y_pos + y) * width_];
            screen_[X_pos + x + (Y_pos + y) * width_] ^= (sprite_row >> (7 - x)) & 0x01;
            if (screen_[X_pos + x + (Y_pos + y) * width_] == 0 && init_val != 0) unset = 1;
        }
    }

    return unset;
}

auto rem8Cpp::msb_reg_idx(uint8_t msb) const noexcept -> uint8_t {
    return msb & 0x0F;
}

auto rem8Cpp::lsb_reg_idx(uint8_t lsb) const noexcept -> uint8_t {
    return (lsb >> 4) & 0x0F;
}


// Instructions - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

// Execute machine language subroutine at address NNN
auto rem8Cpp::instr_0NNN() noexcept -> void {
    return;
}

// Clear the screen
auto rem8Cpp::instr_00E0() noexcept -> void {
    memset(screen_.data(), 0x00, screen_.size() * sizeof(uint8_t));
    return;
}

// Return from a subroutine
auto rem8Cpp::instr_00EE() noexcept -> void {
    stack_pull_pc();
}

// Jump to address NNN
auto rem8Cpp::instr_1NNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    program_counter_ = ((msb & 0x0F) << 8) | lsb;
}

// Execute subroutine starting at address NNN
auto rem8Cpp::instr_2NNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    stack_push_pc();
    program_counter_ = ((msb & 0x0F) << 8) | lsb;
}

// Skip following instruction if VX == NN
auto rem8Cpp::instr_3XNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    if (data_registers_[X] == lsb) program_counter_ += INSTR_SIZE;
}

// Skip following instruction if VX != NN
auto rem8Cpp::instr_4XNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    if (data_registers_[X] != lsb) program_counter_ += INSTR_SIZE;
}

// Skip following instruction if VX == VY
auto rem8Cpp::instr_5XY0(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    if (data_registers_[X] == data_registers_[Y]) program_counter_ += INSTR_SIZE;
}

// Store value NN in VX
auto rem8Cpp::instr_6XNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    data_registers_[X] = lsb;
}

// Add value NN to VX
auto rem8Cpp::instr_7XNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    data_registers_[msb & 0x0F] += lsb;
}

// Store the value of VY in VX
auto rem8Cpp::instr_8XY0(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    data_registers_[X] = data_registers_[Y];
}

// Set VX to VX | VY , reset 0x0F register
auto rem8Cpp::instr_8XY1(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    data_registers_[X] |= data_registers_[Y];
    data_registers_[0x0F] = 0x00;
}

// Set VX to VX & VY , reset 0x0F register
auto rem8Cpp::instr_8XY2(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    data_registers_[X] &= data_registers_[Y];
    data_registers_[0x0F] = 0x00;
}

// Set VX to VX ^ VY , reset 0x0F register
auto rem8Cpp::instr_8XY3(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    data_registers_[X] ^= data_registers_[Y];
    data_registers_[0x0F] = 0x00;
}

// Set VX to VX + VY , if overflow VF = 0x01
auto rem8Cpp::instr_8XY4(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t X_init = data_registers_[X];
    data_registers_[X] += data_registers_[Y];
    if (data_registers_[X] < X_init) data_registers_[0x0F] = 0x01;
    else data_registers_[0x0F] = 0x00;
}

// Set VX to VX - VY , if borrow VF = 0x00
auto rem8Cpp::instr_8XY5(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t X_init = data_registers_[X];
    data_registers_[X] -= data_registers_[Y];
    if (X_init >= data_registers_[Y]) data_registers_[0x0F] = 0x01;
    else data_registers_[0x0F] = 0x00;
}

// Set VX to VY >> 1 , set VF to VY LSb
auto rem8Cpp::instr_8XY6(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t lsbit  = data_registers_[Y] & 0x01;
    data_registers_[X] = data_registers_[Y] >> 1;
    data_registers_[0x0F] = lsbit;
}

/* Set VX to VY - VX , if borrow VF = 0x00 */
auto rem8Cpp::instr_8XY7(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t X_init = data_registers_[X];
    data_registers_[X] = data_registers_[Y] - data_registers_[X];
    if (X_init <= data_registers_[Y]) data_registers_[0x0F] = 0x01;
    else data_registers_[0x0F] = 0x00;
}

/* Set VX to VY << 1 , set VF to VY MSb */
auto rem8Cpp::instr_8XYE(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t msbit = ((data_registers_[Y] & 0x80) != 0);
    data_registers_[X] = data_registers_[Y] << 1;
    data_registers_[0x0F] = msbit;
}

/* Skip following instruction if VX != VY */
auto rem8Cpp::instr_9XY0(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    if (data_registers_[X] != data_registers_[Y]) program_counter_ += INSTR_SIZE;
}

/* Store NNN in addr register */
auto rem8Cpp::instr_ANNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    I_register_ = ((msb & 0x0F) << 8) | lsb;
}

/* Jump to address NNN + V0 */
auto rem8Cpp::instr_BNNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    program_counter_ = (((msb & 0x0F) << 8) | lsb) + data_registers_[0];
}

/* Set VX to random num with mask NN  */
auto rem8Cpp::instr_CXNN(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    data_registers_[X] = (rand() % 0xFF) & lsb;
}

/* Draw sprite at (VX, VY) 8px wide and Npx tall */
auto rem8Cpp::instr_DXYN(uint8_t msb, uint8_t lsb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t Y = lsb_reg_idx(lsb);
    uint8_t N = lsb & 0x0F;
    data_registers_[0x0F] = sprite_draw(data_registers_[X], data_registers_[Y], N);
}

/* Skip following instruction if key == VX */
auto rem8Cpp::instr_EX9E(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t X_val = data_registers_[X] & 0x0F;
    if (key_[X_val] == KEY_ON) program_counter_ += INSTR_SIZE;
}

/* Skip following instruction if key != VX */
auto rem8Cpp::instr_EXA1(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t X_val = data_registers_[X] & 0x0F;
    if (key_[X_val] == KEY_OFF) program_counter_ += INSTR_SIZE; 
}

/* Store delay timer into VX */
auto rem8Cpp::instr_FX07(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    data_registers_[X] = delay_timer_;
}

/* Wait for keypress and store result in VX */
auto rem8Cpp::instr_FX0A(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    for (int i = 0; i < 16; i++) {
        if (key_[i] == KEY_OFF && key_pressed_) {
            data_registers_[X] = i;
            return;
        }
    }
    program_counter_ -= 2;
}

/* Set delay timer to value of VX */
auto rem8Cpp::instr_FX15(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    delay_timer_ = data_registers_[X];
}

/* Set sound timer to value of VX */
auto rem8Cpp::instr_FX18(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    sound_timer_ = data_registers_[X];
}

/* Add value of VX to addr register  */
auto rem8Cpp::instr_FX1E(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    I_register_ += data_registers_[X];
}

/* Set addr register to sprite address of VX */
auto rem8Cpp::instr_FX29(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    I_register_ = data_registers_[X] * SPRITE_WIDTH + sprite_addr_;
}

/* Store BCD of VX at addr of addr register */
auto rem8Cpp::instr_FX33(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    uint8_t val = data_registers_[X];
    for (int i = 2; i >= 0; i--) {
        memory_[I_register_ + i] = val % 10;
        val /= 10;
    }
}

/* Store V0 to VX in memory starting at addr register */
auto rem8Cpp::instr_FX55(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    for (int i = 0; i <= X; i++) {
        memory_[I_register_ + i] = data_registers_[i];
    }
    I_register_ += X + 1;
}

/* Fill V0 to VX from memory starting at addr register */
auto rem8Cpp::instr_FX65(uint8_t msb) noexcept -> void {
    uint8_t X = msb_reg_idx(msb);
    for (int i = 0; i <= X; i++) {
        data_registers_[i] = memory_[I_register_ + i] ;
    }
    I_register_ += X + 1;
}

