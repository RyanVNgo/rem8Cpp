
#include "gtest/gtest.h"

#include "emulator.h"


// Helper Functions - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

void load_instruction_at_pc(rem8Cpp& emulator, uint16_t instr) {
  std::vector<char> instr_data;
  instr_data.push_back((instr & 0xFF00) >> 8);
  instr_data.push_back(instr & 0x00FF);
  emulator.load_rom(emulator.program_counter(), instr_data, sizeof(char) * 2);
}

void set_register(rem8Cpp& emulator, uint8_t reg, uint8_t val) {
  if (reg > 0x0F) return; // Only valid registers are 0x00 to 0x0F
  emulator.set_program_counter(0x200); // Set PC to a valid location
  auto instr = 0x6000 | (reg << 8) | val; // Create instruction to set register
  load_instruction_at_pc(emulator, instr);
  emulator.cycle(); // Execute the instruction
}

void set_delay_timer(rem8Cpp& emulator, uint8_t reg) {
  emulator.set_program_counter(0x200); // Set PC to a valid location
  auto instr = 0xF015 | (reg << 8); // Create instruction to set delay timer
  load_instruction_at_pc(emulator, instr);
  emulator.cycle(); // Execute the instruction
}

// Tests - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

TEST(rem8Cpp, get_screen__blank) {
  auto em = rem8Cpp();
  auto screen_data = em.get_screen();
  for (const auto& val : screen_data) {
    EXPECT_EQ(val, 0x00);
  }
}

// Null instruction cycle
TEST(rem8Cpp, cycle__blank) {
  auto em = rem8Cpp();
  uint16_t init_pc = em.program_counter();

  em.cycle();

  uint16_t curr_pc = em.program_counter();
  EXPECT_NE(init_pc, curr_pc);
  EXPECT_EQ(init_pc + 0x02, curr_pc);
}

// Exec subroutine at NNN
TEST(rem8Cpp_instr, exec_0NNN) {
  GTEST_SKIP();
}

// Screen Clear
TEST(rem8Cpp_instr, exec_00E0) {
  auto em = rem8Cpp();
  auto instr = 0x00E0;
  load_instruction_at_pc(em, instr);
  em.cycle();

  // screen cleared
  auto screen_data = em.get_screen();
  for (const auto& val : screen_data) {
    EXPECT_EQ(val, 0x00);
  }
}

// Return from subroutine
TEST(rem8Cpp_instr, exec_00EE) {
  GTEST_SKIP();
}

// Jump to address NNN
TEST(rem8Cpp_instr, exec_1NNN) {
  auto em = rem8Cpp();
  auto instr = 0x1234;
  load_instruction_at_pc(em, instr);
  em.cycle();
  
  // jumped to address
  auto pc = em.program_counter();
  EXPECT_EQ(pc, instr & 0x0FFF);
}

// Execute subroutine at address NNN, pushes current SP to stack
TEST(rem8Cpp_instr, exec_2NNN) {
  auto em = rem8Cpp();
  auto instr = 0x2234;

  auto pc_shifted = em.program_counter() + 0x2;
  auto sp_init = em.stack_pointer();
  load_instruction_at_pc(em, instr);

  em.cycle();

  // stack pointer moved
  auto sp_curr = em.stack_pointer();
  EXPECT_NE(sp_init, sp_curr);
  EXPECT_EQ(sp_init, sp_curr + 2);

  // previous SP (shifted) pushed to stack
  EXPECT_EQ(em.read_memory(sp_curr + 1), pc_shifted >> 8 & 0xFF);
  EXPECT_EQ(em.read_memory(sp_curr + 2), pc_shifted & 0xFF);

  // moved to new address
  auto pc_curr = em.program_counter();
  EXPECT_EQ(pc_curr, instr & 0x0FFF);
}

// Skip following instruction if VX == NN
TEST(rem8Cpp_instr, exec_3XNN) {
  GTEST_SKIP();
}

// Skip following instruction if VX != NN
TEST(rem8Cpp_instr, exec_4XNN) {
  GTEST_SKIP();
}

// Skip following instruction if VX == VY
TEST(rem8Cpp_instr, exec_5XY0) {
  GTEST_SKIP();
}

// Store value of NN in VX
TEST(rem8Cpp_instr, exec_6XNN) {
  auto em = rem8Cpp();
  auto instr = 0x6402;
  load_instruction_at_pc(em, instr);
  em.cycle();
  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x02);
}

// Add value of NN in VX
TEST(rem8Cpp_instr, exec_7XNN) {
  auto em = rem8Cpp();
  auto instr = 0x7402; 
  set_register(em, 0x04, 0x01);
  load_instruction_at_pc(em, instr);

  em.cycle();
  
  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x03);
}

// Store value of VY in VX
TEST(rem8Cpp_instr, exec_8XY0) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x05);
  set_register(em, 0x05, 0x10);

  auto instr = 0x8450;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x10);
}

// Set VX to VX | VY and reset 0x0F register
TEST(rem8Cpp_instr, exec_8XY1) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x0F);
  set_register(em, 0x05, 0xF0);

  auto instr = 0x8451;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0xFF);
}

// Set VX to VX & VY and reset 0x0F register
TEST(rem8Cpp_instr, exec_8XY2) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x0F);
  set_register(em, 0x05, 0xF0);
  set_register(em, 0x0F, 0x01);

  auto instr = 0x8452;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x00);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x00);
}

// Set VX to VX ^ VY and reset 0x0F register
TEST(rem8Cpp_instr, exec_8XY3) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x0F);
  set_register(em, 0x05, 0xF0);
  set_register(em, 0x0F, 0x01);

  auto instr = 0x8453;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0xFF);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x00);
}

// Set VX to VX + VY and if overflow VF = 0x01
TEST(rem8Cpp_instr, exec_8XY4) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0xFF); 
  set_register(em, 0x05, 0x10); 

  auto instr = 0x8454; // Set V4 to V4 + V5
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x0F);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x01);
}

// Set VX to VX - VY and if borrow VF = 0x00
TEST(rem8Cpp_instr, exec_8XY5) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x10);
  set_register(em, 0x05, 0x0E);
  set_register(em, 0x0F, 0x01);

  auto instr = 0x8545;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x05);
  EXPECT_EQ(reg_val, 0xFE);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x00);
}

// Set VX to VY >> 1 and set VF to VY LSb
TEST(rem8Cpp_instr, exec_8XY6) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x02);
  set_register(em, 0x05, 0x03);

  auto instr = 0x8456;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x01);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x03 & 0x01);
}

// Set VX to VY - VX and if borrow VF = 0x00
TEST(rem8Cpp_instr, exec_8XY7) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x10);
  set_register(em, 0x05, 0x0E);
  set_register(em, 0x0F, 0x01);

  auto instr = 0x8457;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0xFE);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x00);
}

// Set VX to VY << 1 and set VF to VY MSb
TEST(rem8Cpp_instr, exec_8XYE) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x01);
  set_register(em, 0x05, 0xC0);

  auto instr = 0x845E;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x80);

  auto vF_val = em.data_register(0x0F);
  EXPECT_EQ(vF_val, 0x01);
}

// Skip following instruction if VX != VY
TEST(rem8Cpp_instr, exec_9XY0) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x01); 
  set_register(em, 0x05, 0x02); 

  auto instr = 0x9450;
  load_instruction_at_pc(em, instr);
  auto pc_init = em.program_counter();

  em.cycle();

  // Program counter should have skipped 
  auto pc_curr = em.program_counter();
  EXPECT_EQ(pc_init + 4, pc_curr);
}

// Store NNN in addr register
TEST(rem8Cpp_instr, exec_ANNN) {
  auto em = rem8Cpp();
  auto instr = 0xA123;
  load_instruction_at_pc(em, instr);

  em.cycle();

  auto i_reg = em.I_register();
  EXPECT_EQ(i_reg, 0x0123); 
}

// Jump to address NNN + V0
TEST(rem8Cpp_instr, exec_BNNN) {
  auto em = rem8Cpp();
  set_register(em, 0x00, 0x10);
  auto instr = 0xB123;
  load_instruction_at_pc(em, instr);

  em.cycle();

  auto pc = em.program_counter();
  EXPECT_EQ(pc, 0x0123 + 0x10);
}

// Set VX to random num with mask NN 
TEST(rem8Cpp_instr, exec_CXNN) {
  auto em = rem8Cpp();
  auto mask = 0xAA;
  auto instr = 0xC400 | mask;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val & !0xAA, 0x00);
}

// Draw sprite at (VX, VY) 8px wide and Npx tall
TEST(rem8Cpp_instr, exec_DXYN) {
  GTEST_SKIP();
}

// Skip following instruction if key == VX
TEST(rem8Cpp_instr, exec_EX9E) {
  GTEST_SKIP();
}

// Skip following instruction if key != VX
TEST(rem8Cpp_instr, exec_EXA1) {
  GTEST_SKIP();
}

// Store delay timer into VX
TEST(rem8Cpp_instr, exec_FX07) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x00); 

  // set delay timer
  set_register(em, 0x05, 0x04);
  set_delay_timer(em, 0x05);

  auto instr = 0xF407; // Store delay timer in V4
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto reg_val = em.data_register(0x04);
  EXPECT_EQ(reg_val, 0x04);
}

// Wait for keypress and store result in VX
TEST(rem8Cpp_instr, exec_FX0A) {
  GTEST_SKIP();
}

// Set delay timer to value of VX
TEST(rem8Cpp_instr, exec_FX15) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x05);

  auto instr = 0xF415;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto delay_timer = em.delay_timer();
  EXPECT_EQ(delay_timer, 0x05);
}

// Set sound timer to value of VX
TEST(rem8Cpp_instr, exec_FX18) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x05);

  auto instr = 0xF418;
  load_instruction_at_pc(em, instr);
  em.cycle();

  auto sound_timer = em.sound_timer();
  EXPECT_EQ(sound_timer, 0x05);
}

// Add value of VX to addr register
TEST(rem8Cpp_instr, exec_FX1E) {
  auto em = rem8Cpp();
  set_register(em, 0x04, 0x05);
  em.set_program_counter(0x200);

  auto instr = 0xF41E;
  load_instruction_at_pc(em, instr);
  auto i_reg_init = em.I_register();

  em.cycle();

  auto i_reg_curr = em.I_register();
  EXPECT_EQ(i_reg_init + 0x05, i_reg_curr);
}

// Set addr register to sprite address of VX
TEST(rem8Cpp_instr, exec_FX29) {
  GTEST_SKIP();
}

// Store BCD of VX at addr of addr register
TEST(rem8Cpp_instr, exec_FX33) {
  GTEST_SKIP();
}

// Store V0 to VX in memory starting at addr register
TEST(rem8Cpp_instr, exec_FX55) {
  GTEST_SKIP();
}

// Fill V0 to VX from memory starting at addr register
TEST(rem8Cpp_instr, exec_FX65) {
  GTEST_SKIP();
}

