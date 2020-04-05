/**
 * @file  opcode.c
 * @brief A handler of opcode related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def   OPCODE_TABLE_LEN
 * @brief The length of opcode hash table.
 */
#define OPCODE_TABLE_LEN 20

/**
 * @brief Structure of linear congruential generator integer constants.
 */
struct lcg
{
  /** 0<= increment < modulus. */
  int increment;
  /** 0 < modulus. */
  int modulus;
  /** 0 < multipler < modulus. */
  int multipler;
};

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
 * @brief LCG constants used to create opcode table.
 */
static struct lcg _lcg = {0,};

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

/**
 * @brief A hash table of opcodes.
 */
struct opcode *_opcode_table[OPCODE_TABLE_LEN] = {NULL,};

/**
 * @brief Create opcode hash table.
 */
static void opcode_create_table(void);

/**
 * @brief Initialize lcg constants.
 */
static void opcode_initialize_lcg(void);

void opcode_execute(char *cmd, int argc, char *argv[])
{
  // TODO: to be implemented.
  printf("opcode_execute() is called\n");
}

void opcode_initialize(void)
{
  opcode_initialize_lcg();
  opcode_create_table();
}

void opcode_terminate(void)
{
  // TODO: to be implemented.
}

static void opcode_create_table(void)
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

static void opcode_initialize_lcg(void)
{
  // Any number in [0, OPCODE_TABLE_LEN] relatively prime with modulus is fine.
  _lcg.increment = 19;

  // Any prime number larger than OPCODE_TABLE_LEN is fine.
  _lcg.modulus = 31;

  // multipler - 1 should be divisible by all prime factors in modulus.
  _lcg.multipler = 1;
}
