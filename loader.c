/**
 * @file  loader.h
 * @brief A handler of loader related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "loader.h"

#include "logger.h"

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief          Perform linking and loading.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool loader_execute_loader(const char *cmd,
                                  const int  argc,
                                  const char *argv[]);

void loader_execute(const char *cmd,
                    const int  argc,
                    const char *argv[])
{
  if(!strcmp("loader", cmd))
  {
    _is_command_executed = loader_execute_loader(cmd, argc, argv);
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

static bool loader_execute_loader(const char *cmd,
                                  const int  argc,
                                  const char *argv[])
{
  if(0 == argc)
  {
    printf("loader: at least one object file is required\n");
    return false;
  }
  if(3 < argc)
  {
    printf("loader: at most three object files can be loaded\n");
    return false;
  }

  printf("loader_execute_loader is called\n");

  return true;
}
