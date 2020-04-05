/**
 * @file  opcode.c
 * @brief A handler of opcode related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "logger.h"

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
 * @brief Equals to 16.
 */
static const int HEX = 16;

/**
 * @brief Equals to 25.
 */
static int OPCODE_LEN = 25;

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
 * @brief              Compute key for hash table based on the given seed.
 * @param[in] mnemonic A seed for single linear congruential generator.
 */
static int opcode_compute_key(char *mnemonic);

/**
 * @brief              Create opcode object
 * @param[in] opcode   Opcode.
 * @param[in] mnemonic Mnemonic.
 * @param[in] format   Format type.
 * @return             Created opcode object.
 */
static struct opcode * opcode_create_opcode(char *opcode,
                                            char *mnemonic,
                                            char *format);

/**
 * @brief Create opcode hash table using universal hasing.
 */
static void opcode_create_table(void);

/**
 * @brief          Print opcode of the given mnemonic.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
static bool opcode_execute_opcode(char *cmd, int argc, char *argv[]);

/**
 * @brief            Insert new opcode object into hash table.
 * @param[in] opcode An opcode object to be inserted into table.
 */
static void opcode_insert_opcode(struct opcode *opcode);

/*
 * @brief Initialize lcg constants.
 */
static void opcode_initialize_lcg(void);

void opcode_execute(char *cmd, int argc, char *argv[])
{
  if(!strcmp("opcode", cmd))
  {
    _is_command_executed = opcode_execute_opcode(cmd, argc, argv);
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

void opcode_initialize(void)
{
  srand((unsigned int)time(NULL)); // For universal hashing.

  opcode_initialize_lcg();
  opcode_create_table();
}

void opcode_terminate(void)
{
  for(int i = 0; i < OPCODE_TABLE_LEN; ++i)
  {
    struct opcode *walk = _opcode_table[i];
    while(walk)
    {
      struct opcode *del = walk;
      walk = walk->next;
      free(del);
    }
  }
}

static int opcode_compute_key(char *mnemonic)
{
  int seed = 0;

  for(int i = 0; i < strlen(mnemonic); ++i)
  {
    seed += (int)mnemonic[i];
  }

  int lcg = (_lcg.multipler * seed + _lcg.increment) % _lcg.modulus;
  return lcg % OPCODE_TABLE_LEN;
}

static struct opcode * opcode_create_opcode(char *opcode,
                                            char *mnemonic,
                                            char *format)
{
  struct opcode *new_opcode = malloc(sizeof(*new_opcode) +
                                     sizeof(char) * (strlen(mnemonic) + 1));
  memset(new_opcode, 0, sizeof(*new_opcode));
  new_opcode->next = NULL;
  new_opcode->opcode = strtol(opcode, NULL, HEX);
  for(int i = 0; i < format[i]; ++i)
  {
    int type = strtol(&format[i], NULL, HEX);
    switch(type)
    {
      case 1:
        new_opcode->format1 = 1;
        break;
      case 2:
        new_opcode->format2 = 1;
        break;
      case 3:
        new_opcode->format3 = 1;
        break;
      case 4:
        new_opcode->format4 = 1;
        break;
      default:
        // There is no format 0, so strtol() returns 0 means it failed.
        // In this case, this happens when it reads '/', the separator.
        // Do nothing.
        break;
    }
  }
  strcpy(new_opcode->mnemonic, mnemonic);

  return new_opcode;
}

static void opcode_create_table(void)
{
  FILE *fp                     = NULL;
  char instruction[OPCODE_LEN];
  char *opcode                 = NULL;
  char *mnemonic               = NULL;
  char *format                 = NULL;

  fp = fopen("opcode.txt", "r");
  if(!fp)
  {
    printf("opcode: cannot find 'opcode.txt' file.\n");
    return;
  }

  while(fgets(instruction, OPCODE_LEN, fp))
  {
    opcode = strtok(instruction, " \t\n");
    mnemonic = strtok(NULL, " \t\n");
    format = strtok(NULL, " \t\n");

    struct opcode *new_opcode = opcode_create_opcode(opcode, mnemonic, format);
    opcode_insert_opcode(new_opcode);
  }

  fclose(fp);
}

static bool opcode_execute_opcode(char *cmd, int argc, char *argv[])
{
  if(0 == argc)
  {
    printf("opcode: one argument is required\n");
    return false;
  }
  if(1 < argc)
  {
    printf("opcode: too many arguments\n");
    return false;
  }

  int key = opcode_compute_key(argv[0]);
  struct opcode *walk = _opcode_table[key];
  while(walk && strcmp(argv[0], walk->mnemonic))
  {
    walk = walk->next;
  }

  if(walk)
  {
    printf("opcode is %X\n", walk->opcode);
    return true;
  }
  else
  {
    printf("opcode: cannot find mnemonic %s\n", argv[0]);
    return false;
  }
}

static void opcode_insert_opcode(struct opcode *opcode)
{
  int key = opcode_compute_key(opcode->mnemonic);

  if(!_opcode_table[key])
  {
    _opcode_table[key] = opcode;
  }
  else
  {
    struct opcode *walk = _opcode_table[key];
    while(walk->next)
    {
      walk = walk->next;
    }
    walk->next = opcode;
  }
}

static void opcode_initialize_lcg(void)
{
  // Any prime number larger than OPCODE_TABLE_LEN is fine.
  _lcg.modulus = 61;

  int divisor = RAND_MAX / _lcg.modulus; // To eliminate skewness.
  do
  {
    do
    {
      _lcg.multipler = rand() / divisor;
    } while(_lcg.multipler > _lcg.modulus);
  } while(!_lcg.multipler);
  // _lcg.multipler is a random integer in [1, _lcg.modulus).

  do
  {
    _lcg.increment = rand() / divisor;
  } while(_lcg.increment > _lcg.modulus);
  // _lcg.increment is a random integer in [0, _lcg.modulus).
}
