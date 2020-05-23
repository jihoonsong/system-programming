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
 * @brief Set breakpoint.
 */
static void debugger_set_breakpoint(const int address);

/**
 * @brief Show all stored breakpoints.
 */
static void debugger_show_breakpoints(void);

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

  printf("run is called\n");
  return true;
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
