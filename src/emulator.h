/*  @file   emulator.h
 *  @brief  rem8C++ emulator interface.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <cstdint>
#include <array>

#define REM8CPP_SCREEN_WIDTH  0x40
#define REM8CPP_SCREEN_HEIGHT 0x20

#define REM8CPP_MAX_ADDR      0x0FFF
#define REM8CPP_START_ADDR    0x0200


class rem8Cpp {
  public:
    void cycle();
    

  private:
    rem8Cpp();

    std::array<bool, REM8CPP_SCREEN_WIDTH * REM8CPP_SCREEN_HEIGHT> screen;
    std::array<std::uint8_t, REM8CPP_MAX_ADDR> memory;

};

