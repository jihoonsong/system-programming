/**
 * @file  memspace.c
 * @brief A handler of memspace related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * @def   MEMORY_SIZE
 * @brief 1Mbyte, i.e. 2^20.
 * @see   memory
 */
#define MEMORY_SIZE 0xFFFFF

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief A memory on that object file will be loaded.
 */
static char _memory[MEMORY_SIZE] = {0,};

/**
 * @brief          Print memory in the given range.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_dump(char *cmd, int argc, char *argv[]);

void memspace_execute(char *cmd, int argc, char *argv[])
{
  if(!strcmp("du", cmd) || !strcmp("dump", cmd))
  {
    _is_command_executed = memspace_execute_dump(cmd, argc, argv);
  }
  else
  {
    printf("%s: command not found\n", cmd);
  }
}

static bool memspace_execute_dump(char *cmd, int argc, char *argv[])
{
  // TODO: to be implemented.
  return false;
}
