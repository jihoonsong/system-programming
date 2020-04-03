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
  SHELL,
  MEMSPACE,
  OPCODE,
  NONE,
};

/**
 * @brief The number of arguments. Its maximum value is ARGC_MAX.
 * @see   ARGC_MAX
 * @see   argv
 */
static int argc = 0;

/**
 * @brief An list of arguments with trailing NULL.
 * @see   ARGC_MAX
 * @see   argc
 */
static char *argv[ARGC_MAX + 1] = {NULL,};

/**
 * @brief A pointer to an array of char that contains command.
 */
static char *cmd = NULL;

/**
 * @brief A handler assigned to the current command.
 */
static enum handler handler = NONE;

/**
 * @brief Assign handler designated to the command.
 */
static void mainloop_assign_handler(void);

/**
 * @brief Tokenize input into cmd and argv, and update argc.
 * @see   cmd
 * @see   argv
 * @see   argc
 */
static void mainloop_tokenize_input(char *input);

void mainloop_initialize(void)
{
  // TODO: to be implemented.
}

void mainloop_launch(void)
{
  const int INPUT_LEN = 64;
  char      input[INPUT_LEN];

  while(true)
  {
    printf("sicsim> ");
    if(fgets(input, INPUT_LEN, stdin))
    {
      mainloop_tokenize_input(input);
      mainloop_assign_handler();

      // Test for assign_handler().
      if(SHELL == handler)
      {
        printf("shell\n");
      }
      else if(MEMSPACE == handler)
      {
        printf("memspace\n");
      }
      else if(OPCODE == handler)
      {
        printf("opcode\n");
      }
      else
      {
        printf("none\n");
      }
    }
  }
}

void mainloop_terminate(void)
{
  // TODO: to be implemented.
}

static void mainloop_assign_handler(void)
{
  const char * const shell_cmds[] = {"h",
                                     "help",
                                     "d",
                                     "dir",
                                     "q",
                                     "quit",
                                     "hi",
                                     "history"};
  const char * const memspace_cmds[] = {"du",
                                        "dump",
                                        "e",
                                        "edit",
                                        "f",
                                        "fill",
                                        "reset"};
  const char * const opcode_cmds[] = {"opcode",
                                      "opcodelist"};
  const int shell_cmds_count = (int)(sizeof(shell_cmds) /
                                     sizeof(shell_cmds[0]));
  const int memspace_cmds_count = (int)(sizeof(memspace_cmds) /
                                        sizeof(memspace_cmds[0]));
  const int opcode_cmds_count = (int)(sizeof(opcode_cmds) /
                                      sizeof(opcode_cmds[0]));

  for(int i = 0; i < shell_cmds_count; ++i)
  {
    if(!strcmp(cmd, shell_cmds[i]))
    {
      handler = SHELL;
      return;
    }
  }
  for(int i = 0; i < memspace_cmds_count; ++i)
  {
    if(!strcmp(cmd, memspace_cmds[i]))
    {
      handler = MEMSPACE;
      return;
    }
  }
  for(int i = 0; i < opcode_cmds_count; ++i)
  {
    if(!strcmp(cmd, opcode_cmds[i]))
    {
      handler = OPCODE;
      return;
    }
  }
  handler = NONE;
}

static void mainloop_tokenize_input(char *input)
{
  input[strlen(input) - 1] = '\0';

  cmd = strtok(input, " \t");
  for(argc = 0; argc < ARGC_MAX; ++argc)
  {
    // The input is separated by only comma.
    // Disclaimer: this is an assumption that obeying the specification.
    argv[argc] = strtok(NULL, " \t,");
    if(!argv[argc])
    {
      break;
    }
  }
}
