/**
 * @file  shell.h
 * @brief A handler of shell related commands.
 */

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mainloop.h"

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

/**
 * @brief          Set flag to quit this program.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static void shell_execute_quit(char *cmd, int argc, char *argv[]);

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
  else if(!strcmp("q", cmd) || !strcmp("quit", cmd))
  {
    shell_execute_quit(cmd, argc, argv);
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

  DIR *dir = NULL;
  if((dir = opendir(".")))
  {
    struct dirent *ent = NULL;
		while((ent = readdir(dir)))
		{
      if(!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
      {
        // Skipped ./ and ../ for simplicity.
        continue;
      }

			printf("%s", ent->d_name);
      if(DT_DIR == ent->d_type)
      {
        // This entry is a directory.
        printf("/");
      }
			if(DT_REG == ent->d_type && (0 == access(ent->d_name, X_OK)))
			{
        // This entry is executable file.
				printf("*");
			}
			printf("\n");
		}
  }
  else
  {
    printf("dir: cannot open directory\n");
    return;
  }
  closedir(dir);
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

static void shell_execute_quit(char *cmd, int argc, char *argv[])
{
	if(0 < argc)
  {
    printf("quit: too many arguments\n");
    return;
  }

  mainloop_quit();
}
