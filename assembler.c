/**
 * @file  assembler.c
 * @brief A handler of assembler related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"

#include "logger.h"

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief          Read .asm file and create .obj and .lst files.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool assembler_execute_assemble(const char *cmd,
                                       const int argc,
                                       const char *argv[]);

/**
 * @brief          Print symbol table created during assembly.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool assembler_execute_symbol(const char *cmd,
                                     const int argc,
                                     const char *argv[]);

void assembler_execute(const char *cmd,
                       const int argc,
                       const char *argv[])
{
  if(!strcmp("assemble", cmd))
  {
    _is_command_executed = assembler_execute_assemble(cmd, argc, argv);
  }
  else if(!strcmp("symbol", cmd))
  {
    _is_command_executed = assembler_execute_symbol(cmd, argc, argv);
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

static bool assembler_execute_assemble(const char *cmd,
                                       const int argc,
                                       const char *argv[])
{
  // TODO: to be implemented.
  printf("assemble invoked\n");
  return true;
}

static bool assembler_execute_symbol(const char *cmd,
                                     const int argc,
                                     const char *argv[])
{
  // TODO: to be implemented.
  printf("symbol invoked\n");
  return true;
}
