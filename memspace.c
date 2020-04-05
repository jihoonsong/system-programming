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
 * @brief Its address is represented in 20bits. i.e. 1Mbyte.
 * @see   memory
 */
#define MEMORY_SIZE 0xFFFFF + 1

/**
 * @brief Equals to 0x00000.
 */
const int ADDRESS_MIN = 0x00000;

/**
 * @brief Equals to 0xFFFFF.
 */
const int ADDRESS_MAX = 0xFFFFF;

/**
 * @brief Equals to 10. i.e. dump 10 lines.
 */
const int DUMP_LINE_COUNT = 10;

/**
 * @brief Equals to 16. i.e. dump 16 bytes per line.
 */
const int DUMP_LINE_LEN = 16;

/**
 * @brief Equals to 160. i.e. DUMP_LINE_COUNT * DUMP_LINE_LEN.
 * @see   DUMP_LINE_COUNT
 * @see   DUMP_LINE_LEN
 */
const int DUMP_SIZE = 160;

/**
 * @brief Equals to 16.
 */
const int HEX = 16;

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
 * @note  The index range is [ADDRESS_MIN, ADDRESS_MAX].
 */
static char _memory[MEMORY_SIZE] = {0,};

/**
 * @brief          Print memory in the given range.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_dump(char *cmd, int argc, char *argv[]);

/**
 * @brief          Set memory the given value.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_edit(char *cmd, int argc, char *argv[]);

void memspace_execute(char *cmd, int argc, char *argv[])
{
  if(!strcmp("du", cmd) || !strcmp("dump", cmd))
  {
    _is_command_executed = memspace_execute_dump(cmd, argc, argv);
  }
  else if(!strcmp("e", cmd) || !strcmp("edit", cmd))
  {
    _is_command_executed = memspace_execute_edit(cmd, argc, argv);
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
    if(dump_start > ADDRESS_MAX)
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
    if(dump_start > ADDRESS_MAX)
    {
      printf("dump: start value '%d' is too large\n", dump_start);
      return false;
    }
  }

  if(2 > argc)
  {
    dump_end = dump_start + DUMP_SIZE - 1; // Closed interval.
    if(dump_end > ADDRESS_MAX)
    {
      dump_end = ADDRESS_MAX;
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
    if(dump_end > ADDRESS_MAX)
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

  for(int line = dump_start / DUMP_LINE_LEN;
      line <= dump_end / DUMP_LINE_LEN;
      ++line)
  {
    int base = line * DUMP_LINE_LEN;

    printf("%05X ", base);
    for(int offset = 0; offset < DUMP_LINE_LEN; ++offset)
    {
      int address = base + offset;
      if(address < dump_start || address > dump_end)
      {
        printf("%2c ", ' ');
      }
      else
      {
        printf("%02X ", _memory[address]);
      }
    }
    printf("; ");
    for(int offset = 0; offset < DUMP_LINE_LEN; ++offset)
    {
      int address = base + offset;
      if(address < dump_start || address > dump_end ||
         _memory[address] < 0x20 || _memory[address] > 0x7E)
      {
        printf(".");
      }
      else
      {
        printf("%c", _memory[address]);
      }
    }
    printf("\n");
  }

  _last_dumped = dump_end;

  return true;
}

static bool memspace_execute_edit(char *cmd, int argc, char *argv[])
{
}
