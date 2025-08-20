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


//---------------------------------------------------
// rem8Cpp
//---------------------------------------------------

rem8Cpp::rem8Cpp() 
  : m_width(REM8CPP_SCREEN_WIDTH),
    m_height(REM8CPP_SCREEN_HEIGHT),
    m_screen(m_width * m_height, 0x00),
    m_program_counter(0x200),
    m_stack_pointer(0x200 - 0x01),
    m_sprite_addr(FONT_SET_ADDR),
    m_key_pressed(false),
    m_sound_timer(0x00),
    m_delay_timer(0x00),
    m_memory(REM8CPP_MAX_ADDR, 0x00)
{ 
  _sprite_set(m_sprite_addr);
  m_key_binds[0x1] = '1'; m_key_binds[0x2] = '2'; m_key_binds[0x3] = '3'; m_key_binds[0xC] = '4';
  m_key_binds[0x4] = 'q'; m_key_binds[0x5] = 'w'; m_key_binds[0x6] = 'e'; m_key_binds[0xD] = 'r';
  m_key_binds[0x7] = 'a'; m_key_binds[0x8] = 's'; m_key_binds[0x9] = 'd'; m_key_binds[0xE] = 'f';
  m_key_binds[0xA] = 'z'; m_key_binds[0x0] = 'x'; m_key_binds[0xB] = 'c'; m_key_binds[0xF] = 'v';
  memset(m_key, 0x00, sizeof(uint8_t) * 0x10);
}

void rem8Cpp::cycle() {
  uint8_t msb = m_memory[m_program_counter++];
  uint8_t lsb = m_memory[m_program_counter++];
  
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
  return m_screen;
}

void rem8Cpp::get_screen_rgb(std::vector<unsigned char>& buffer) const {
  for (std::size_t i = 0; i < m_screen.size(); i++) {
    memset(buffer.data() + i * 3, (m_screen[i] & 0x01) * UCHAR_MAX, sizeof(unsigned char) * 3);
  }
}

void rem8Cpp::set_program_counter(uint16_t addr) {
  if (addr >= REM8CPP_MAX_ADDR) return;
  m_program_counter = addr;
}

void rem8Cpp::load_rom(uint16_t addr, std::vector<char> data, size_t size) {
  if (addr + size >= REM8CPP_MAX_ADDR) return;
  memset(m_memory.data(), 0x00, sizeof(uint8_t) * m_memory.size());
  memcpy(&m_memory[addr], data.data(), size);
}

void rem8Cpp::update_timers() {
  if (m_delay_timer > 0) m_delay_timer--;
  if (m_sound_timer > 0) m_sound_timer--;
}

void rem8Cpp::set_key(uint8_t key) {
  for (int i = 0; i < 16; i++) {
    if (m_key_binds[i] == key) {
      m_key[i] = KEY_ON;
      m_key_pressed = true;
      break;
    }
  }
}

void rem8Cpp::unset_key(uint8_t key) {
  for (int i = 0; i < 16; i++) {
    if (m_key_binds[i] == key) {
      m_key[i] = KEY_OFF;
      m_key_pressed = false;
      break;
    }
  }
}


// Diagnositc methods

std::size_t rem8Cpp::width() const {
  return m_width;
}

std::size_t rem8Cpp::height() const {
  return m_height;
}

uint16_t rem8Cpp::program_counter() const {
  return m_program_counter;
}

uint8_t rem8Cpp::read_memory(uint16_t addr) const {
  return m_memory[addr];
}

uint8_t rem8Cpp::data_register(uint8_t reg) const {
  if (reg > 0x10) return 0xFF;
  return m_data_registers[reg];
}

uint16_t rem8Cpp::I_register() const {
  return m_I_register;
}

uint16_t rem8Cpp::stack_pointer() const {
  return m_stack_pointer;
}

uint8_t rem8Cpp::key(uint8_t key) const {
  for (int i = 0; i < 16; i++) {
    if (m_key_binds[i] == key) {
      return m_key[i];
    }
  }
  return 0xFF;
}

bool rem8Cpp::key_pressed() const {
  return m_key_pressed;
}

uint8_t rem8Cpp::sound_timer() const {
  return m_sound_timer;
}

uint8_t rem8Cpp::delay_timer() const {
  return m_delay_timer;
}


// Private methods - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

void rem8Cpp::_stack_push_pc() {
  m_memory[m_stack_pointer] = m_program_counter & 0xFF;
  m_stack_pointer--;
  m_memory[m_stack_pointer] = (m_program_counter >> 8) & 0xFF;
  m_stack_pointer--;
}

void rem8Cpp::_stack_pull_pc() {
  m_stack_pointer++;
  uint8_t msb = m_memory[m_stack_pointer];
  m_stack_pointer++;
  uint8_t lsb = m_memory[m_stack_pointer];
  m_program_counter = (msb << 8) | lsb;
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
  memcpy(&m_memory[loc], sprite_data, sizeof(sprite_data));
}

char rem8Cpp::_sprite_draw(uint8_t X, uint8_t Y, char height) {
  char unset = 0;

  std::size_t X_pos = X;
  std::size_t  Y_pos = Y;
  if (X >= m_width || Y >= m_height) {
    X_pos = X % m_width;
    Y_pos = Y % m_height;
  }

  for (int y = 0; y < height; y++) {
    uint8_t sprite_row = m_memory[m_I_register + y];
    if (Y_pos + y >= m_height) break;
    for (int x = 0; x < 8; x++) {
      if (X_pos + x >= m_width) continue;
      uint8_t init_val = m_screen[X_pos + x + (Y_pos + y) * m_width];
      m_screen[X_pos + x + (Y_pos + y) * m_width] ^= (sprite_row >> (7 - x)) & 0x01;
      if (m_screen[X_pos + x + (Y_pos + y) * m_width] == 0 && init_val != 0) unset = 1;
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


// Instructions - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

// Execute machine language subroutine at address NNN
void rem8Cpp::_instr_0NNN() {
  return;
}

// Clear the screen
void rem8Cpp::_instr_00E0() {
  memset(m_screen.data(), 0x00, m_screen.size() * sizeof(uint8_t));
  return;
}

// Return from a subroutine
void rem8Cpp::_instr_00EE() {
  _stack_pull_pc();
}

// Jump to address NNN
void rem8Cpp::_instr_1NNN(uint8_t msb, uint8_t lsb) {
  m_program_counter = ((msb & 0x0F) << 8) | lsb;
}

// Execute subroutine starting at address NNN
void rem8Cpp::_instr_2NNN(uint8_t msb, uint8_t lsb) {
  _stack_push_pc();
  m_program_counter = ((msb & 0x0F) << 8) | lsb;
}

// Skip following instruction if VX == NN
void rem8Cpp::_instr_3XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  if (m_data_registers[X] == lsb) m_program_counter += INSTR_SIZE;
}

// Skip following instruction if VX != NN
void rem8Cpp::_instr_4XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  if (m_data_registers[X] != lsb) m_program_counter += INSTR_SIZE;
}

// Skip following instruction if VX == VY
void rem8Cpp::_instr_5XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  if (m_data_registers[X] == m_data_registers[Y]) m_program_counter += INSTR_SIZE;
}

// Store value NN in VX
void rem8Cpp::_instr_6XNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  m_data_registers[X] = lsb;
}

// Add value NN to VX
void rem8Cpp::_instr_7XNN(uint8_t msb, uint8_t lsb) {
  m_data_registers[msb & 0x0F] += lsb;
}

// Store the value of VY in VX
void rem8Cpp::_instr_8XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  m_data_registers[X] = m_data_registers[Y];
}

// Set VX to VX | VY , reset 0x0F register
void rem8Cpp::_instr_8XY1(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  m_data_registers[X] |= m_data_registers[Y];
  m_data_registers[0x0F] = 0x00;
}

// Set VX to VX & VY , reset 0x0F register
void rem8Cpp::_instr_8XY2(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  m_data_registers[X] &= m_data_registers[Y];
  m_data_registers[0x0F] = 0x00;
}

// Set VX to VX ^ VY , reset 0x0F register
void rem8Cpp::_instr_8XY3(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  m_data_registers[X] ^= m_data_registers[Y];
  m_data_registers[0x0F] = 0x00;
}

// Set VX to VX + VY , if overflow VF = 0x01
void rem8Cpp::_instr_8XY4(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = m_data_registers[X];
  m_data_registers[X] += m_data_registers[Y];
  if (m_data_registers[X] < X_init) m_data_registers[0x0F] = 0x01;
  else m_data_registers[0x0F] = 0x00;
}

// Set VX to VX - VY , if borrow VF = 0x00
void rem8Cpp::_instr_8XY5(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = m_data_registers[X];
  m_data_registers[X] -= m_data_registers[Y];
  if (X_init >= m_data_registers[Y]) m_data_registers[0x0F] = 0x01;
  else m_data_registers[0x0F] = 0x00;
}

// Set VX to VY >> 1 , set VF to VY LSb
void rem8Cpp::_instr_8XY6(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t lsbit  = m_data_registers[Y] & 0x01;
  m_data_registers[X] = m_data_registers[Y] >> 1;
  m_data_registers[0x0F] = lsbit;
}

/* Set VX to VY - VX , if borrow VF = 0x00 */
void rem8Cpp::_instr_8XY7(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t X_init = m_data_registers[X];
  m_data_registers[X] = m_data_registers[Y] - m_data_registers[X];
  if (X_init <= m_data_registers[Y]) m_data_registers[0x0F] = 0x01;
  else m_data_registers[0x0F] = 0x00;
}

/* Set VX to VY << 1 , set VF to VY MSb */
void rem8Cpp::_instr_8XYE(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t msbit = ((m_data_registers[Y] & 0x80) != 0);
  m_data_registers[X] = m_data_registers[Y] << 1;
  m_data_registers[0x0F] = msbit;
}

/* Skip following instruction if VX != VY */
void rem8Cpp::_instr_9XY0(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  if (m_data_registers[X] != m_data_registers[Y]) m_program_counter += INSTR_SIZE;
}

/* Store NNN in addr register */
void rem8Cpp::_instr_ANNN(uint8_t msb, uint8_t lsb) {
  m_I_register = ((msb & 0x0F) << 8) | lsb;
}

/* Jump to address NNN + V0 */
void rem8Cpp::_instr_BNNN(uint8_t msb, uint8_t lsb) {
  m_program_counter = (((msb & 0x0F) << 8) | lsb) + m_data_registers[0];
}

/* Set VX to random num with mask NN  */
void rem8Cpp::_instr_CXNN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  m_data_registers[X] = (rand() % 0xFF) & lsb;
}

/* Draw sprite at (VX, VY) 8px wide and Npx tall */
void rem8Cpp::_instr_DXYN(uint8_t msb, uint8_t lsb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t Y = _lsb_reg_idx(lsb);
  uint8_t N = lsb & 0x0F;
  m_data_registers[0x0F] = _sprite_draw(m_data_registers[X], m_data_registers[Y], N);
}

/* Skip following instruction if key == VX */
void rem8Cpp::_instr_EX9E(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t X_val = m_data_registers[X] & 0x0F;
  if (m_key[X_val] == KEY_ON) m_program_counter += INSTR_SIZE;
}

/* Skip following instruction if key != VX */
void rem8Cpp::_instr_EXA1(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t X_val = m_data_registers[X] & 0x0F;
  if (m_key[X_val] == KEY_OFF) m_program_counter += INSTR_SIZE; 
}

/* Store delay timer into VX */
void rem8Cpp::_instr_FX07(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  m_data_registers[X] = m_delay_timer;
}

/* Wait for keypress and store result in VX */
void rem8Cpp::_instr_FX0A(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i < 16; i++) {
    if (m_key[i] == KEY_OFF && m_key_pressed) {
      m_data_registers[X] = i;
      return;
    }
  }
  m_program_counter -= 2;
}

/* Set delay timer to value of VX */
void rem8Cpp::_instr_FX15(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  m_delay_timer = m_data_registers[X];
}

/* Set sound timer to value of VX */
void rem8Cpp::_instr_FX18(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  m_sound_timer = m_data_registers[X];
}

/* Add value of VX to addr register  */
void rem8Cpp::_instr_FX1E(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  m_I_register += m_data_registers[X];
}

/* Set addr register to sprite address of VX */
void rem8Cpp::_instr_FX29(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  m_I_register = m_data_registers[X] * SPRITE_WIDTH + m_sprite_addr;
}

/* Store BCD of VX at addr of addr register */
void rem8Cpp::_instr_FX33(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  uint8_t val = m_data_registers[X];
  for (int i = 2; i >= 0; i--) {
    m_memory[m_I_register + i] = val % 10;
    val /= 10;
  }
}

/* Store V0 to VX in memory starting at addr register */
void rem8Cpp::_instr_FX55(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i <= X; i++) {
    m_memory[m_I_register + i] = m_data_registers[i];
  }
  m_I_register += X + 1;
}

/* Fill V0 to VX from memory starting at addr register */
void rem8Cpp::_instr_FX65(uint8_t msb) {
  uint8_t X = _msb_reg_idx(msb);
  for (int i = 0; i <= X; i++) {
    m_data_registers[i] = m_memory[m_I_register + i] ;
  }
  m_I_register += X + 1;
}

