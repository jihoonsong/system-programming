/**
 * @file 20131567.c
 * @brief The starting point of this program.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @def   INPUT_LEN
 * @brief The length of input string. Its value can be modified
 *        if it is long enough to hold command and arguments.
 * @see   input
 */
#define INPUT_LEN 64

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
 * @brief An array of char that holds received command and arguments.
 */
static char input[INPUT_LEN] = {0,};

/**
 * @brief Assign handler designated to the command.
 */
void assign_handler(void);

/**
 * @brief Initialize the global states.
 */
void initialize(void);

/**
 * @brief The main loop of this program.
 * @details Receives command along with its arguments,
 *          check if the given command is valid,
 *          pass it to the designated handler if valid,
 *          and ignore it if invalid.
 */
void mainloop(void);

/**
 * @brief Release all allocated memory.
 */
void terminate(void);

/**
 * @brief Tokenize input into cmd and argv, and update argc.
 * @see   cmd
 * @see   argv
 * @see   argc
 */
static void tokenize_input(void);

/**
 * @brief Initialize states and start main loop.
 *        Clean up memory when the main loop is over.
 */
int main(void)
{
  initialize();
  mainloop();
  terminate();

  return 0;
}

void assign_handler(void)
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

void initialize(void)
{
  // TODO: to be implemented.
}

void mainloop(void)
{
  while(true)
  {
    printf("sicsim> ");
    if(fgets(input, INPUT_LEN, stdin))
    {
      tokenize_input();
      assign_handler();

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

void terminate(void)
{
  // TODO: to be implemented.
}

static void tokenize_input(void)
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
