/**
 * @file  memspace.c
 * @brief A handler of memspace related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

/**
 * @def   MEMORY_SIZE
 * @brief 1Mbyte, i.e. 2^20.
 * @see   memory
 */
#define MEMORY_SIZE 0xFFFFF

/**
 * @brief Equals to 16.
 */
const int HEX = 16;

/**
 * @brief Equals to 160. It is 10 lines with 16 bytes per line.
 */
const int DEFAULT_DUMP = 160;

/**
 * @brief A flag indicating the last dumped address.
 */
static int _last_dumped = -1;

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

  if(_is_command_executed)
  {
    logger_write_log(cmd, argc, argv);
  }
}

static bool memspace_execute_dump(char *cmd, int argc, char *argv[])
{
  if(2 < argc)
  {
    printf("dump: too many arguments\n");
    return false;
  }

  int  dump_start = 0;
  int  dump_end = 0;
  char *endptr = NULL;

  if(0 == argc)
  {
    dump_start = _last_dumped + 1;
    if(dump_start > MEMORY_SIZE)
    {
      dump_start = 0;
    }
  }
  else
  {
    dump_start = strtol(argv[0], &endptr, HEX);
    if('\0' != *endptr)
    {
      printf("dump: argument '%s' is invalid\n", argv[0]);
      return false;
    }
    if(dump_start > MEMORY_SIZE)
    {
      printf("dump: start value '%d' is too large\n", dump_start);
      return false;
    }
  }

  if(2 > argc)
  {
    dump_end = dump_start + DEFAULT_DUMP - 1; // Closed interval.
    if(dump_end > MEMORY_SIZE)
    {
      dump_end = MEMORY_SIZE;
    }
  }
  else
  {
    dump_end = strtol(argv[1], &endptr, HEX);
    if('\0' != *endptr)
    {
      printf("dump: argument '%s' is invalid\n", argv[1]);
      return false;
    }
    if(dump_end > MEMORY_SIZE)
    {
      printf("dump: end value '%d' is too large\n", dump_end);
      return false;
    }

    if(dump_start > dump_end)
    {
      printf("dump: start value '%d' is larger than end value '%d'\n",
          dump_start, dump_end);
      return false;
    }
  }

  printf("start: %d, end: %d\n", dump_start, dump_end);

  return true;
}
