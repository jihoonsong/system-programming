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
 * @brief A list of breakpoints. All breakpoints are stored in ascending order.
 */
static struct breakpoint *_breakpoint_list = NULL;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief Clear all stored breakpoints.
 */
static void debugger_clear_breakpoints(void);

/**
 * @brief Show all stored breakpoints.
 */
static void debugger_show_breakpoints(void);

/**
 * @brief          Set or unset breakpoint, or show all breakpoints.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool debugger_execute_bp(const char *cmd,
                                const int  argc,
                                const char *argv[]);

void debugger_execute(const char *cmd,
                      const int  argc,
                      const char *argv[])
{
  if(!strcmp("bp", cmd))
  {
    _is_command_executed = debugger_execute_bp(cmd, argc, argv);
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

  printf("bp is called\n");
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
