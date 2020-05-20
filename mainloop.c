/**
 * @file  mainloop.c
 * @brief A mainloop of this program. Acts like a client of
 *        command pattern. Receives command and invokes handler.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "loader.h"
#include "logger.h"
#include "memspace.h"
#include "opcode.h"
#include "shell.h"
#include "symbol.h"

#include "mainloop.h"

/**
 * @def   ARGC_MAX
 * @brief The maximum value of argc. According to the specificaiton,
 *        the number of argc can be 3 at most. This value is set one
 *        bigger than it in order to sense if there are arguments more
 *        than expected.
 * @see   argc
 */
#define ARGC_MAX 4

/**
 * @brief Structure of elements required to execute command.
 * @note  Elements are ordered in a way that can reduce their size.
 */
struct command
{
  /** A command to be executed. */
  char *cmd;
  /** A NULL-terminated list of arguments. */
  char *argv[ARGC_MAX + 1];
  /** The number of arguments. */
  int  argc;
  /** An assigned handler. */
  void (*handler)(const char *, const int, const char *[]);
};

/**
 * @brief A const variable that holds the length of input.
 */
static const int INPUT_LEN = 64;

/**
 * @brief A command object that contains all information need to
 *        execute the command.
 */
static struct command _command = {0,};

/**
 * @brief A flag indicating whether quit mainloop or not.
 */
static bool _quit_mainloop = false;

/**
 * @brief  Assign handler according to the command.
 * @return True on success, false on fail.
 */
static bool mainloop_assign_handler(void);

/**
 * @brief           Tokenize input to cmd and argv, and return argc.
 * @param[in] input An array of char to be tokenized.
 */
static void mainloop_tokenize_input(char *input);

void mainloop_initialize(void)
{
  logger_initialize(INPUT_LEN);
  opcode_initialize();
  symbol_initialize();
}

void mainloop_launch(void)
{
  char input[INPUT_LEN];

  while(!_quit_mainloop)
  {
    printf("sicsim> ");
    if(fgets(input, INPUT_LEN, stdin))
    {
      mainloop_tokenize_input(input);
      if(mainloop_assign_handler())
      {
        if(_command.handler)
        {
          _command.handler((const char *)_command.cmd,
                           (const int)_command.argc,
                           (const char **)_command.argv);
        }
        else
        {
          printf("%s: command cannot be handled\n", _command.cmd);
        }
      }
      else
      {
        printf("%s: command not found\n", _command.cmd);
      }
    }
  }
}

void mainloop_quit(void)
{
  _quit_mainloop = true;
}

void mainloop_terminate(void)
{
  logger_terminate();
  opcode_terminate();
  symbol_terminate();
}

static bool mainloop_assign_handler(void)
{
  if(!_command.cmd)
  {
    // The input was empty.
    _command.handler = NULL;
    return false;
  }

  const char * const ASSEMBLER_CMDS[] = {"assemble",
                                         "symbol"};
  const char * const LOADER_CMDS[]    = {"loader"};
  const char * const MEMSPACE_CMDS[]  = {"du",
                                         "dump",
                                         "e",
                                         "edit",
                                         "f",
                                         "fill",
                                         "reset"};
  const char * const OPCODE_CMDS[]    = {"opcode",
                                         "opcodelist"};
  const char * const SHELL_CMDS[]     = {"h",
                                         "help",
                                         "d",
                                         "dir",
                                         "q",
                                         "quit",
                                         "hi",
                                         "history",
                                         "type"};
  const int ASSEMBLER_CMDS_COUNT = (int)(sizeof(ASSEMBLER_CMDS) /
                                         sizeof(ASSEMBLER_CMDS[0]));
  const int LOADER_CMDS_COUNT    = (int)(sizeof(LOADER_CMDS) /
                                         sizeof(LOADER_CMDS[0]));
  const int MEMSPACE_CMDS_COUNT  = (int)(sizeof(MEMSPACE_CMDS) /
                                         sizeof(MEMSPACE_CMDS[0]));
  const int OPCODE_CMDS_COUNT    = (int)(sizeof(OPCODE_CMDS) /
                                         sizeof(OPCODE_CMDS[0]));
  const int SHELL_CMDS_COUNT     = (int)(sizeof(SHELL_CMDS) /
                                         sizeof(SHELL_CMDS[0]));

  for(int i = 0; i < ASSEMBLER_CMDS_COUNT; ++i)
  {
    if(!strcmp(ASSEMBLER_CMDS[i], _command.cmd))
    {
      _command.handler = assembler_execute;
      return true;
    }
  }
  for(int i = 0; i < LOADER_CMDS_COUNT; ++i)
  {
    if(!strcmp(LOADER_CMDS[i], _command.cmd))
    {
      _command.handler = loader_execute;
      return true;
    }
  }
  for(int i = 0; i < MEMSPACE_CMDS_COUNT; ++i)
  {
    if(!strcmp(MEMSPACE_CMDS[i], _command.cmd))
    {
      _command.handler = memspace_execute;
      return true;
    }
  }
  for(int i = 0; i < OPCODE_CMDS_COUNT; ++i)
  {
    if(!strcmp(OPCODE_CMDS[i], _command.cmd))
    {
      _command.handler = opcode_execute;
      return true;
    }
  }
  for(int i = 0; i < SHELL_CMDS_COUNT; ++i)
  {
    if(!strcmp(SHELL_CMDS[i], _command.cmd))
    {
      _command.handler = shell_execute;
      return true;
    }
  }

  _command.handler = NULL;
  return false;
}

static void mainloop_tokenize_input(char *input)
{
  input[strlen(input) - 1] = '\0';
  _command.cmd = strtok(input, " \t");
  for(_command.argc = 0; _command.argc < ARGC_MAX; ++_command.argc)
  {
    // The input is separated by only comma.
    // Disclaimer: it is an assumption that following the specification.
    _command.argv[_command.argc] = strtok(NULL, " \t,");
    if(!_command.argv[_command.argc])
    {
      break;
    }
  }
}
