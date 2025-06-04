/*  @file   emulator.h
 *  @brief  rem8C++ emulator interface.
 *  @author Ryan V. Ngo
 */

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

rem8Cpp::rem8Cpp() 
  : width_(REM8CPP_SCREEN_WIDTH),
    height_(REM8CPP_SCREEN_HEIGHT),
    screen_(width_ * height_, 0x00),
    program_counter_(0x200),
    stack_pointer_(0x200 - 0x01),
    sprite_addr_(FONT_SET_ADDR),
    memory_(REM8CPP_MAX_ADDR, 0x00)
{ 
  _sprite_set(sprite_addr_);
  key_binds_[0x1] = '1'; key_binds_[0x2] = '2'; key_binds_[0x3] = '3'; key_binds_[0xC] = '4';
  key_binds_[0x4] = 'q'; key_binds_[0x5] = 'w'; key_binds_[0x6] = 'e'; key_binds_[0xD] = 'r';
  key_binds_[0x7] = 'a'; key_binds_[0x8] = 's'; key_binds_[0x9] = 'd'; key_binds_[0xE] = 'f';
  key_binds_[0xA] = 'z'; key_binds_[0x0] = 'x'; key_binds_[0xB] = 'c'; key_binds_[0xF] = 'v';
}

void rem8Cpp::_sprite_set(uint16_t loc) {
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

void rem8Cpp::cycle() {
  uint8_t msb = memory_[program_counter_++];
  uint8_t lsb = memory_[program_counter_++];
  
  switch (msb & 0xF0) {
    case 0x00:
      switch (msb << 8 | lsb) {
        case 0x00E0:
          _instr_00E0(); break;
        case 0x00EE:
          _instr_00EE(); break;
        default:
          _instr_0NNN(); break;
      }
      break;
    case 0x10:
      _instr_1NNN(msb, lsb); break;
    case 0x20:
      _instr_2NNN(msb, lsb); break;
    case 0x30:
      _instr_3XNN(msb, lsb); break;
    case 0x40:
      _instr_4XNN(msb, lsb); break;
    case 0x50:
      _instr_5XY0(msb, lsb); break;
    case 0x60:
      _instr_6XNN(msb, lsb); break;
    case 0x70:
      _instr_7XNN(msb, lsb); break;
    case 0x80:
      switch (lsb & 0x0F) {
        case 0x00:
          _instr_8XY0(msb, lsb); break;
        case 0x01:
          _instr_8XY1(msb, lsb); break;
        case 0x02:
          _instr_8XY2(msb, lsb); break;
        case 0x03:
          _instr_8XY3(msb, lsb); break;
        case 0x04:
          _instr_8XY4(msb, lsb); break;
        case 0x05:
          _instr_8XY5(msb, lsb); break;
        case 0x06:
          _instr_8XY6(msb, lsb); break;
        case 0x07:
          _instr_8XY7(msb, lsb); break;
        case 0x0E:
          _instr_8XYE(msb, lsb); break;
        default: break;
      }
      break;
    case 0x90:
      _instr_9XY0(msb, lsb); break;
    case 0xA0:
      _instr_ANNN(msb, lsb); break;
    case 0xB0:
      _instr_BNNN(msb, lsb); break;
    case 0xC0:
      _instr_CXNN(msb, lsb); break;
    case 0xD0:
      _instr_DXYN(msb, lsb); break;
    case 0xE0:
      switch (lsb) {
        case 0x9E:
          _instr_EX9E(msb); break;
        case 0xA1:
          _instr_EXA1(msb); break;
        default: break;
      }
      break;
    case 0xF0:
      switch (lsb) {
        case 0x07:
          _instr_FX07(msb); break;
        case 0x0A:
          _instr_FX0A(msb); break;
        case 0x15:
          _instr_FX15(msb); break;
        case 0x18:
          _instr_FX18(msb); break;
        case 0x1E:
          _instr_FX1E(msb); break;
        case 0x29:
          _instr_FX29(msb); break;
        case 0x33:
          _instr_FX33(msb); break;
        case 0x55:
          _instr_FX55(msb); break;
        case 0x65:
          _instr_FX65(msb); break;
        default: break;
      }
      break;
    default: break;
  }
}

const std::vector<uint8_t>& rem8Cpp::get_screen() const {
  return screen_;
}

std::vector<unsigned char> rem8Cpp::get_screen_rgb() const {
  std::vector<unsigned char> screen_rgb(width_ * height_ * 3);
  for (std::size_t i = 0; i < screen_.size(); i++) {
    memset(screen_rgb.data() + i * 3, (screen_[i] & 0x01) * UCHAR_MAX, sizeof(unsigned char) * 3);
  }
  return screen_rgb;
}

void rem8Cpp::set_program_counter(uint16_t addr) {
  if (addr >= REM8CPP_MAX_ADDR) return;
  program_counter_ = addr;
}

void rem8Cpp::load_rom(uint16_t addr, std::vector<char> data, size_t size) {
  if (addr + size >= REM8CPP_MAX_ADDR) return;
  memcpy(&memory_[addr], data.data(), size);
}

void rem8Cpp::update_timers() {
  if (delay_timer_ > 0) delay_timer_--;
  if (sound_timer_ > 0) sound_timer_--;
}

void rem8Cpp::set_key(uint8_t key) {
  for (int i = 0; i < 16; i++) {
    if (key_binds_[i] == key) {
      key_[i] = KEY_ON;
      key_pressed_ = true;
      break;
    }
  }
}

void rem8Cpp::unset_key(uint8_t key) {
  for (int i = 0; i < 16; i++) {
    if (key_binds_[i] == key) {
      key_[i] = KEY_OFF;
      key_pressed_ = false;
      break;
    }
  }
}

std::size_t rem8Cpp::width() const {
  return width_;
}

std::size_t rem8Cpp::height() const {
  return height_;
}

uint16_t rem8Cpp::current_program_counter() const {
  return program_counter_;
}

uint8_t rem8Cpp::read_memory(uint16_t addr) const {
  return memory_[addr];
}

void rem8Cpp::_stack_push_pc() {
  memory_[stack_pointer_] = program_counter_ & 0xFF;
  stack_pointer_--;
  memory_[stack_pointer_] = (program_counter_ >> 8) & 0xFF;
  stack_pointer_--;
}

void rem8Cpp::_stack_pull_pc() {
  stack_pointer_++;
  uint8_t msb = memory_[stack_pointer_];
  stack_pointer_++;
  uint8_t lsb = memory_[stack_pointer_];
  program_counter_ = (msb << 8) | lsb;
}

char rem8Cpp::_sprite_draw(uint8_t X, uint8_t Y, char height) {
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

uint8_t _msb_reg_idx(uint8_t msb) {
  return msb & 0x0F;
}

uint8_t _lsb_reg_idx(uint8_t lsb) {
  return (lsb >> 4) & 0x0F;
}

/* Execute machine language subroutine at address NNN */
void rem8Cpp::_instr_0NNN() {
  return;
}

/* Clear the screen */
void rem8Cpp::_instr_00E0() {
  memset(screen_.data(), 0x00, screen_.size() * sizeof(uint8_t));
  return;
}

/* Return from a subroutine */
void rem8Cpp::_instr_00EE() {
  _stack_pull_pc();
}

/* Jump to address NNN */
void rem8Cpp::_instr_1NNN(uint8_t msb, uint8_t lsb) {
  program_counter_ = ((msb & 0x0F) << 8) | lsb;
}

/* Execute subroutine starting at address NNN */
void rem8Cpp::_instr_2NNN(uint8_t msb, uint8_t lsb) {
  _stack_push_pc();
  program_counter_ = ((msb & 0x0F) << 8) | lsb;
}

/* Skip following instruction if VX == NN */
void rem8Cpp::_instr_3XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  if (data_registers_[X] == lsb) program_counter_ += INSTR_SIZE;
}

/* Skip following instruction if VX != NN */
void rem8Cpp::_instr_4XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  if (data_registers_[X] != lsb) program_counter_ += INSTR_SIZE;
}

/* Skip following instruction if VX == VY*/
void rem8Cpp::_instr_5XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  if (data_registers_[X] == data_registers_[Y]) program_counter_ += INSTR_SIZE;
}

/* Store value NN in VX */
void rem8Cpp::_instr_6XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  data_registers_[X] = lsb;
}

/* Add value NN to VX */
void rem8Cpp::_instr_7XNN(uint8_t msb, uint8_t lsb) {
  data_registers_[msb & 0x0F] += lsb;
}

/* Store the value of VY in VX */
void rem8Cpp::_instr_8XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  data_registers_[X] = data_registers_[Y];
}

/* Set VX to VX | VY , reset 0x0F register */
void rem8Cpp::_instr_8XY1(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  data_registers_[X] |= data_registers_[Y];
  data_registers_[0x0F] = 0x00;
}

/* Set VX to VX & VY , reset 0x0F register */
void rem8Cpp::_instr_8XY2(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  data_registers_[X] &= data_registers_[Y];
  data_registers_[0x0F] = 0x00;
}

/* Set VX to VX ^ VY , reset 0x0F register */
void rem8Cpp::_instr_8XY3(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  data_registers_[X] ^= data_registers_[Y];
  data_registers_[0x0F] = 0x00;
}

/* Set VX to VX + VY , if overflow VF = 0x01 */
void rem8Cpp::_instr_8XY4(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = data_registers_[X];
  data_registers_[X] += data_registers_[Y];
  if (data_registers_[X] < X_init) data_registers_[0x0F] = 0x01;
  else data_registers_[0x0F] = 0x00;
}

/* Set VX to VX - VY , if borrow VF = 0x00 */
void rem8Cpp::_instr_8XY5(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = data_registers_[X];
  data_registers_[X] -= data_registers_[Y];
  if (X_init >= data_registers_[Y]) data_registers_[0x0F] = 0x01;
  else data_registers_[0x0F] = 0x00;
}

/* Set VX to VY >> 1 , set VF to VY & 0x01 */
void rem8Cpp::_instr_8XY6(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t lsbit  = data_registers_[Y] & 0x01;
  data_registers_[X] = data_registers_[Y] >> 1;
  data_registers_[0x0F] = lsbit;
}

/* Set VX to VY - VX , if borrow VF = 0x00 */
void rem8Cpp::_instr_8XY7(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = data_registers_[X];
  data_registers_[X] = data_registers_[Y] - data_registers_[X];
  if (X_init <= data_registers_[Y]) data_registers_[0x0F] = 0x01;
  else data_registers_[0x0F] = 0x00;
}

/* Set VX to VY << 1 , set VF to VY & 0x01 */
void rem8Cpp::_instr_8XYE(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t msbit = ((data_registers_[Y] & 0x80) != 0);
  data_registers_[X] = data_registers_[Y] << 1;
  data_registers_[0x0F] = msbit;
}

/* Skip following instruction if VX != VY */
void rem8Cpp::_instr_9XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  if (data_registers_[X] != data_registers_[Y]) program_counter_ += INSTR_SIZE;
}

/* Store NNN in addr register */
void rem8Cpp::_instr_ANNN(uint8_t msb, uint8_t lsb) {
  I_register_ = ((msb & 0x0F) << 8) | lsb;
}

/* Jump to address NNN + V0 */
void rem8Cpp::_instr_BNNN(uint8_t msb, uint8_t lsb) {
  program_counter_ = (((msb & 0x0F) << 8) | lsb) + data_registers_[0];
}

/* Set VX to random num with mask NN  */
void rem8Cpp::_instr_CXNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  data_registers_[X] = (rand() % 0xFF) & lsb;
}

/* Draw sprite at (VX, VY) 8px wide and Npx tall */
void rem8Cpp::_instr_DXYN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t N = lsb & 0x0F;
  data_registers_[0x0F] = _sprite_draw(data_registers_[X], data_registers_[Y], N);
}

/* Skip following instruction if key == VX */
void rem8Cpp::_instr_EX9E(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t X_val = data_registers_[X] & 0x0F;
  if (key_[X_val] == KEY_ON) program_counter_ += INSTR_SIZE;
}

/* Skip following instruction if key != VX */
void rem8Cpp::_instr_EXA1(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t X_val = data_registers_[X] & 0x0F;
  if (key_[X_val] == KEY_OFF) program_counter_ += INSTR_SIZE; 
}

/* Store delay timer into VX */
void rem8Cpp::_instr_FX07(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  data_registers_[X] = delay_timer_;
}

/* Wait for keypress and store result in VX */
void rem8Cpp::_instr_FX0A(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i < 16; i++) {
    if (key_[i] == KEY_OFF && key_pressed_) {
      data_registers_[X] = i;
      return;
    }
  }
  program_counter_ -= 2;
}

/* Set delay timer to value of VX */
void rem8Cpp::_instr_FX15(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  delay_timer_ = data_registers_[X];
}

/* Set sound timer to value of VX */
void rem8Cpp::_instr_FX18(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  sound_timer_ = data_registers_[X];
}

/* Add value of VX to addr register  */
void rem8Cpp::_instr_FX1E(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  I_register_ += data_registers_[X];
}

/* Set addr register to sprite address of VX */
void rem8Cpp::_instr_FX29(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  I_register_ = data_registers_[X] * SPRITE_WIDTH + sprite_addr_;
}

/* Store BCD of VX at addr of addr register */
void rem8Cpp::_instr_FX33(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t val = data_registers_[X];
  for (int i = 2; i >= 0; i--) {
    memory_[I_register_ + i] = val % 10;
    val /= 10;
  }
}

/* Store V0 to VX in memory starting at addr register */
void rem8Cpp::_instr_FX55(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i <= X; i++) {
    memory_[I_register_ + i] = data_registers_[i];
  }
  I_register_ += X + 1;
}

/* File V0 to VX from memory starting at addr register */
void rem8Cpp::_instr_FX65(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i <= X; i++) {
    data_registers_[i] = memory_[I_register_ + i] ;
  }
  I_register_ += X + 1;
}

