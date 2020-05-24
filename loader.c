/**
 * @file  loader.c
 * @brief A handler of loader related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"

#include "debugger.h"
#include "external_symbol.h"
#include "logger.h"
#include "memspace.h"

/**
 * @brief A const variable that holds the length of buffer used for
 *        file reading.
 */
static const int BUFFER_LEN = 0xFF;

/**
 * @brief Equals to 10.
 */
static const int DECIMAL = 10;

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
 * @return               True on success, false otherwise.
 */
static bool loader_pass1(const int file_count, const char *file_names[]);

/**
 * @brief                Load object code on memory.
 * @param[in] file_count The number of object files.
 * @param[in] file_names A list of object file names.
 * @return               True on success, false otherwise.
 */
static bool loader_pass2(const int file_count, const char *file_names[]);

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

/**
 * @brief                          Tokenize text record.
 * @param[in]  buffer              The content of record to be tokenized.
 * @param[out] object_code_address The address of object code.
 * @param[out] object_code_length  The length of object code.
 * @param[out] object_code         The packed object code. (1 column per byte)
 */
static void loader_tokenize_text_record(const char    *buffer,
                                        int           *object_code_address,
                                        int           *object_code_length,
                                        unsigned char *object_code);

/**
 * @brief                           Tokenize modification record.
 * @param[in]  buffer               The content of record to be tokenized.
 * @param[out] modification_address The address of the field to be modified.
 * @param[out] modification_length  The length of the field to be modified.
 * @param[out] modification_flag    Modification flag. (+ or -)
 * @param[out] reference_num        The reference number of external symbol
 *                                  whose value is to be added to or subtracted
 *                                  from the field.
 */
static void loader_tokenize_modification_record(const char *buffer,
                                                int        *modification_address,
                                                int        *modification_length,
                                                char       *modification_flag,
                                                int        *reference_num);

/**
 * @brief                         Tokenize refer record.
 * @param[in]  buffer             The content of record to be tokenized.
 * @param[out] external_reference A list of addresses of external references.
 */
static void loader_tokenize_refer_record(const char *buffer,
                                         int        *external_references);

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

  bool is_success = loader_pass1(argc, argv);
  if(!is_success)
  {
    return false;
  }

  is_success = loader_pass2(argc, argv);
  if(!is_success)
  {
    return false;
  }

  external_symbol_show_table();

  return true;
}

static bool loader_pass1(const int file_count, const char *file_names[])
{
  int  program_address         = 0;
  char control_section_name[7] = {0,};
  int  control_section_length  = 0;
  int  control_section_address = 0;
  FILE *obj_file               = NULL;
  char buffer[BUFFER_LEN];

  program_address         = memspace_get_progaddr();
  control_section_address = program_address;

  for(int i = 0; i < file_count; ++i)
  {
    obj_file = fopen(file_names[i], "r");
    if(!obj_file)
    {
      printf("loader: there is no such file '%s'\n", file_names[i]);
      return false;
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

  debugger_prepare_run(program_address, control_section_address);

  return true;
}

static bool loader_pass2(const int file_count, const char *file_names[])
{
  char control_section_name[7]  = {0,};
  int  control_section_length   = 0;
  int  control_section_address  = 0;
  int  external_references[100] = {0,};
  FILE *obj_file                = NULL;
  char buffer[BUFFER_LEN];

  control_section_address = memspace_get_progaddr();

  for(int i = 0; i < file_count; ++i)
  {
    obj_file = fopen(file_names[i], "r");
    if(!obj_file)
    {
      printf("loader: there is no such file '%s'\n", file_names[i]);
      return false;
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
    external_references[1] = external_symbol_get_address(control_section_name);

    while(fgets(buffer, BUFFER_LEN, obj_file))
    {
      buffer[strlen(buffer) - 1] = '\0'; // Replace newline with null byte.

      char record_type = buffer[0];
      if('T' == record_type)
      {
        int           object_code_address = 0;
        int           object_code_length  = 0;
        unsigned char object_code[60]     = {0,};
        loader_tokenize_text_record(buffer,
                                    &object_code_address,
                                    &object_code_length,
                                    object_code);
        bool is_load_success = memspace_set_memory(control_section_address +
                                                   object_code_address,
                                                   object_code,
                                                   object_code_length);
        if(!is_load_success)
        {
          printf("loader: loading text record at '%05X' failed\n",
              control_section_address + object_code_address);
          return false;
        }
      }
      else if('M' == record_type)
      {
        int  modification_address = 0;
        int  modification_length  = 0;
        char modification_flag    = 0;
        int  reference_num        = 0;
        loader_tokenize_modification_record(buffer,
                                            &modification_address,
                                            &modification_length,
                                            &modification_flag,
                                            &reference_num);
        bool is_modify_success = memspace_modify_memory(control_section_address +
                                                        modification_address,
                                                        modification_length,
                                                        modification_flag,
                                                        external_references[reference_num]);
        if(!is_modify_success)
        {
          printf("loader: modifying memory at '%05X' failed\n",
              control_section_address + modification_address);
          return false;
        }
      }
      else if('R' == record_type)
      {
        loader_tokenize_refer_record(buffer,
                                     external_references);
      }
      else if('E' == record_type)
      {
        break;
      }
      else
      {
        // When record type is 'D' or comment line.
        // Do nothing.
      }
    }

    control_section_address += control_section_length;

    memset(control_section_name, 0, sizeof(control_section_name));
    memset(external_references, 0, sizeof(external_references));
    fclose(obj_file);
  }

  return true;
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

static void loader_tokenize_text_record(const char    *buffer,
                                        int           *object_code_address,
                                        int           *object_code_length,
                                        unsigned char *object_code)
{
  char address[7] = {0,};
  strncpy(address, &buffer[1], 6);
  *object_code_address = strtol(address, NULL, HEX);

  char length[3] = {0,};
  strncpy(length, &buffer[7], 2);
  *object_code_length = strtol(length, NULL, HEX);

  for(int i = 0; i < *object_code_length; ++i)
  {
    char byte[3] = {0,};
    strncpy(byte, &buffer[9 + i * 2], 2);
    object_code[i] = strtol(byte, NULL, HEX);
  }
}

static void loader_tokenize_modification_record(const char *buffer,
                                                int        *modification_address,
                                                int        *modification_length,
                                                char       *modification_flag,
                                                int        *reference_num)
{
  char address[7] = {0,};
  strncpy(address, &buffer[1], 6);
  *modification_address = strtol(address, NULL, HEX);

  char length[3] = {0,};
  strncpy(length, &buffer[7], 2);
  *modification_length = strtol(length, NULL, HEX);

  *modification_flag = buffer[9];

  char num[3] = {0,};
  strncpy(num, &buffer[10], 2);
  *reference_num = strtol(num, NULL, DECIMAL);
}

static void loader_tokenize_refer_record(const char *buffer,
                                         int        *external_references)
{
  int external_reference_count = (strlen(buffer) - 1 + 5) / 8;
  for(int i = 0; i < external_reference_count; ++i)
  {
    char num[3] = {0,};
    strncpy(num, &buffer[1 + i * 8], 2);
    int reference_num = strtol(num, NULL, DECIMAL);

    char symbol[7] = {0,};
    strncpy(symbol, &buffer[3 + i * 8], 6);
    for(int i = strlen(symbol); i < 6; ++i)
    {
      // A length of the last symbol of refer record could be shorter than 6.
      // However, all symbols are blank space padded 6 letters long, so pad
      // blank spaces at the end of the last symbol.
      symbol[i] = ' ';
    }

    int address = external_symbol_get_address(symbol);
    external_references[reference_num] = address;
  }
}
