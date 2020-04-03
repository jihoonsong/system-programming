/**
 * @file  shell.h
 * @brief A handler of shell related commands.
 */

#include <stdio.h>
#include <string.h>

/**
 * @brief          Show all files in the current directory.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static void shell_execute_dir(char *cmd, int argc, char *argv[]);

/**
 * @brief          Show all executable commands.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static void shell_execute_help(char *cmd, int argc, char *argv[]);

void shell_execute(char *cmd, int argc, char *argv[])
{
  if(!strcmp("h", cmd) || !strcmp("help", cmd))
  {
    shell_execute_help(cmd, argc, argv);
  }
  else if(!strcmp("d", cmd) || !strcmp("dir", cmd))
  {
    shell_execute_dir(cmd, argc, argv);
  }
  else
  {
    printf("%s: command not found\n", cmd);
  }
}

static void shell_execute_dir(char *cmd, int argc, char *argv[])
{
  if(0 < argc)
  {
    printf("dir: too many arguments\n");
    return;
  }

  // TODO: to be implemented.
}

static void shell_execute_help(char *cmd, int argc, char *argv[])
{
	if(0 < argc)
  {
    printf("help: too many arguments\n");
    return;
  }

  printf("h[elp]\n");
  printf("d[ir]\n");
  printf("q[uit]\n");
  printf("hi[story]\n");
  printf("du[mp] [start, end]\n");
  printf("e[dit] address, value\n");
  printf("f[ill] start, end, value\n");
  printf("reset\n");
  printf("opcode mnemonic\n");
  printf("opcodelist\n");
}
