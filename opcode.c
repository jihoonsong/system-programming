/**
 * @file  opcode.c
 * @brief A handler of opcode related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Structure of opcode elements.
 */
struct opcode
{
  /** A pointer to the next log element. */
  struct opcode *next;
  /** An opcode value. */
  int           opcode;
  /** Type of format. */
  unsigned int  format1 : 1;
  unsigned int  format2 : 1;
  unsigned int  format3 : 1;
  unsigned int  format4 : 1;
  /** A mnemonic equivalent to the opcode. */
  char          mnemonic[];
};

/**
 * @brief Equals to 25.
 */
static int INSTRUCTION_LEN = 25;

/**
 * @brief Equals to 20.
 */
static int OPCODE_TABLE_LEN = 20;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief A hash table of opcodes.
 */
struct opcode *_opcode_table[OPCODE_TABLE_LEN] = {NULL,};

void opcode_execute(char *cmd, int argc, char *argv[])
{
  // TODO: to be implemented.
  printf("opcode_execute() is called\n");
}

void opcode_initialize(void)
{
  FILE *fp                          = NULL;
  char instruction[INSTRUCTION_LEN];
  char *opcode                      = NULL;
  char *mnemonic                    = NULL;
  char *format                      = NULL;

  fp = fopen("opcode.txt", "r");
  if(!fp)
  {
    printf("opcode: cannot find 'opcode.txt' file.\n");
    return;
  }

  while(fgets(instruction, INSTRUCTION_LEN, fp))
  {
    opcode = strtok(instruction, " \t\n");
    mnemonic = strtok(NULL, " \t\n");
    format = strtok(NULL, " \t\n");

    printf("%s %s %s\n", opcode, mnemonic, format);
  }

  fclose(fp);
}

void opcode_terminate(void)
{
  // TODO: to be implemented.
}
