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
 * @brief Its address is represented in 20 bits. i.e. 1 Mbyte.
 * @see   memory
 */
#define MEMORY_SIZE 0xFFFFF + 1

/**
 * @brief Equals to 0x00000.
 */
static const int ADDRESS_MIN = 0x00000;

/**
 * @brief Equals to 0xFFFFF.
 */
static const int ADDRESS_MAX = 0xFFFFF;

/**
 * @brief Equals to 10. i.e. dump 10 lines.
 */
static const int DUMP_LINE_COUNT = 10;

/**
 * @brief Equals to 16. i.e. dump 16 bytes per line.
 */
static const int DUMP_LINE_LEN = 16;

/**
 * @brief Equals to 160. i.e. DUMP_LINE_COUNT * DUMP_LINE_LEN.
 * @see   DUMP_LINE_COUNT
 * @see   DUMP_LINE_LEN
 */
static const int DUMP_SIZE = 160;

/**
 * @brief Equals to 16.
 */
static const int HEX = 16;

/**
 * @brief Equals to 0x00.
 * @note  Assumption is that 1 byte is 8 bits.
 */
static const int VALUE_MIN = 0x00;

/**
 * @brief Equals to 0xFF.
 * @note  Assumption is that 1 byte is 8 bits.
 */
static const int VALUE_MAX = 0xFF;

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
static unsigned char _memory[MEMORY_SIZE] = {0,};

/**
 * @brief An starting address in memory where a program is to be loaded.
 */
static int _progaddr = 0;

/**
 * @brief          Print memory in the given range.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_dump(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Set memory the given value.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_edit(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Fill memory the given value.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_fill(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Set progaddr the given value.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_progaddr(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Clear all memory.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool memspace_execute_reset(const char *cmd, const int argc, const char *argv[]);

void memspace_execute(const char *cmd, const int argc, const char *argv[])
{
  if(!strcmp("du", cmd) || !strcmp("dump", cmd))
  {
    _is_command_executed = memspace_execute_dump(cmd, argc, argv);
  }
  else if(!strcmp("e", cmd) || !strcmp("edit", cmd))
  {
    _is_command_executed = memspace_execute_edit(cmd, argc, argv);
  }
  else if(!strcmp("f", cmd) || !strcmp("fill", cmd))
  {
    _is_command_executed = memspace_execute_fill(cmd, argc, argv);
  }
  else if(!strcmp("reset", cmd))
  {
    _is_command_executed = memspace_execute_reset(cmd, argc, argv);
  }
  else if(!strcmp("progaddr", cmd))
  {
    _is_command_executed = memspace_execute_progaddr(cmd, argc, argv);
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

int memspace_get_progaddr(void)
{
  return _progaddr;
}

unsigned char *memspace_get_memory(unsigned char *memory,
                                   const int address,
                                   const int byte_count)
{
  if(ADDRESS_MIN > address ||
     ADDRESS_MAX < address)
  {
    printf("memspace: address '%X' is out of range\n", address);
    return NULL;
  }
  if(ADDRESS_MAX < address + byte_count)
  {
    printf("memspace: '%d' bytes from the address '%X' is out of range\n",
        byte_count,
        address);
    return NULL;
  }
  if(!memory)
  {
    printf("memspace: the address of memory to obtain is NULL\n");
    return NULL;
  }

  memcpy(memory, &_memory[address], byte_count);
  return memory;
}

static bool memspace_execute_dump(const char *cmd, const int argc, const char *argv[])
{
  if(2 < argc)
  {
    printf("dump: too many arguments\n");
    return false;
  }

  int  dump_start = 0;
  int  dump_end   = 0;
  char *endptr    = NULL;

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
    if(ADDRESS_MIN > dump_start ||
       ADDRESS_MAX < dump_start)
    {
      printf("dump: start '%X' is out of range\n", dump_start);
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
    if(ADDRESS_MIN > dump_end ||
       ADDRESS_MAX < dump_end)
    {
      printf("dump: end '%X' is out of range\n", dump_end);
      return false;
    }

    if(dump_start > dump_end)
    {
      printf("dump: start '%X' is larger than end value '%X'\n",
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

static bool memspace_execute_edit(const char *cmd, const int argc, const char *argv[])
{
  if(2 != argc)
  {
    printf("edit: two arguments are required\n");
    return false;
  }

  int  address = 0;
  int  value   = 0;
  char *endptr = NULL;

  address = strtol(argv[0], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("edit: argument '%s' is invalid\n", argv[0]);
    return false;
  }
  if(ADDRESS_MIN > address ||
     ADDRESS_MAX < address)
  {
    printf("dump: address '%X' is out of range\n", address);
    return false;
  }

  value = strtol(argv[1], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("edit: argument '%s' is invalid\n", argv[1]);
    return false;
  }
  if(VALUE_MIN > value ||
     VALUE_MAX < value)
  {
    printf("dump: value '%X' is out of range\n", value);
    return false;
  }

  _memory[address] = value;

  return true;
}

static bool memspace_execute_fill(const char *cmd, const int argc, const char *argv[])
{
  if(3 != argc)
  {
    printf("fill: three arguments are required\n");
    return false;
  }

  int  start   = 0;
  int  end     = 0;
  int  value   = 0;
  char *endptr = NULL;

  start = strtol(argv[0], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("fill: argument '%s' is invalid\n", argv[0]);
    return false;
  }
  if(ADDRESS_MIN > start ||
     ADDRESS_MAX < start)
  {
    printf("fill: start '%X' is out of range\n", start);
    return false;
  }

  end = strtol(argv[1], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("fill: argument '%s' is invalid\n", argv[1]);
    return false;
  }
  if(ADDRESS_MIN > end ||
     ADDRESS_MAX < end)
  {
    printf("fill: end '%X' is out of range\n", end);
    return false;
  }

  if(start > end)
  {
    printf("fill: end '%X' is smaller than start '%X'\n", start, end);
    return false;
  }

  value = strtol(argv[2], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("fill: argument '%s' is invalid\n", argv[2]);
    return false;
  }
  if(VALUE_MIN > value ||
     VALUE_MAX < value)
  {
    printf("fill: value '%X' is out of range\n", value);
    return false;
  }

  memset(&_memory[start], value, end - start + 1);

  return true;
}

static bool memspace_execute_progaddr(const char *cmd, const int argc, const char *argv[])
{
  if(1 != argc)
  {
    printf("progaddr: one argument is required\n");
    return false;
  }

  int  value   = 0;
  char *endptr = NULL;

  value = strtol(argv[0], &endptr, HEX);
  if('\0' != *endptr)
  {
    printf("progaddr: argument '%s' is invalid\n", argv[0]);
    return false;
  }
  if(ADDRESS_MIN > value ||
     ADDRESS_MAX < value)
  {
    printf("progaddr: value '%X' is out of range\n", value);
    return false;
  }

  _progaddr = value;

  return true;
}

static bool memspace_execute_reset(const char *cmd, const int argc, const char *argv[])
{
  if(0 < argc)
  {
    printf("reset: too many arguments\n");
    return false;
  }

  memset(_memory, 0, MEMORY_SIZE);

  return true;
}
