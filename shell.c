/**
 * @file  shell.c
 * @brief A handler of shell related commands.
 */

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"

#include "mainloop.h"

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief          Show all files in the current directory.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool shell_execute_dir(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Show all executable commands.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool shell_execute_help(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Show all executed commands so far.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool shell_execute_history(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief          Set flag to quit this program.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool shell_execute_quit(const char *cmd, const int argc, const char *argv[]);

void shell_execute(const char *cmd, const int argc, const char *argv[])
{
  if(!strcmp("h", cmd) || !strcmp("help", cmd))
  {
    _is_command_executed = shell_execute_help(cmd, argc, argv);
  }
  else if(!strcmp("d", cmd) || !strcmp("dir", cmd))
  {
    _is_command_executed = shell_execute_dir(cmd, argc, argv);
  }
  else if(!strcmp("q", cmd) || !strcmp("quit", cmd))
  {
    _is_command_executed = shell_execute_quit(cmd, argc, argv);
  }
  else if(!strcmp("hi", cmd) || !strcmp("history", cmd))
  {
    _is_command_executed = shell_execute_history(cmd, argc, argv);
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

static bool shell_execute_dir(const char *cmd, const int argc, const char *argv[])
{
  if(0 < argc)
  {
    printf("dir: too many arguments\n");
    return false;
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
    return false;
  }
  closedir(dir);

  return true;
}

static bool shell_execute_help(const char *cmd, const int argc, const char *argv[])
{
  if(0 < argc)
  {
    printf("help: too many arguments\n");
    return false;
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

  return true;
}

static bool shell_execute_history(const char *cmd, const int argc, const char *argv[])
{
  if(0 < argc)
  {
    printf("history: too many arguments\n");
    return false;
  }

  const int log_count = logger_view_log();
  printf("%d\t", log_count + 1);
  printf("%s\n", cmd); // Current execution is considered successful.

  return true;
}

static bool shell_execute_quit(const char *cmd, const int argc, const char *argv[])
{
  if(0 < argc)
  {
    printf("quit: too many arguments\n");
    return false;
  }

  mainloop_quit();

  return true;
}
