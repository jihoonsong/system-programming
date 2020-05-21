/**
 * @file  loader.c
 * @brief A handler of loader related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "loader.h"

#include "external_symbol.h"
#include "logger.h"
#include "memspace.h"

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief          Perform linking and loading.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool loader_execute_loader(const char *cmd,
                                  const int  argc,
                                  const char *argv[]);

/**
 * @brief                Create external symbol table.
 * @param[in] file_count The number of object files.
 * @param[in] file_names A list of object file names.
 */
static void loader_pass1(const int file_count, const char *file_names[]);

/**
 * @brief                Load object code on memory.
 * @param[in] file_count The number of object files.
 * @param[in] file_names A list of object file names.
 */
static void loader_pass2(const int file_count, const char *file_names[]);

void loader_execute(const char *cmd,
                    const int  argc,
                    const char *argv[])
{
  if(!strcmp("loader", cmd))
  {
    _is_command_executed = loader_execute_loader(cmd, argc, argv);
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

static bool loader_execute_loader(const char *cmd,
                                  const int  argc,
                                  const char *argv[])
{
  if(0 == argc)
  {
    printf("loader: at least one object file is required\n");
    return false;
  }
  if(3 < argc)
  {
    printf("loader: at most three object files can be loaded\n");
    return false;
  }

  external_symbol_initialize();

  loader_pass1(argc, argv);
  loader_pass2(argc, argv);

  external_symbol_show_table();

  return true;
}

static void loader_pass1(const int file_count, const char *file_names[])
{
  printf("loader_pass1\n");
}

static void loader_pass2(const int file_count, const char *file_names[])
{
  printf("loader_pass2\n");
}
