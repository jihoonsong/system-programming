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

#define MODIF_RECORD_LEN 10

/**
 * @brief Structure of symbol elements.
 */
struct modif_record
{
  /** A record content. */
  char modif[MODIF_RECORD_LEN];
  /** A pointer to the next record element. */
  struct modif_record *next;
};

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
 * @brief Equals to 0x000, equals to decimal 0 in unsigned.
 */
static const int BASE_MIN = 0x000;

/**
 * @brief Equals to 0x7FF, equals to decimal 4095 in unsigned.
 */
static const int BASE_MAX = 0xFFF;

/**
 * @brief Equals to 10.
 */
static const int DECIMAL = 10;

/**
 * @brief Equals to -0x800, equals to decimal -2048 in two's complement.
 */
static const int DISPLACEMENT_MIN = -0x800;

/**
 * @brief Equals to 0xFFF. The displacement field is 12-bits long.
 */
static const int DISPLACEMENT_MASK = 0xFFF;

/**
 * @brief Equals to 0x7FF, equals to decimal 2047 in two's complement.
 */
static const int DISPLACEMENT_MAX = 0x7FF;

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
 * @brief A const variable that holds the amount of line increment.
 */
static const int LINE_INCREMENT = 5;

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
 * @brief A const variable that holds the manximum length of text record.
 */
static const int TEXT_RECORD_MAX_LEN = 55;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief                   Create a new modification record. It is appended on
 *                          the given modificationi records list.
 * @param[in] modif_records A list of modification records.
 * @param[in] modif_start   A start locctr of modification.
 */
static void assembler_create_modif_record(struct modif_record **modif_records,
                                          const int           modif_start);

/**
 * @brief          Read .asm file and create .obj and .lst files.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool assembler_execute_assemble(const char *cmd,
                                       const int  argc,
                                       const char *argv[]);

/**
 * @brief          Print symbol table created during assembly.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool assembler_execute_symbol(const char *cmd,
                                     const int  argc,
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
 * @brief                     Read lines from .asm file and .int file and
 *                            tokenize them. Skip empty or comment lines.
 *                            Tokens are written to .lst file.
 * @param[in] asm_file        A file pointer to an .asm file to be read.
 * @param[in] int_file        A file pointer to an .int file to be read.
 * @param[in] lst_file        A file pointer to an .lst file to be written.
 * @param[in] buffer          A line from .asm file.
 * @param[in] line            The current line number.
 * @param[in] locctr          A locctr of the current instruction.
 * @param[in] instruction_len A length of the current instruction.
 * @param[in] label           A label of the current line.
 * @param[in] mnemonic        A mnemonic of the current line.
 * @param[in] operands        A operands of the current line.
 */
static void assembler_pass2_get_ready_line(FILE *asm_file,
                                           FILE *int_file,
                                           FILE *lst_file,
                                           char *buffer,
                                           int  *line,
                                           int  *locctr,
                                           int  *instruction_len,
                                           char **label,
                                           char **mnemonic,
                                           char *(*operands)[]);

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
static void assembler_write_lst_line(FILE       *lst_file,
                                     const int  line,
                                     const int  locctr,
                                     const char *label,
                                     const char *mnemonic,
                                     const char *operand1,
                                     const char *operand2);

/**
 * @brief              Write a newline to .lst file.
 * @param[in] lst_file A file pointer to an .lst file to be written.
 */
static void assembler_write_lst_newline(FILE *lst_file);

/**
 * @brief                 Write an object code to .lst file.
 * @param[in] lst_file    A file pointer to an .lst file to be written.
 * @param[in] object_code An object code.
 */
static void assembler_write_lst_object_code(FILE       *lst_file,
                                            const char *object_code);

/**
 * @brief                  Write a end record to .obj file.
 * @param[in] obj_file     A file pointer to an .obj file to be written.
 * @param[in] start_locctr A start locctr of the program.
 */
static void assembler_write_obj_end(FILE      *obj_file,
                                    const int program_start);

/**
 * @brief                  Write a header record to .obj file.
 * @param[in] obj_file     A file pointer to an .obj file to be written.
 * @param[in] program_name A name of the program.
 * @param[in] start_locctr A start locctr of the program.
 * @param[in] program_len  A length of the program.
 */
static void assembler_write_obj_header(FILE       *obj_file,
                                       const char *program_name,
                                       const int  program_start,
                                       const int  program_len);
/**
 * @brief                   Write modificatoin records to .obj file.
 * @param[in] obj_file      A file pointer to an .obj file to be written.
 * @param[in] modif_records A list of modification records.
 */
static void assembler_write_obj_modif(FILE                      *obj_file,
                                      const struct modif_record *modif_records);


/**
 * @brief                       Write a text record to .obj file.
 * @param[in] obj_file          A file pointer to an .obj file to be written.
 * @param[in] text_record_start A start locctr of the text record.
 * @param[in] text_record       A text record to be written.
 */
static void assembler_write_obj_text(FILE       *obj_file,
                                     const int  text_record_start,
                                     const char *text_record);

void assembler_execute(const char *cmd,
                       const int  argc,
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

static void assembler_create_modif_record(struct modif_record **modif_records,
                                          const int           modif_start)
{
  struct modif_record *new_modif_record = malloc(sizeof(*new_modif_record));
  sprintf(new_modif_record->modif, "M%06X05", modif_start);
  new_modif_record->next = NULL;

  if(!*modif_records)
  {
    *modif_records = new_modif_record;
  }
  else
  {
    struct modif_record *walk = *modif_records;
    while(walk->next)
    {
      walk = walk->next;
    }
    walk->next = new_modif_record;
  }
}

static bool assembler_execute_assemble(const char *cmd,
                                       const int  argc,
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
  FILE *int_file = fopen(int_filename, "w+");
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
  fflush(int_file);
  rewind(asm_file);
  rewind(int_file);

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
  remove(int_filename);
  free(int_filename);
  free(lst_filename);
  free(obj_filename);

  symbol_save_table();

  return true;
}

static bool assembler_execute_symbol(const char *cmd,
                                     const int  argc,
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

  if('.' == buffer[0])
  {
    // This line is a comment.
    return false;
  }

  char *substr = strtok(buffer, " \t");
  if(!substr)
  {
    // This line is empty.
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
  int  program_start             = 0; // The start locctr of this program.
  int  line                      = 0; // Line is increased by 5.
  int  locctr                    = 0;
  int  instruction_len           = 0;
  char *label                    = NULL;
  char *mnemonic                 = NULL;
  char *operands[OPERANDS_COUNT];
  char buffer[BUFFER_LEN];

  // Read lines until meet the first non-empty and non-comment line.
  while(fgets(buffer, BUFFER_LEN, asm_file))
  {
    line += LINE_INCREMENT;
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

        fprintf(int_file, "%d\t%X\t%X\n", line, locctr, instruction_len);

        // Read lines until meet the first non-empty and non-comment line.
        while(fgets(buffer, BUFFER_LEN, asm_file))
        {
          line += LINE_INCREMENT;
          if(assembler_tokenize_line(buffer, &label, &mnemonic, &operands))
          {
            fprintf(int_file, "%d\t%X\t", line, locctr);
            break;
          }
        }
      }
      else
      {
        locctr = 0;

        fprintf(int_file, "%d\t%X\t%X\n", line, locctr, instruction_len);
      }

      break;
    }
  }
  // Now, buffer has the first non-comment line after the START line.
  program_start = locctr;

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
        instruction_len = 1;
      }
      else if(fabsf(2.0f - format) <= EPSILON)
      {
        instruction_len = 2;
      }
      else if(fabsf(3.5f - format) <= EPSILON)
      {
        instruction_len = 3;
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
        instruction_len = 4;
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
        instruction_len = strlen(operands[0]) - 3;
      }
      else if('X' == operands[0][0])
      {
        // Round up if length is odd.
        instruction_len = (strlen(operands[0]) - 3 + 1) / 2;
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

      instruction_len = 3;
    }
    else if(!strcmp("RESB", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      instruction_len = strtol(operands[0], NULL, DECIMAL);
    }
    else if(!strcmp("RESW", mnemonic))
    {
      if(!operands[0] || operands[1])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      instruction_len = 3 * strtol(operands[0], NULL, DECIMAL);
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

    fprintf(int_file, "%X\n", instruction_len);
    locctr += instruction_len;
    instruction_len = 0;

    do
    {
      if(!fgets(buffer, BUFFER_LEN, asm_file))
      {
        printf("assemble: END mnemonic is not found\n");
        return false;
      }
      line += LINE_INCREMENT;

      // Skip empty or comment lines.
    } while(!assembler_tokenize_line(buffer, &label, &mnemonic, &operands));

    fprintf(int_file, "%d\t%X\t", line, locctr);
  }

  *program_len = locctr - program_start;

  return true;
}

static bool assembler_pass2(FILE *asm_file,
                            FILE *int_file,
                            FILE *lst_file,
                            FILE *obj_file,
                            int  program_len)
{
  int                 program_start             = 0;
  int                 line                      = 0;
  int                 locctr                    = 0;
  int                 instruction_len           = 0;
  char                *label                    = NULL;
  char                *mnemonic                 = NULL;
  char                *operands[OPERANDS_COUNT];
  char                buffer[BUFFER_LEN];
  char                text_record[BUFFER_LEN];
  int                 text_record_start         = 0;
  bool                write_text_record         = false;
  struct modif_record *modif_records            = NULL;
  int                 base                      = 0;
  bool                is_base_relative_enabled  = false;

  // Read the first non-empty and non-comment line.
  assembler_pass2_get_ready_line(asm_file,
                                 int_file,
                                 lst_file,
                                 buffer,
                                 &line,
                                 &locctr,
                                 &instruction_len,
                                 &label,
                                 &mnemonic,
                                 &operands);

  // Write header record to .obj file.
  assembler_write_obj_header(obj_file, label, locctr, program_len);

  if(!strcmp("START", mnemonic))
  {
    assembler_write_lst_object_code(lst_file, NULL);

    assembler_pass2_get_ready_line(asm_file,
                                   int_file,
                                   lst_file,
                                   buffer,
                                   &line,
                                   &locctr,
                                   &instruction_len,
                                   &label,
                                   &mnemonic,
                                   &operands);
  }
  // Now, buffer has the first non-comment line after the START line.

  // Prepare text record that will be written to .obj file.
  memset(text_record, 0, sizeof(text_record));
  text_record_start = locctr;

  // Start assembly.
  while(strcmp("END", mnemonic))
  {
    char object_code[BUFFER_LEN];
    int  opcode                  = 0;
    int  n                       = 0;
    int  i                       = 0;
    int  x                       = 0;
    int  b                       = 0;
    int  p                       = 0;
    int  e                       = 0;
    int  displacement            = 0;
    int  address                 = 0;

    locctr += instruction_len; // Advance locctr points to the next instruciton.

    memset(object_code, 0, sizeof(object_code));
    if(!strcmp("BYTE", mnemonic))
    {
      if(!operands[0])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      if('C' == operands[0][0])
      {
        for(int i = 2; i < strlen(operands[0]) - 1; ++i)
        {
          // Store half-word for one byte.
          sprintf(&object_code[2 * (i - 2)], "%02X", operands[0][i]);
        }
      }
      else if('X' == operands[0][0])
      {
        strncpy(object_code, &operands[0][2], strlen(operands[0]) - 3);
      }
      else
      {
        symbol_set_error(INVALID_OPERAND, line, operands[0]);
        return false;
      }
    }
    else if(!strcmp("WORD", mnemonic))
    {
      if(!operands[0])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      sprintf(object_code, "%06X", (unsigned int)strtol(operands[0], NULL, DECIMAL));
    }
    else if(!strcmp("RESB", mnemonic) || !strcmp("RESW", mnemonic))
    {
      write_text_record = true;
    }
    else if(!strcmp("BASE", mnemonic))
    {
      if(!operands[0])
      {
        symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
        return false;
      }

      base = symbol_get_locctr(operands[0]);
      is_base_relative_enabled = true;
    }
    else if(!strcmp("NOBASE", mnemonic))
    {
      is_base_relative_enabled = false;
    }
    else
    {
      if(opcode_is_opcode(mnemonic))
      {
        // Nothing to do.
      }
      else if('+' == mnemonic[0] && opcode_is_opcode(&mnemonic[1]))
      {
        // Format 4.
        e = 1;
        ++mnemonic;
      }
      else
      {
        symbol_set_error(INVALID_OPCODE, line, mnemonic);
        return false;
      }

      opcode       = opcode_get_opcode(mnemonic);
      float format = opcode_get_format(mnemonic);
      if(fabsf(1.0f - format) <= EPSILON)
      {
        if(e)
        {
          --mnemonic;
          symbol_set_error(INVALID_OPCODE, line, mnemonic);
          return false;
        }

        sprintf(object_code, "%02X", opcode);
      }
      else if(fabsf(2.0f - format) <= EPSILON)
      {
        if(e)
        {
          --mnemonic;
          symbol_set_error(INVALID_OPCODE, line, mnemonic);
          return false;
        }

        if(!operands[0])
        {
          symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
          return false;
        }

        sprintf(object_code, "%02X", opcode);
        sprintf(&object_code[2], "%1X", symbol_get_locctr(operands[0]));
        sprintf(&object_code[3], "%1X", operands[1] ?
                                        symbol_get_locctr(operands[1]) :
                                        0);
      }
      else if(fabsf(3.5f - format) <= EPSILON)
      {
        if(!strcmp("RSUB", mnemonic))
        {
          // RSUB does not require any operand, uniquely.
          // Simple addressing.
          n = 1;
          i = 1;
        }
        else if(!operands[0])
        {
          // All format 3 or 4 instructions require at least one operand.
          symbol_set_error(REQUIRED_ONE_OPERAND, line, mnemonic);
          return false;
        }
        else
        {
          if('#' == operands[0][0])
          {
            // Immediate addressing.
            n = 0;
            i = 1;
            ++operands[0];
          }
          else if('@' == operands[0][0])
          {
            // Indirect addressing.
            n = 1;
            i = 0;
            ++operands[0];
          }
          else
          {
            // Simple addressing.
            n = 1;
            i = 1;
          }

          if(operands[1])
          {
            if(!strcmp("X", operands[1]))
            {
              // Indexed addressing.
              x = 1;
            }
            else
            {
              // Only register X can used for indexed addressing.
              symbol_set_error(INVALID_OPERAND, line, operands[0]);
              return false;
            }
          }
        }

        // Calculate displacement or address.
        if(!strcmp("RSUB", mnemonic))
        {
          // RSUB does not have any operand, which is
          // direct addressing in a sense.
          b = 0;
          p = 0;
          displacement = 0;
        }
        else if(n == 0 && i == 1 && !symbol_is_exist(operands[0]))
        {
          // Immediate addressing with value.
          // It is also direct addressing.
          b = 0;
          p = 0;
          if(e)
          {
            address = strtol(operands[0], NULL, DECIMAL);
          }
          else
          {
            displacement = strtol(operands[0], NULL, DECIMAL);
          }
        }
        else
        {
          int  target_address        = symbol_get_locctr(operands[0]);
          bool is_addressing_success = false;

          // Try PC-relative addressing first.
          displacement = target_address - locctr;
          if(DISPLACEMENT_MIN <= displacement &&
              DISPLACEMENT_MAX >= displacement)
          {
            b = 0;
            p = 1;
            is_addressing_success = true;
          }

          // Try BASE-relative addressing, if PC-relative addressing failed.
          if(!is_addressing_success)
          {
            if(!is_base_relative_enabled)
            {
              symbol_set_error(INVALID_OPERAND, line, operands[0]);
              return false;
            }

            displacement = target_address - base;
            if(BASE_MIN <= displacement &&
                BASE_MAX >= displacement)
            {
              b = 1;
              p = 0;
              is_addressing_success = true;
            }
          }

          // Try format 4, if BASE-relative addressing also failed.
          if(!is_addressing_success)
          {
            if(!e)
            {
              symbol_set_error(INVALID_OPERAND, line, operands[0]);
              return false;
            }

            // Direct addressing.
            b = 0;
            p = 0;
            address = target_address;
            is_addressing_success = true;

            if(!symbol_is_register(operands[0]))
            {
              // The operand is nor register neither value.
              // Mark its locctr in modification record for relocation.
              assembler_create_modif_record(&modif_records,
                                            locctr - instruction_len + 1);
            }
          }
        }

        sprintf(object_code, "%02X", opcode + (n << 1) + i);
        sprintf(&object_code[2], "%1X", (x << 3) + (b << 2) + (p << 1) + e);
        sprintf(&object_code[3],
                e ? "%05X" : "%03X",
                e ? address : displacement & DISPLACEMENT_MASK);
      }
      else
      {
        symbol_set_error(INVALID_OPCODE, line, mnemonic);
        return false;
      }
    }

    strcat(text_record, object_code);
    if(TEXT_RECORD_MAX_LEN <= strlen(text_record))
    {
      write_text_record = true;
    }

    // Write text record if there was a variable or text record is full.
    if(write_text_record)
    {
      if(strlen(text_record))
      {
        assembler_write_obj_text(obj_file, text_record_start, text_record);
        memset(text_record, 0, sizeof(text_record));
        text_record_start = locctr;
      }
      write_text_record = false;
    }

    assembler_write_lst_object_code(lst_file, object_code);

    assembler_pass2_get_ready_line(asm_file,
                                   int_file,
                                   lst_file,
                                   buffer,
                                   &line,
                                   &locctr,
                                   &instruction_len,
                                   &label,
                                   &mnemonic,
                                   &operands);
  }

  // Write trailing lines to .lst file.
  assembler_write_lst_newline(lst_file);

  // Write remaining text record, modification record, and end record
  // to .obj file.
  assembler_write_obj_text(obj_file, text_record_start, text_record);
  assembler_write_obj_modif(obj_file, modif_records);
  assembler_write_obj_end(obj_file, program_start);

  return true;
}

static void assembler_pass2_get_ready_line(FILE *asm_file,
                                           FILE *int_file,
                                           FILE *lst_file,
                                           char *buffer,
                                           int  *line,
                                           int  *locctr,
                                           int  *instruction_len,
                                           char **label,
                                           char **mnemonic,
                                           char *(*operands)[])
{
  while(fgets(buffer, BUFFER_LEN, asm_file))
  {
    if(assembler_tokenize_line(buffer, label, mnemonic, operands))
    {
      fscanf(int_file, "%d\t%X\t%X\n", line, locctr, instruction_len);
      assembler_write_lst_line(lst_file,
                               *line,
                               *locctr,
                               *label,
                               *mnemonic,
                               (*operands)[0],
                               (*operands)[1]);
      break;
    }
    else
    {
      *line += LINE_INCREMENT;
      assembler_write_lst_comment(lst_file, *line, buffer);
    }
  }
}

static void assembler_write_lst_comment(FILE       *lst_file,
                                        const int  line,
                                        const char *buffer)
{
  fprintf(lst_file, "%3d\t%3s\t%s\n", line, " ", buffer);
}

static void assembler_write_lst_line(FILE       *lst_file,
                                     const int  line,
                                     const int  locctr,
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
  fprintf(lst_file, "\t%s", operand1 ? : "");
  fprintf(lst_file, "%2s%s", operand2 ? ", " : " ", operand2 ? operand2 : "");

  // Add padding for columns alignment.
  int padding = 14;
  if(operand1)
  {
    padding -= strlen(operand1);
  }
  if(operand2)
  {
    padding -= strlen(operand2);
  }
  for(int i = 0; i < padding; ++i)
  {
    fprintf(lst_file, "%s", " ");
  }
}

static void assembler_write_lst_newline(FILE *lst_file)
{
  fprintf(lst_file, "\n");
}

static void assembler_write_lst_object_code(FILE       *lst_file,
                                            const char *object_code)
{
  fprintf(lst_file, "%-6s\n", object_code ? object_code : "");
}

static void assembler_write_obj_end(FILE      *obj_file,
                                    const int program_start)
{
  fprintf(obj_file, "E%06X\n", program_start);
}

static void assembler_write_obj_header(FILE       *obj_file,
                                       const char *program_name,
                                       const int  program_start,
                                       const int  program_len)
{
  fprintf(obj_file, "H%-6s%06X%06X\n", program_name ? program_name : " ",
                                       program_start,
                                       program_len);
}

static void assembler_write_obj_modif(FILE                      *obj_file,
                                      const struct modif_record *modif_records)
{
  const struct modif_record *walk = modif_records;
  while(walk)
  {
    fprintf(obj_file, "%s\n", walk->modif);
    walk = walk->next;
  }
}

static void assembler_write_obj_text(FILE       *obj_file,
                                     const int  text_record_start,
                                     const char *text_record)
{
  fprintf(obj_file, "T%06X%02X", text_record_start,
                                 (unsigned int)strlen(text_record) / 2);
  fputs(text_record, obj_file);
  fputs("\n", obj_file);
}
