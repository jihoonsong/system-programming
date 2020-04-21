/**
 * @file  assembler.c
 * @brief A handler of assembler related commands.
 */

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"

#include "logger.h"
#include "opcode.h"
#include "symbol.h"

/**
 * @brief A const variable that holds the length of buffer used for
 *        file reading.
 */
static const int BUFFER_LEN = 110;

/**
 * @brief A const variable that holds the extension of asm file.
 */
static const char *ASM_EXTENSION = "asm";

/**
 * @brief A const variable that holds the length of asm file extension.
 */
static const int ASM_EXTENSION_LEN = 3;

/**
 * @brief Equals to 10.
 */
static const int DECIMAL = 10;

/**
 * @brief A const variable that holds the list of assembler directives.
 */
static const char *DIRECTIVES[] = {"START",
                                   "END",
                                   "BYTE",
                                   "WORD",
                                   "RESB",
                                   "RESW",
                                   "BASE",
                                   "NOBASE"};

/**
 * @brief A const variable that holds the number of assembler directives.
 */
const int DIRECTIVES_COUNT = (int)(sizeof(DIRECTIVES) /
                                   sizeof(DIRECTIVES[0]));

/**
 * @brief A const variable that holds the maximum error margin between floats.
 */
const float EPSILON = 1e-3f;

/**
 * @brief Equals to 16.
 */
static const int HEX = 16;

/**
 * @brief A const variable that holds the extension of int file.
 */
static const char *INT_EXTENSION = "int";

/**
 * @brief A const variable that holds the length of int file extension.
 */
static const int INT_EXTENSION_LEN = 3;

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
 * @brief A const variable that holds the manximum number of operands.
 */
static const int OPERANDS_COUNT = 2;

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
 * @brief         Check if the str is mnemonic.
 * @param[in] str A str to be validated.
 * @return        True if mnemonic, false otherwise.
 */
static bool assembler_is_mnemonic(const char *str);

/**
 * @brief              Tokenize line into label, mnemonic, and operands.
 * @param[in] buffer   A line to be tokenized.
 * @param[in] label    A label.
 * @param[in] mnemonic A mnemonic.
 * @param[in] operands An list of operands.
 * @return             False if empty or comment line, true otherwise.
 */
static bool assembler_tokenize_line(char *buffer,
                                    char **label,
                                    char **mnemonic,
                                    char *(*operands)[]);

/**
 * @brief                  Create symbol table. The symbol table contains
 *                         pairs of symbol and its locctr.
 * @param[in]  asm_file    A file pointer to an .asm file to be assembled.
 * @param[in]  int_file    A file pointer to an .int file to be written.
 * @param[out] program_len A length of prgram.
 */
static bool assembler_pass1(FILE *asm_file, FILE *int_file, int *program_len);

/**
 * @brief                 Write .lst file and obj file.
 * @param[in] asm_file    A file pointer to an .asm file to be assembled.
 * @param[in] int_file    A file pointer to an .int file to be read.
 * @param[in] lst_file    A file pointer to an .lst file to be written.
 * @param[in] obj_file    A file pointer to an .obj file to be written.
 * @param[in] program_len A length of program.
 */
static bool assembler_pass2(FILE *asm_file,
                            FILE *int_file,
                            FILE *lst_file,
                            FILE *obj_file,
                            int  program_len);

/**
 * @brief              Write a comment line to .lst file.
 * @param[in] lst_file A file pointer to an .lst file to be written.
 * @param[in] line     A line.
 * @param[in] buffer   A line from .asm file.
 */
static void assembler_write_lst_comment(FILE *lst_file,
                                        const int line,
                                        const char *buffer);

/**
 * @brief              Write a line to .lst file.
 * @param[in] lst_file A file pointer to an .lst file to be written.
 * @param[in] line     A line.
 * @param[in] locctr   A locctr.
 * @param[in] label    A label.
 * @param[in] mnemonic A mnemonic.
 * @param[in] operand1 The first operand.
 * @param[in] operand2 The second operand.
 */
static void assembler_write_lst_line(FILE *lst_file,
                                     const int line,
                                     const int locctr,
                                     const char *label,
                                     const char *mnemonic,
                                     const char *operand1,
                                     const char *operand2);

/**
 * @brief                 Write an object code to .lst file.
 * @param[in] lst_file    A file pointer to an .lst file to be written.
 * @param[in] object_code An object code.
 */
static void assembler_write_lst_object_code(FILE *lst_file,
                                            const char *object_code);

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

  char *int_filename = malloc((strlen(argv[0]) + 1) * sizeof(*int_filename));
  strcpy(int_filename, argv[0]);
  strcpy(int_filename + strlen(int_filename) - INT_EXTENSION_LEN, INT_EXTENSION);
  FILE *int_file = fopen(int_filename, "w");
  if(!int_file)
  {
    printf("assemble: cannot create '%s' file\n", int_filename);
    free(int_filename);
    return false;
  }

  symbol_new_table();

  int program_len = 0;
  bool is_success = assembler_pass1(asm_file, int_file, &program_len);
  if(!is_success)
  {
    symbol_show_error_msg();

    remove(int_filename);
    free(int_filename);
    return false;
  }

  char *lst_filename = malloc((strlen(argv[0]) + 1) * sizeof(*lst_filename));
  strcpy(lst_filename, argv[0]);
  strcpy(lst_filename + strlen(lst_filename) - LST_EXTENSION_LEN, LST_EXTENSION);
  FILE *lst_file = fopen(lst_filename, "w");
  if(!lst_file)
  {
    printf("assemble: cannot create '%s' file\n", lst_filename);
    fclose(asm_file);
    fclose(int_file);
    remove(int_filename);
    free(int_filename);
    free(lst_filename);
    return false;
  }

  char *obj_filename = malloc((strlen(argv[0]) + 1) * sizeof(*obj_filename));
  strcpy(obj_filename, argv[0]);
  strcpy(obj_filename + strlen(obj_filename) - OBJ_EXTENSION_LEN, OBJ_EXTENSION);
  FILE *obj_file = fopen(obj_filename, "w");
  if(!obj_file)
  {
    printf("assemble: cannot create '%s' file\n", obj_filename);
    fclose(asm_file);
    fclose(int_file);
    fclose(lst_file);
    remove(int_filename);
    remove(lst_filename);
    free(int_filename);
    free(lst_filename);
    free(obj_filename);
    return false;
  }

  is_success = assembler_pass2(asm_file, int_file, lst_file, obj_file, program_len);
  fclose(asm_file);
  fclose(int_file);
  fclose(lst_file);
  fclose(obj_file);
  if(!is_success)
  {
    symbol_show_error_msg();

    remove(obj_filename);
    remove(lst_filename);
    return false;
  }
  // TODO: remove int file!
  //remove(int_filename);
  free(int_filename);
  free(lst_filename);
  free(obj_filename);

  symbol_save_table();

  return true;
}

static bool assembler_execute_symbol(const char *cmd,
                                     const int argc,
                                     const char *argv[])
{
  if(0 < argc)
  {
    printf("assemble: too many arguments\n");
    return false;
  }

  symbol_show_table();

  return true;
}

static bool assembler_is_mnemonic(const char *str)
{
  if(!str)
  {
    return false;
  }

  for(int i = 0; i < DIRECTIVES_COUNT; ++i)
  {
    if(!strcmp(DIRECTIVES[i], str))
    {
      return true;
    }
  }

  if('+' == str[0])
  {
    ++str;
  }
  if(opcode_is_opcode(str))
  {
    return true;
  }

  return false;
}

static bool assembler_tokenize_line(char *buffer,
                                    char **label,
                                    char **mnemonic,
                                    char *(*operands)[])
{
  buffer[strlen(buffer) - 1] = '\0';

  char *substr = strtok(buffer, " \t");
  if(!substr || !strcmp(".", substr))
  {
    // This line is empty or comment.
    return false;
  }

  if(assembler_is_mnemonic(substr))
  {
    *label = NULL;
    *mnemonic = substr;
  }
  else
  {
    *label = substr;
    *mnemonic = strtok(NULL, " \t");
  }
  for(int i = 0; i < OPERANDS_COUNT; ++i)
  {
    (*operands)[i] = strtok(NULL, " \t,");
  }

  return true;
}

static bool assembler_pass1(FILE *asm_file, FILE *int_file, int *program_len)
{
  int  line                      = 0; // line is increased by 5.
  int  locctr                    = 0;
  char *label                    = NULL;
  char *mnemonic                 = NULL;
  char *operands[OPERANDS_COUNT];
  char buffer[BUFFER_LEN];

  // Read lines until meet the first non-empty and non-comment line.
  while(fgets(buffer, BUFFER_LEN, asm_file))
  {
    line += 5;
    if(assembler_tokenize_line(buffer, &label, &mnemonic, &operands))
    {
      if(!strcmp("START", mnemonic))
      {
        if(!operands[0] || operands[1])
        {
          symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
          return false;
        }

        locctr = strtol(operands[0], NULL, HEX);

        fprintf(int_file, "%d\t%X\n", line, locctr);

        // Read lines until meet the first non-empty and non-comment line.
        while(fgets(buffer, BUFFER_LEN, asm_file))
        {
          line += 5;
          if(assembler_tokenize_line(buffer, &label, &mnemonic, &operands))
          {
            fprintf(int_file, "%d\t%X\n", line, locctr);
            break;
          }
        }
      }
      else
      {
        locctr = 0;

        fprintf(int_file, "%d\t%X\n", line, locctr);
      }

      break;
    }
  }
  // Now, buffer has the first non-comment line after the START line.

  while(strcmp("END", mnemonic))
  {
    if(label)
    {
      if(symbol_is_exist(label))
      {
        symbol_set_error(DUPLICATE_SYMBOL, line, label);
        return false;
      }
      else
      {
        if(!symbol_insert_symbol(label, locctr))
        {
          printf("assemble: symbol '%s' insertion failed\n", label);
          return false;
        }
      }
    }

    if(opcode_is_opcode(mnemonic))
    {
      float format = opcode_get_format(mnemonic);
      if(fabsf(1.0f - format) <= EPSILON)
      {
        locctr += 1;
      }
      else if(fabsf(2.0f - format) <= EPSILON)
      {
        locctr += 2;
      }
      else if(fabsf(3.5f - format) <= EPSILON)
      {
        locctr += 3;
      }
      else
      {
        symbol_set_error(INVALID_OPCODE, line, mnemonic);
        return false;
      }
    }
    else if('+' == mnemonic[0] && opcode_is_opcode(&mnemonic[1]))
    {
      float format = opcode_get_format(&mnemonic[1]);
      if(fabsf(3.5f - format) <= EPSILON)
      {
        locctr += 4;
      }
      else
      {
        symbol_set_error(INVALID_OPCODE, line, mnemonic);
        return false;
      }
    }
    else if(!strcmp("BYTE", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      if('\'' != operands[0][1] ||
         '\'' != operands[0][strlen(operands[0]) - 1])
      {
        symbol_set_error(INVALID_OPERAND, line, operands[0]);
        return false;
      }
      for(int i = 2; i < strlen(operands[0]) - 1; ++i)
      {
        if(!(('A' <= operands[0][i] && 'Z' >= operands[0][i]) ||
             ('0' <= operands[0][i] && '9' >= operands[0][i])))
        {
          // If not upper alphbets or number.
          symbol_set_error(INVALID_OPERAND, line, operands[0]);
          return false;
        }
      }

      if('C' == operands[0][0])
      {
        locctr += strlen(operands[0]) - 3;
      }
      else if('X' == operands[0][0])
      {
        // Round up if length is odd.
        locctr += (strlen(operands[0]) - 3 + 1) / 2;
      }
      else
      {
        symbol_set_error(INVALID_OPERAND, line, operands[0]);
        return false;
      }
    }
    else if(!strcmp("WORD", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      locctr += 3;
    }
    else if(!strcmp("RESB", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      locctr += strtol(operands[0], NULL, DECIMAL);
    }
    else if(!strcmp("RESW", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      locctr += 3 * strtol(operands[0], NULL, DECIMAL);
    }
    else if(!strcmp("BASE", mnemonic) ||
            !strcmp("NOBASE", mnemonic))
    {
      // Do nothing.
    }
    else
    {
      symbol_set_error(INVALID_OPCODE, line, mnemonic);
      return false;
    }

    do
    {
      if(!fgets(buffer, BUFFER_LEN, asm_file))
      {
        printf("assemble: END mnemonic is not found\n");
        return false;
      }
      line += 5;

      // Skip empty or comment lines.
    } while(!assembler_tokenize_line(buffer, &label, &mnemonic, &operands));

    fprintf(int_file, "%d\t%X\t", line, locctr);
  }

  *program_len = locctr - start;

  return true;
}

static bool assembler_pass2(FILE *asm_file,
                            FILE *int_file,
                            FILE *lst_file,
                            FILE *obj_file,
                            int  program_len)
{
  // TODO: to be implemented.
  printf("pass2 invoked\n");
  return true;
}

static void assembler_write_lst_comment(FILE *lst_file,
                                        const int line,
                                        const char *buffer)
{
  fprintf(lst_file, "%3d\t%3s\t%s\n", line, " ", buffer);
}

static void assembler_write_lst_line(FILE *lst_file,
                                     const int line,
                                     const int locctr,
                                     const char *label,
                                     const char *mnemonic,
                                     const char *operand1,
                                     const char *operand2)
{
  fprintf(lst_file, "%3d", line);
  if(strcmp("BASE", mnemonic) &&
     strcmp("NOBASE", mnemonic) &&
     strcmp("END", mnemonic))
  {
    fprintf(lst_file, "\t%04X", locctr);
  }
  else
  {
    fprintf(lst_file, "\t%4s", " ");
  }
  fprintf(lst_file, "\t%-6s", label ? label : " ");
  fprintf(lst_file, "\t%-6s", mnemonic);
  fprintf(lst_file, "\t%-6s", operand1 ? : " ");
  fprintf(lst_file, "%s%-6s", operand2 ? ", " : " ", operand2 ? operand2 : " ");
}

static void assembler_write_lst_object_code(FILE *lst_file,
                                            const char *object_code)
{
  fprintf(lst_file, "\t%-6s\n", object_code ? object_code : "");
}
