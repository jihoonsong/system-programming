/**
 * @file  assembler.c
 * @brief A handler of assembler related commands.
 */

#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"

#include "logger.h"

/**

/**
 * @brief A const variable that holds the extension of asm file.
 */
static const char *ASM_EXTENSION = "asm";

/**
 * @brief A const variable that holds the length of asm file extension.
 */
static const int ASM_EXTENSION_LEN = 3;

/**
 * @brief A const variable that holds the extension of lst file.
 */
static const char *LST_EXTENSION = "lst";

/**
 * @brief A const variable that holds the length of lst file extension.
 */
static const int LST_EXTENSION_LEN = 3;

/**
 * @brief A const variable that holds the extension of obj file.
 */
static const char *OBJ_EXTENSION = "obj";

/**
 * @brief A const variable that holds the length of obj file extension.
 */
static const int OBJ_EXTENSION_LEN = 3;

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

/**
 * @brief              Create symbol table. The symbol table contains
                       pairs of symbol and its locctr.
 * @param[in] asm_file A file pointer to an .asm file to be assembled.
 */
static bool assembler_pass1(FILE *asm_file);

/**
 * @brief              Write .lst file and obj file.
 * @param[in] lst_file A file pointer to an .lst file to be written.
 * @param[in] obj_file A file pointer to an .obj file to be written.
 */
static bool assembler_pass2(FILE *lst_file, FILE *obj_file);

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
  if(1 != argc)
  {
    printf("assemble: one argument is required\n");
    return false;
  }

  if(opendir(argv[0]))
  {
    // Ignore directory file.
    printf("assemble: '%s' is a directory\n", argv[0]);
    return false;
  }

  if(strcmp(ASM_EXTENSION, argv[0] + strlen(argv[0]) - ASM_EXTENSION_LEN))
  {
    printf("assemble: '%s' is not .asm file\n", argv[0]);
    return false;
  }

  FILE *asm_file = fopen(argv[0], "r");
  if(!asm_file)
  {
    printf("assemble: there is no such file '%s'\n", argv[0]);
    return false;
  }

  bool is_success = assembler_pass1(asm_file);
  fclose(asm_file);
  if(!is_success)
  {
    // Although there was an error, assemble command was executed.
    return true;
  }

  char *lst_filename = malloc((strlen(argv[0]) + 1) * sizeof(*lst_filename));
  strcpy(lst_filename, argv[0]);
  strcpy(lst_filename + strlen(lst_filename) - LST_EXTENSION_LEN, LST_EXTENSION);
  FILE *lst_file = fopen(lst_filename, "w");
  if(!lst_file)
  {
    printf("assemble: cannot create '%s' file\n", lst_filename);
    free(lst_filename);
    return true;
  }

  char *obj_filename = malloc((strlen(argv[0]) + 1) * sizeof(*obj_filename));
  strcpy(obj_filename, argv[0]);
  strcpy(obj_filename + strlen(obj_filename) - OBJ_EXTENSION_LEN, OBJ_EXTENSION);
  FILE *obj_file = fopen(obj_filename, "w");
  if(!obj_file)
  {
    printf("assemble: cannot create '%s' file\n", obj_filename);
    remove(lst_filename);
    free(obj_filename);
    free(lst_filename);
    return true;
  }

  is_success = assembler_pass2(lst_file, obj_file);
  fclose(obj_file);
  fclose(lst_file);
  if(!is_success)
  {
    remove(obj_filename);
    remove(lst_filename);
  }
  free(obj_filename);
  free(lst_filename);

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

static bool assembler_pass1(FILE *asm_file)
{
  // TODO: to be implemented.
  printf("pass1 invoked\n");
  return true;
}

static bool assembler_pass2(FILE *lst_file, FILE *obj_file)
{
  // TODO: to be implemented.
  printf("pass2 invoked\n");
  return true;
}
