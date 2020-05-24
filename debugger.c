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
 * @brief Structure of registers.
 */
struct registers
{
  unsigned int A;  // Accumulator; used for arithmetic operations.
  unsigned int X;  // Index register; used for addressing.
  unsigned int L;  // Linkage register; the Jump to Subroutine (JSUB)
                   // instruction stores the return address in this register.
  unsigned int PC; // Program counter; contains the address of the next
                   // instruction to be fetched for execution.
  unsigned int B;  // Base register; used for addressing.
  unsigned int S;  // General working register - no special use.
  unsigned int T;  // General working register - no special use.
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
 * @brief Equals to 16.
 */
static const int HEX = 16;

/**
 * @brief A list of breakpoints. All breakpoints are stored in ascending order.
 */
static struct breakpoint *_breakpoint_list = NULL;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief A length of program that currently loaded on memory.
 */
static int _program_length = 0;

/**
 * @brief Registers used for program execution.
 */
static struct registers _registers = {0,};

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

  memset(&_registers, 0, sizeof(_registers));
  _program_length = 0;
}

void debugger_prepare_run(const int program_address, const int program_length)
{
  _registers.L    = program_length;
  _registers.PC   = program_address;
  _program_length = program_length;
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
    memspace_get_memory(instruction, _registers.PC, 3);

    unsigned int opcode = instruction[0] & 0xFC;
    int          format = debugger_get_format(opcode);
    if(1 == format)
    {
      // Format 1.
      _registers.PC += 1;

      // TODO
    }
    else if(2 == format)
    {
      // Format 2.
      _registers.PC += 2;

      // TODO
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
        _registers.PC += 3;

        // TODO
      }
      else
      {
        // Format 4.
        // In real design, memory is fetched by the size of register. (in SIC/XE, it's 3 bytes.)
        // However, in this implementation, we just fetch one byte for conveinence.
        memspace_get_memory(&instruction[3], _registers.PC + 3, 1);
        _registers.PC += 4;

        // TODO
      }

    }
    else
    {
      // Invalid opcode.
      return false;
    }

    if(_program_length <= _registers.PC)
    {
      debugger_show_registers();
      printf("Program finished\n");

      debugger_initialize();
      is_break = true;
    }
    else if(debugger_is_reached_breakpoint(_registers.PC))
    {
      debugger_show_registers();
      printf("Breakpoint at %X\n", _registers.PC);

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
  printf("A: %06X   X: %06X\n", _registers.A, _registers.X);
  printf("L: %06X  PC: %06X\n", _registers.L, _registers.PC);
  printf("B: %06X   S: %06X\n", _registers.B, _registers.S);
  printf("T: %06X\n", _registers.T);
}
