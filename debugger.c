/**
 * @file  debugger.c
 * @brief A handler of debugger related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugger.h"

#include "logger.h"
#include "memspace.h"

/**
 * @def   REGISTER_FILE_LEN
 * @brief The length of register file.
 */
#define REGISTER_FILE_LEN 10

/**
 * @brief Structure of breakpoint elements.
 */
struct breakpoint
{
  /** A pointer to the next breakpoint element. */
  struct breakpoint *next;
  /** An address value. */
  int               address;
};

/**
 * @brief Equals to 0x00000.
 */
static const int ADDRESS_MIN = 0x00000;

/**
 * @brief Equals to 0xFFFFF.
 */
static const int ADDRESS_MAX = 0xFFFFF;

/**
 * @brief Equals to 0xFFF. The displacement field is 12-bits long.
 */
static const int DISPLACEMENT_MASK = 0xFFF;

/**
 * @brief Equals to 0x7FF, equals to decimal 2047 in two's complement.
 */
static const int DISPLACEMENT_MAX = 0x7FF;

/**
 * @brief Equals to 16.
 */
static const int HEX = 16;

/**
 * @brief An assigned number of each register.
 */
static const int REGISTER_A  = 0; // Accumulator; used for arithmetic
                                  // operations.
static const int REGISTER_X  = 1; // Index register; used for addressing.
static const int REGISTER_L  = 2; // Linkage register; the Jump to Subroutine
                                  // (JSUB) instruction stores the return
                                  // address in this register.
static const int REGISTER_B  = 3; // Base register; used for addressing.
static const int REGISTER_S  = 4; // General working register - no special use.
static const int REGISTER_T  = 5; // General working register - no special use.
static const int REGISTER_F  = 6; // Floating-point accumulator. (48 bits)
static const int REGISTER_PC = 8; // Program counter; contains the address of
                                  // the next instruction to be fetched for
                                  // execution.
static const int REGISTER_SW = 9; // Status word; contains a variety of
                                  // information, including a Condition Code.
                                  // (CC)

/**
 * @brief A list of breakpoints. All breakpoints are stored in ascending order.
 */
static struct breakpoint *_breakpoint_list = NULL;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief A starting address of program that currently loaded on memory.
 */
static int _program_address = 0;

/**
 * @brief A length of program that currently loaded on memory.
 */
static int _program_length = 0;

/**
 * @brief A list of registers.
 */
static unsigned int _registers[REGISTER_FILE_LEN] = {0,};

/**
 * @brief Clear all stored breakpoints.
 */
static void debugger_clear_breakpoints(void);

/**
 * @brief          Set or unset breakpoint, or show all breakpoints.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool debugger_execute_bp(const char *cmd,
                                const int  argc,
                                const char *argv[]);
/**
 * @brief          Run loaded program and show value of each registers.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool debugger_execute_run(const char *cmd,
                                 const int  argc,
                                 const char *argv[]);

/**
 * @brief            Return a format of the given opcode.
 * @param[in] opcode An opcode to be examined.
 * @return           A format of the given opcode.
 */
static int debugger_get_format(const unsigned int opcode);

/**
 * @brief            Execute instruction format 1.
 * @param[in] opcode An opcode of instruction to be executed.
 */
static void debugger_instruction_format1(const unsigned int opcode);

/**
 * @brief            Execute instruction format 2.
 * @param[in] opcode An opcode of instruction to be executed.
 * @param[in] r1     A register 1.
 * @param[in] r2     A register 2.
 */
static void debugger_instruction_format2(const unsigned int opcode,
                                         const int          r1,
                                         const int          r2);

/**
 * @brief                    Execute instruction format 3.
 * @param[in] opcode         An opcode of instruction to be executed.
 * @param[in] n              A flag n.
 * @param[in] i              A flag i.
 * @param[in] target_address A target address.
 * @note                     The value of target address get changed
 *                           during indirect addressing.
 */
static void debugger_instruction_format3_4(const unsigned int opcode,
                                           const unsigned int n,
                                           const unsigned int i,
                                           int                target_address);
/**
 * @brief             Check if PC reached any breakpoint.
 * @param[in] address An address that PC has.
 * @return    True if PC reached any breakpoint, false otherwise.
 */
static bool debugger_is_reached_breakpoint(const int address);

/**
 * @brief Set breakpoint.
 */
static void debugger_set_breakpoint(const int address);

/**
 * @brief Show all stored breakpoints.
 */
static void debugger_show_breakpoints(void);

/**
 * @brief Show values of registers.
 */
static void debugger_show_registers(void);

void debugger_execute(const char *cmd,
                      const int  argc,
                      const char *argv[])
{
  if(!strcmp("bp", cmd))
  {
    _is_command_executed = debugger_execute_bp(cmd, argc, argv);
  }
  else if(!strcmp("run", cmd))
  {
    _is_command_executed = debugger_execute_run(cmd, argc, argv);
  }
  else
  {
    printf("%s: command not found\n", cmd);
  }

  if(_is_command_executed)
  {
    logger_write_log(cmd, argc, argv);
  }
}

void debugger_initialize(void)
{
  debugger_terminate();

  memset(_registers, 0, sizeof(_registers));
  _program_length = 0;
}

void debugger_prepare_run(const int program_address, const int program_length)
{
  _registers[REGISTER_L]  = program_length;
  _registers[REGISTER_PC] = program_address;
  _program_address        = program_address;
  _program_length         = program_length;
}

void debugger_terminate(void)
{
  debugger_clear_breakpoints();
}

static bool debugger_execute_bp(const char *cmd,
                                const int  argc,
                                const char *argv[])
{
  if(1 < argc)
  {
    printf("debugger: too many arguments\n");
    return false;
  }

  if(0 == argc)
  {
    debugger_show_breakpoints();
  }
  else
  {
    if(!strcmp("clear", argv[0]))
    {
      debugger_clear_breakpoints();
    }
    else
    {
      char *endptr = NULL;
      int  address = strtol(argv[0], &endptr, HEX);
      if('\0' != *endptr)
      {
        printf("debugger: argument '%s' is invalid\n", argv[0]);
        return false;
      }
      if(ADDRESS_MIN > address ||
         ADDRESS_MAX < address)
      {
        printf("debugger: address '%X' is out of range\n", address);
        return false;
      }

      debugger_set_breakpoint(address);
    }
  }

  return true;
}

static void debugger_clear_breakpoints(void)
{
  if(!_breakpoint_list)
  {
    return;
  }

  struct breakpoint *walk = _breakpoint_list;
  while(walk)
  {
    struct breakpoint *del = walk;
    walk = walk->next;
    free(del);
  }

  _breakpoint_list = NULL;
}

static bool debugger_execute_run(const char *cmd,
                                 const int  argc,
                                 const char *argv[])
{
  if(0 < argc)
  {
    printf("debugger: too many arguments\n");
    return false;
  }

  if(0 == _program_length)
  {
    printf("debugger: no program is loaded\n");
    return false;
  }

  bool is_break = false;
  while(!is_break)
  {
    unsigned char instruction[4] = {0,};
    memspace_get_memory(instruction, _registers[REGISTER_PC], 3);

    unsigned int opcode = instruction[0] & 0xFC;
    int          format = debugger_get_format(opcode);
    if(1 == format)
    {
      // Format 1.
      _registers[REGISTER_PC] += 1;

      debugger_instruction_format1(opcode);
    }
    else if(2 == format)
    {
      // Format 2.
      _registers[REGISTER_PC] += 2;

      int r1 = (instruction[1] >> 4) & 0xF;
      int r2 = instruction[1] & 0xF;
      debugger_instruction_format2(opcode, r1, r2);
    }
    else if(3 == format)
    {
      unsigned int n = (instruction[0] & 0x02) ? 1 : 0;
      unsigned int i = (instruction[0] & 0x01) ? 1 : 0;
      unsigned int x = (instruction[1] & 0x80) ? 1 : 0;
      unsigned int b = (instruction[1] & 0x40) ? 1 : 0;
      unsigned int p = (instruction[1] & 0x20) ? 1 : 0;
      unsigned int e = (instruction[1] & 0x10) ? 1 : 0;

      if(!e)
      {
        // Format 3.
        _registers[REGISTER_PC] += 3;

        int displacement = instruction[1] & 0x0F;
        displacement     = displacement << 8;
        displacement     += instruction[2];

        int target_address = 0;
        if(0 == n && 0 == i)
        {
          // A backward compatiblity to SIC machine.
          target_address = (b << 14) + (p << 13) + (e << 12) + displacement;
        }
        else
        {
          if(1 == b && 0 == p)
          {
            // Base relative addressing.
            target_address = _registers[REGISTER_B] + displacement;
          }
          else if(0 == b && 1 == p)
          {
            // PC relative addressing.
            if(DISPLACEMENT_MAX < displacement)
            {
              // A displacment is a negative value, so perform sign extension.
              displacement = -(-displacement & DISPLACEMENT_MASK);
            }
            target_address = _registers[REGISTER_PC] + displacement;
          }
          else if(0 == b && 0 == p)
          {
            target_address = displacement;
          }
          else
          {
            printf("debugger: invalid addressing\n");
            return false;
          }
        }
        if(1 == x)
        {
          // Indexed addressing.
          target_address += _registers[REGISTER_X];
        }

        debugger_instruction_format3_4(opcode, n, i, target_address);
      }
      else
      {
        // Format 4.
        // In real design, memory is fetched by the size of register.
        // (in SIC/XE, it's 3 bytes.) However, in this implementation,
        // we just fetch one byte for conveinence.
        memspace_get_memory(&instruction[3], _registers[REGISTER_PC] + 3, 1);
        _registers[REGISTER_PC] += 4;

        int address = instruction[1] & 0x0F;
        address     = address << 8;
        address     += instruction[2];
        address     = address << 8;
        address     += instruction[3];

        int target_address = address;
        if(1 == x)
        {
          // Indexed addressing.
          target_address += _registers[REGISTER_X];
        }

        debugger_instruction_format3_4(opcode, n, i, target_address);
      }
    }
    else
    {
      // Invalid opcode.
      printf("debugger: invalid opcode\n");
      return false;
    }

    if(_program_address + _program_length <= _registers[REGISTER_PC])
    {
      debugger_show_registers();
      printf("Program finished\n");

      debugger_initialize();
      is_break = true;
    }
    else if(debugger_is_reached_breakpoint(_registers[REGISTER_PC]))
    {
      debugger_show_registers();
      printf("Breakpoint at %X\n", _registers[REGISTER_PC]);

      is_break = true;
    }
    else
    {
      // Continue program execution.
    }
  }

  return true;
}

static int debugger_get_format(const unsigned int opcode)
{
  if(0xC4 == opcode ||
     0xC0 == opcode ||
     0xF4 == opcode ||
     0xC8 == opcode ||
     0xF0 == opcode ||
     0xF8 == opcode)
  {
    // Format 1.
    return 1;
  }
  else if(0x90 == opcode ||
          0xB4 == opcode ||
          0xA0 == opcode ||
          0x9C == opcode ||
          0x98 == opcode ||
          0xAC == opcode ||
          0xA4 == opcode ||
          0xA8 == opcode ||
          0x94 == opcode ||
          0xB0 == opcode ||
          0xB8 == opcode)
  {
    // Format 2.
    return 2;
  }
  else if(0x18 == opcode ||
          0x58 == opcode ||
          0x40 == opcode ||
          0x28 == opcode ||
          0x88 == opcode ||
          0x24 == opcode ||
          0x64 == opcode ||
          0x3C == opcode ||
          0x30 == opcode ||
          0x34 == opcode ||
          0x38 == opcode ||
          0x48 == opcode ||
          0x00 == opcode ||
          0x68 == opcode ||
          0x50 == opcode ||
          0x70 == opcode ||
          0x08 == opcode ||
          0x6C == opcode ||
          0x74 == opcode ||
          0x04 == opcode ||
          0xD0 == opcode ||
          0x20 == opcode ||
          0x60 == opcode ||
          0x44 == opcode ||
          0xD8 == opcode ||
          0x4C == opcode ||
          0xEC == opcode ||
          0x0C == opcode ||
          0x78 == opcode ||
          0x54 == opcode ||
          0x80 == opcode ||
          0xD4 == opcode ||
          0x14 == opcode ||
          0x7C == opcode ||
          0xE8 == opcode ||
          0x84 == opcode ||
          0x10 == opcode ||
          0x1C == opcode ||
          0x5C == opcode ||
          0xE0 == opcode ||
          0x2C == opcode ||
          0xDC == opcode)
  {
    // Format 3/4.
    return 3;
  }
  else
  {
    printf("debugger: cannot find opcode '%02X'\n", opcode);
    return 0;
  }
}

static void debugger_instruction_format1(const unsigned int opcode)
{
  if(0xC4 == opcode)
  {
    // FIX: A <- (F) [convert to integer].
    // This implementation ignores this opcode.
  }
  else if(0xC0 == opcode)
  {
    // FLOAT: F <- (A) [convert to floating].
    // This implementation ignores this opcode.
  }
  else if(0xF4 == opcode)
  {
    // HIO: Halt I/O channel number (A).
    // This implementation ignores this opcode.
  }
  else if(0xC8 == opcode)
  {
    // NORM: F <- (F) [normalized].
    // This implementation ignores this opcode.
  }
  else if(0xF0 == opcode)
  {
    // SIO: Start I/O channel number (A); address of
    //      channel program is given by (S).
    // This implementation ignores this opcode.
  }
  else if(0xF8 == opcode)
  {
    // TIO: Test I/O channel number (A).
    // This implementation ignores this opcode.
  }
  else
  {
    printf("debugger: cannot find opcode '%02X'\n", opcode);
  }
}

static void debugger_instruction_format2(const unsigned int opcode,
                                         const int          r1,
                                         const int          r2)
{
  if(0x90 == opcode)
  {
    // ADDR: r2 <- (r2) + (r1).
    _registers[r1] = _registers[r1] + _registers[r2];
  }
  else if(0xB4 == opcode)
  {
    // CLEAR: r1 <- 0.
    _registers[r1] = 0;
  }
  else if(0xA0 == opcode)
  {
    // COMPR: (r1) : (r2).
    int diff = _registers[r1] - _registers[r2];
    if(diff > 0)
    {
      _registers[REGISTER_SW] = '>';
    }
    else if(diff < 0)
    {
      _registers[REGISTER_SW] = '<';
    }
    else
    {
      _registers[REGISTER_SW] = '=';
    }
  }
  else if(0x9C == opcode)
  {
    // DIVR: r2 <- (r2) / (r1).
    _registers[r2] = _registers[r2] / _registers[r1];
  }
  else if(0x98 == opcode)
  {
    // MULR: r2 <- (r2) * (r1).
    _registers[r2] = _registers[r2] * _registers[r1];
  }
  else if(0xAC == opcode)
  {
    // RMO: r2 <- (r1).
    _registers[r2] = _registers[r1];
  }
  else if(0xA4 == opcode)
  {
    // SHIFTL: r1 <- (r1); left circular shift n bits.
    //         {in assembled instruction, r2 = n-1}.
    // This implementation ignores this opcode.
  }
  else if(0xA8 == opcode)
  {
    // SHIFTR: r1 <- (r1); right shift n bits, with vacated bit positions set
    //         equal to leftmost bit of (r1).
    //         {In assembled instruction, r2 = n-1}.
    // This implementation ignores this opcode.
  }
  else if(0x94 == opcode)
  {
    // SUBR: r2 <- (r2) - (r1).
    _registers[r2] = _registers[r2] - _registers[r1];
  }
  else if(0xB0 == opcode)
  {
    // SVC: Generate SVC interrupt. {In assembled instruction, r1 = n}.
    // This implementation ignores this opcode.
  }
  else if(0xB8 == opcode)
  {
    // TIXR: X <- (X) + 1; (X): (r1).
    _registers[REGISTER_X] = _registers[REGISTER_X] + 1;

    int diff = _registers[REGISTER_X] - _registers[r1];
    if(diff > 0)
    {
      _registers[REGISTER_SW] = '>';
    }
    else if(diff < 0)
    {
      _registers[REGISTER_SW] = '<';
    }
    else
    {
      _registers[REGISTER_SW] = '=';
    }
  }
  else
  {
    printf("debugger: cannot find opcode '%02X'\n", opcode);
  }
}

static void debugger_instruction_format3_4(const unsigned int opcode,
                                           const unsigned int n,
                                           const unsigned int i,
                                           int                target_address)
{
  unsigned int value = 0;
  if(1 == n && 0 == i)
  {
    // Indirect addressing.
    unsigned char memory[3] = {0,};
    memspace_get_memory(memory, target_address, 3);

    target_address = (memory[0] << 16) + (memory[1] << 8) + memory[2];
    memspace_get_memory(memory, target_address, 3);

    value = (memory[0] << 16) + (memory[1] << 8) + memory[2];
  }
  else if(0 == n && 1 == i)
  {
    // Immediate addressing.
    value = target_address;
  }
  else if(1 == n && 1 == i)
  {
    // Simple addressing.
    unsigned char memory[3] = {0,};
    memspace_get_memory(memory, target_address, 3);

    value = (memory[0] << 16) + (memory[1] << 8) + memory[2];
  }
  else
  {
    // A backward compatiblity to SIC machine.
    value = target_address;
  }

  if(0x18 == opcode)
  {
    // ADD: A <- (A) + (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] + (value & 0xFFFFFF);
  }
  else if(0x58 == opcode)
  {
    // ADDF: F <= (F) + (m..m+5).
    // This implementation ignores this opcode.
  }
  else if(0x40 == opcode)
  {
    // AND: A <- (A) & (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] & (value & 0xFFFFFF);
  }
  else if(0x28 == opcode)
  {
    // COMP: (A) : (m..m+2).
    int diff = _registers[REGISTER_A] - (value & 0xFFFFFF);
    if(diff > 0)
    {
      _registers[REGISTER_SW] = '>';
    }
    else if(diff < 0)
    {
      _registers[REGISTER_SW] = '<';
    }
    else
    {
      _registers[REGISTER_SW] = '=';
    }
  }
  else if(0x88 == opcode)
  {
    // COMPF: (F) : (m..m+5).
    // This implementation ignores this opcode.
  }
  else if(0x24 == opcode)
  {
    // DIV: A <- (A) / (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] / (value & 0xFFFFFF);
  }
  else if(0x64 == opcode)
  {
    // DIVF: F <- (F) / (m..m+5).
    // This implementation ignores this opcode.
  }
  else if(0x3C == opcode)
  {
    // J: PC <- m.
    _registers[REGISTER_PC] = target_address;
  }
  else if(0x30 == opcode)
  {
    // JEQ: PC <- m if CC set to =.
    if('=' == _registers[REGISTER_SW])
    {
      _registers[REGISTER_PC] = target_address;
    }
  }
  else if(0x34 == opcode)
  {
    // JGT: PC <- m if CC set to >.
    if('>' == _registers[REGISTER_SW])
    {
      _registers[REGISTER_PC] = target_address;
    }
  }
  else if(0x38 == opcode)
  {
    // JLT: PC <- m if CC set to <.
    if('<' == _registers[REGISTER_SW])
    {
      _registers[REGISTER_PC] = target_address;
    }
  }
  else if(0x48 == opcode)
  {
    // JSUB: L <- (PC); PC <- m.
    _registers[REGISTER_L]  = _registers[REGISTER_PC];
    _registers[REGISTER_PC] = target_address;
  }
  else if(0x00 == opcode)
  {
    // LDA: A <- (m..m+2).
    _registers[REGISTER_A] = (value & 0xFFFFFF);
  }
  else if(0x68 == opcode)
  {
    // LDB: B <- (m..m+2).
    _registers[REGISTER_B] = (value & 0xFFFFFF);
  }
  else if(0x50 == opcode)
  {
    // LDCH: A[rightmost byte] <- (m).
    _registers[REGISTER_A] = ((value >> 16) & 0xFF);
  }
  else if(0x70 == opcode)
  {
    // LDF: F <- (m..m+5).
    // This implementation ignores this opcode.
  }
  else if(0x08 == opcode)
  {
    // LDL: L <- (m..m+2).
    _registers[REGISTER_L] = (value & 0xFFFFFF);
  }
  else if(0x6C == opcode)
  {
    // LDS: S <- (m..m+2).
    _registers[REGISTER_S] = (value & 0xFFFFFF);
  }
  else if(0x74 == opcode)
  {
    // LDT: T <- (m..m+2).
    _registers[REGISTER_T] = (value & 0xFFFFFF);
  }
  else if(0x04 == opcode)
  {
    // LDX: X <- (m..m+2).
    _registers[REGISTER_X] = (value & 0xFFFFFF);
  }
  else if(0xD0 == opcode)
  {
    // LPS: Load processor status from information beginning at address m.
    // This implementation ignores this opcode.
  }
  else if(0x20 == opcode)
  {
    // MUL: A <- (A) * (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] * (value & 0xFFFFFF);
  }
  else if(0x60 == opcode)
  {
    // MULF: F <- (F) * (m..m+5).
    // This implementation ignores this opcode.
  }
  else if(0x44 == opcode)
  {
    // OR: A <- (A) | (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] | (value & 0xFFFFFF);
  }
  else if(0xD8 == opcode)
  {
    // RD: A[rightmost byte] <- data from device specified by (m).
    // This implementation assumes that RD receives 0.
    _registers[REGISTER_A] = 0;
  }
  else if(0x4C == opcode)
  {
    // RSUB: PC <- (L).
    _registers[REGISTER_PC] = _registers[REGISTER_L];
  }
  else if(0xEC == opcode)
  {
    // SSK: Protection key for address m <- (A).
    // This implementation ignores this opcode.
  }
  else if(0x0C == opcode)
  {
    // STA: m..m+2 <- (A).
    unsigned char memory[3] = {(_registers[REGISTER_A] >> 16) & 0xFF,
                               (_registers[REGISTER_A] >> 8) & 0xFF,
                               _registers[REGISTER_A] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x78 == opcode)
  {
    // STB: m..m+2 <- (B).
    unsigned char memory[3] = {(_registers[REGISTER_B] >> 16) & 0xFF,
                               (_registers[REGISTER_B] >> 8) & 0xFF,
                               _registers[REGISTER_B] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x54 == opcode)
  {
    // STCH: m <- (A)[rightmost byte].
    unsigned char memory[1] = {_registers[REGISTER_A] & 0xFF};
    memspace_set_memory(target_address, memory, 1);
  }
  else if(0x80 == opcode)
  {
    // STF: m..m+5 <- (F).
    // This implementation ignores this opcode.
  }
  else if(0xD4 == opcode)
  {
    // STI: Interval timer value <- (m..m+2).
    // This implementation ignores this opcode.
  }
  else if(0x14 == opcode)
  {
    // STL: m..m+2 <- (L).
    unsigned char memory[3] = {(_registers[REGISTER_L] >> 16) & 0xFF,
                               (_registers[REGISTER_L] >> 8) & 0xFF,
                               _registers[REGISTER_L] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x7C == opcode)
  {
    // STS: m..m+2 <- (S).
    unsigned char memory[3] = {(_registers[REGISTER_S] >> 16) & 0xFF,
                               (_registers[REGISTER_S] >> 8) & 0xFF,
                               _registers[REGISTER_S] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0xE8 == opcode)
  {
    // STSW: m..m+2 <- (SW).
    unsigned char memory[3] = {(_registers[REGISTER_SW] >> 16) & 0xFF,
                               (_registers[REGISTER_SW] >> 8) & 0xFF,
                               _registers[REGISTER_SW] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x84 == opcode)
  {
    // STT: m..m+2 <- (T).
    unsigned char memory[3] = {(_registers[REGISTER_T] >> 16) & 0xFF,
                               (_registers[REGISTER_T] >> 8) & 0xFF,
                               _registers[REGISTER_T] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x10 == opcode)
  {
    // STX: m..m+2 <- (X).
    unsigned char memory[3] = {(_registers[REGISTER_X] >> 16) & 0xFF,
                               (_registers[REGISTER_X] >> 8) & 0xFF,
                               _registers[REGISTER_X] & 0xFF};
    memspace_set_memory(target_address, memory, 3);
  }
  else if(0x1C == opcode)
  {
    // SUB: A <- (A) - (m..m+2).
    _registers[REGISTER_A] = _registers[REGISTER_A] - (value & 0xFFFFFF);
  }
  else if(0x5C == opcode)
  {
    // SUBF: F <- (F) - (m..m+2).
    // This implementation ignores this opcode.
  }
  else if(0xE0 == opcode)
  {
    // TD: Test device specified by (m).
    // This implementation assumes that device is always ready.
    _registers[REGISTER_SW] = '<';
  }
  else if(0x2C == opcode)
  {
    // TIX: X <- (X) + 1; (X): (m..m+2).
    _registers[REGISTER_X] = _registers[REGISTER_X] + 1;

    int diff = _registers[REGISTER_X] - value;
    if(diff > 0)
    {
      _registers[REGISTER_SW] = '>';
    }
    else if(diff < 0)
    {
      _registers[REGISTER_SW] = '<';
    }
    else
    {
      _registers[REGISTER_SW] = '=';
    }
  }
  else if(0xDC == opcode)
  {
    // WD: Device specified by (m) <- (A)[rightmost byte].
    // This implementation ignores this opcode.
  }
  else
  {
    printf("debugger: cannot find opcode '%02X'\n", opcode);
  }
}

static bool debugger_is_reached_breakpoint(const int address)
{
  struct breakpoint *walk = _breakpoint_list;
  while(walk)
  {
    if(address == walk->address)
    {
      return true;
    }

    walk = walk->next;
  }

  return false;
}

static void debugger_set_breakpoint(const int address)
{
  struct breakpoint *new_bp = malloc(sizeof(*new_bp));
  new_bp->next    = NULL;
  new_bp->address = address;

  if(!_breakpoint_list)
  {
    _breakpoint_list = new_bp;
  }
  else
  {
    if(new_bp->address < _breakpoint_list->address)
    {
      new_bp->next = _breakpoint_list;
      _breakpoint_list = new_bp;
      return;
    }

    struct breakpoint *prev = NULL;
    struct breakpoint *walk = _breakpoint_list;
    while(walk->next)
    {
      prev = walk;
      walk = walk->next;

      if(new_bp->address < walk->address)
      {
        prev->next   = new_bp;
        new_bp->next = walk;
        return;
      }
    }
    walk->next = new_bp;
  }
}

static void debugger_show_breakpoints(void)
{
  printf("Breakpoints\n");
  printf("-----------\n");

  struct breakpoint *walk = _breakpoint_list;
  while(walk)
  {
    printf("%X\n", walk->address);
    walk = walk->next;
  }
}

static void debugger_show_registers(void)
{
  printf("A: %06X   X: %06X\n", _registers[REGISTER_A], _registers[REGISTER_X]);
  printf("L: %06X  PC: %06X\n", _registers[REGISTER_L], _registers[REGISTER_PC]);
  printf("B: %06X   S: %06X\n", _registers[REGISTER_B], _registers[REGISTER_S]);
  printf("T: %06X\n", _registers[REGISTER_T]);
}
