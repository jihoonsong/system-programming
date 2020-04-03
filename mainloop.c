/**
 * @file  mainloop.c
 * @brief A mainloop of this program. Acts like a client of
 *        command pattern. Receives command and invokes handler.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief Types of handler.
 */
enum handler
{
  NONE,
  SHELL,
  MEMSPACE,
  OPCODE,
};

/**
 * @brief Structure of elements required to execute command.
 * @note  Elements are ordered in a way that can reduce their size.
 */
struct command
{
  char *cmd;                /** A command to be executed. */
  char *argv[ARGC_MAX + 1]; /** A NULL-terminated list of arguments. */
  int  argc;                /** The number of arguments. */
  enum handler handler;     /** An assigned handler. */
};

/**
 * @brief A command object that contains all information need to
 *        execute the command.
 */
static struct command command = {0,};

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
  // TODO: to be implemented.
}

void mainloop_launch(void)
{
  const int      INPUT_LEN        = 64;
  char           input[INPUT_LEN];

  while(true)
  {
    printf("sicsim> ");
    if(fgets(input, INPUT_LEN, stdin))
    {
      mainloop_tokenize_input(input);
      if(mainloop_assign_handler())
      {
        // TODO: invoke handler.
      }
      else
      {
        printf("%s: command not found\n", command.cmd);
      }
    }
  }
}

void mainloop_terminate(void)
{
  // TODO: to be implemented.
}

static bool mainloop_assign_handler(void)
{
  const char * const SHELL_CMDS[]    = {"h",
                                        "help",
                                        "d",
                                        "dir",
                                        "q",
                                        "quit",
                                        "hi",
                                        "history"};
  const char * const MEMSPACE_CMDS[] = {"du",
                                        "dump",
                                        "e",
                                        "edit",
                                        "f",
                                        "fill",
                                        "reset"};
  const char * const OPCODE_CMDS[]    = {"opcode",
                                         "opcodelist"};
  const int SHELL_CMDS_COUNT    = (int)(sizeof(SHELL_CMDS) /
                                        sizeof(SHELL_CMDS[0]));
  const int MEMSPACE_CMDS_COUNT = (int)(sizeof(MEMSPACE_CMDS) /
                                        sizeof(MEMSPACE_CMDS[0]));
  const int OPCODE_CMDS_COUNT   = (int)(sizeof(OPCODE_CMDS) /
                                        sizeof(OPCODE_CMDS[0]));

  for(int i = 0; i < SHELL_CMDS_COUNT; ++i)
  {
    if(!strcmp(SHELL_CMDS[i], command.cmd))
    {
      command.handler = SHELL;
      return true;
    }
  }
  for(int i = 0; i < MEMSPACE_CMDS_COUNT; ++i)
  {
    if(!strcmp(MEMSPACE_CMDS[i], command.cmd))
    {
      command.handler = MEMSPACE;
      return true;
    }
  }
  for(int i = 0; i < OPCODE_CMDS_COUNT; ++i)
  {
    if(!strcmp(OPCODE_CMDS[i], command.cmd))
    {
      command.handler = OPCODE;
      return true;
    }
  }

  command.handler = NONE;
  return false;
}

static void mainloop_tokenize_input(char *input)
{
  input[strlen(input) - 1] = '\0';
  command.cmd = strtok(input, " \t");
  for(command.argc = 0; command.argc < ARGC_MAX; ++command.argc)
  {
    // The input is separated by only comma.
    // Disclaimer: it is an assumption that following the specification.
    command.argv[command.argc] = strtok(NULL, " \t,");
    if(!command.argv[command.argc])
    {
      break;
    }
  }
}
