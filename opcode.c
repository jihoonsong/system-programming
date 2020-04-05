/**
 * @file  opcode.c
 * @brief A handler of opcode related commands.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * @brief Equals to 25.
 */
static int INSTRUCTION_LEN = 25;

/**
 * @brief A flag indicating whether command is executed or not.
 */
static bool _is_command_executed = false;

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
