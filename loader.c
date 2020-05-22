/**
 * @file  loader.c
 * @brief A handler of loader related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"

#include "external_symbol.h"
#include "logger.h"
#include "memspace.h"

/**
 * @brief A const variable that holds the length of buffer used for
 *        file reading.
 */
static const int BUFFER_LEN = 0xFF;

/**
 * @brief Equals to 16.
 */
static const int HEX = 16;

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

/**
 * @brief                     Tokenize define record.
 * @param[in]  buffer         The content of record to be tokenized.
 * @param[out] symbol_name    The name of symbol.
 * @param[out] symbol_address The relative address of symbol.
 */
static void loader_tokenize_define_record(const char *buffer,
                                          char       *symbol_name,
                                          int        *symbol_address);

/**
 * @brief                             Tokenize header record.
 * @param[in]  buffer                 The content of record to be tokenized.
 * @param[out] control_section_name   The name of control section.
 * @param[out] control_section_length The length of control section.
 */
static void loader_tokenize_header_record(const char *buffer,
                                          char       *control_section_name,
                                          int        *control_section_length);
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
  char control_section_name[7] = {0,};
  int  control_section_length  = 0;
  int  control_section_address = 0;
  FILE *obj_file               = NULL;
  char buffer[BUFFER_LEN];

  control_section_address = memspace_get_progaddr();

  for(int i = 0; i < file_count; ++i)
  {
    obj_file = fopen(file_names[i], "r");
    if(!obj_file)
    {
      printf("loader: there is no such file '%s'\n", file_names[i]);
      return;
    }

    while(fgets(buffer, BUFFER_LEN, obj_file))
    {
      char record_type = buffer[0];
      if('H' == record_type)
      {
        buffer[strlen(buffer) - 1] = '\0'; // Replace newline with null byte.
        break;
      }
    }
    // Now, buffer has Header record.

    loader_tokenize_header_record(buffer,
                                  control_section_name,
                                  &control_section_length);
    external_symbol_insert_control_section(control_section_name,
                                           control_section_address,
                                           control_section_length);

    while(fgets(buffer, BUFFER_LEN, obj_file))
    {
      buffer[strlen(buffer) - 1] = '\0'; // Replace newline with null byte.

      char record_type = buffer[0];
      if('D' == record_type)
      {
        char symbol_name[7] = {0,};
        int  symbol_address = 0;
        int  symbol_count   = (strlen(buffer) - 1) / 12;
        for(int i = 0; i < symbol_count; ++i)
        {
          loader_tokenize_define_record(&buffer[i * 12],
                                        symbol_name,
                                        &symbol_address);
          external_symbol_insert_symbol(control_section_name,
                                        symbol_name,
                                        control_section_address + symbol_address);
        }
      }
      else if('E' == record_type)
      {
        break;
      }
      else
      {
        // Do nothing.
      }
    }

    control_section_address += control_section_length;

    memset(control_section_name, 0, sizeof(control_section_name));
    fclose(obj_file);
  }
}

static void loader_pass2(const int file_count, const char *file_names[])
{
  printf("loader_pass2\n");
}

static void loader_tokenize_define_record(const char *buffer,
                                          char       *symbol_name,
                                          int        *symbol_address)
{
  strncpy(symbol_name, &buffer[1], 6);
  symbol_name[6] = '\0';

  char address[7] = {0,};
  strncpy(address, &buffer[7], 6);
  *symbol_address = strtol(address, NULL, HEX);
}

static void loader_tokenize_header_record(const char *buffer,
                                          char       *control_section_name,
                                          int        *control_section_length)
{
  strncpy(control_section_name, &buffer[1], 6);
  control_section_name[6] = '\0';

  *control_section_length = strtol(&buffer[13], NULL, HEX);
}