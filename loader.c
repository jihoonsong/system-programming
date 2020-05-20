/**
 * @file  loader.h
 * @brief A handler of loader related commands.
 */

#include <stdbool.h>
#include <stdio.h>

#include "loader.h"

#include "logger.h"

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

void loader_execute(const char *cmd,
                    const int  argc,
                    const char *argv[])
{
  printf("loader received %s\n", cmd);

  if(_is_command_executed)
  {
    logger_write_log(cmd, argc, argv);
  }
}
