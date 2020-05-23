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
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

void debugger_execute(const char *cmd,
                      const int  argc,
                      const char *argv[])
{
  printf("debugger invoked\n");

  if(_is_command_executed)
  {
    logger_write_log(cmd, argc, argv);
  }
}
