/**
 * @file  opcode.c
 * @brief A handler of opcode related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @def   OPCODE_TABLE_LEN
 * @brief The length of opcode hash table.
 */
#define OPCODE_TABLE_LEN 20

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
 * @brief Equals to 61.
 * @note  A prime number larger than OPCODE_TABLE_LEN;
 */
static int LCG_MODULUS = 61;

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
 * @brief            Insert new opcode object into hash table.
 * @param[in] opcode An opcode object to be inserted into table.
 */
static void opcode_insert_opcode(struct opcode *opcode);

void opcode_execute(char *cmd, int argc, char *argv[])
{
  // TODO: to be implemented.
  printf("opcode_execute() is called\n");
}

void opcode_initialize(void)
{
  srand((unsigned int)time(NULL)); // For universal hashing.

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
  int seed      = 0;
  int multipler = 0;
  int increment = 0;
  int divisor   = RAND_MAX / LCG_MODULUS; // To eliminate skewness.

  for(int i = 0; i < strlen(mnemonic); ++i)
  {
    seed += (int)mnemonic[i];
  }

  do
  {
    do
    {
      multipler = random() / divisor;
    } while(multipler > LCG_MODULUS);
  } while(!multipler);
  // multipler is a random integer in [1, LCG_MODULUS).

  do
  {
    increment = random() / divisor;
  } while(increment > LCG_MODULUS);
  // increment is a random integer in [0, LCG_MODULUS).

  return ((multipler * seed + increment) % LCG_MODULUS) % OPCODE_TABLE_LEN;
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
