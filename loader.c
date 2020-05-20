/**
 * @file  loader.h
 * @brief A handler of loader related commands.
 */

#include "loader.h"

#include "logger.h"

void loader_execute(const char *cmd,
                    const int  argc,
                    const char *argv[])
{
  if(_is_command_executed)
  {
    logger_write_log(cmd, argc, argv);
  }
}
