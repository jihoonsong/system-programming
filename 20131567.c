/**
 * @file 20131567.c
 * @brief The starting point of this program.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @def   ARGC_MAX
 * @brief The maximum value of argc. According to the specificaiton,
 *        the number of argc can be 3 at most. This value is set one
 *        bigger than it in order to sense if there are arguments more
 *        than expected.
 * @see   argc
 */
#define ARGC_MAX 4

/**
 * @def   INPUT_LEN
 * @brief The length of input string. Its value can be modified
 *        if it is long enough to hold command and arguments.
 * @see   input
 */
#define INPUT_LEN 64

/**
 * @brief The number of arguments. Its maximum value is ARGC_MAX.
 * @see   ARGC_MAX
 * @see   argv
 */
static int argc = 0;

/**
 * @brief An list of arguments with trailing NULL.
 * @see   ARGC_MAX
 * @see   argc
 */
static char *argv[ARGC_MAX + 1] = {NULL,};

/**
 * @brief A pointer to an array of char that contains command.
 */
static char *cmd = NULL;

/**
 * @brief An array of char that holds received command and arguments.
 */
static char input[INPUT_LEN] = {0,};

/**
 * @brief Initialize the global states.
 */
void initialize(void);

/**
 * @brief The main loop of this program.
 * @details Receives command along with its arguments,
 *          check if the given command is valid,
 *          pass it to the designated handler if valid,
 *          and ignore it if invalid.
 */
void mainloop(void);

/**
 * @brief Release all allocated memory.
 */
void terminate(void);

/**
 * @brief Initialize states and start main loop.
 *        Clean up memory when the main loop is over.
 */
int main(void)
{
  initialize();
  mainloop();
  terminate();

  return 0;
}

void initialize(void)
{
  // TODO: to be implemented.
}

void mainloop(void)
{
  while(true)
  {
    printf("sicsim> ");
    if(fgets(input, INPUT_LEN, stdin))
    {
      printf("%s", input);
    }
  }
}

void terminate(void)
{
  // TODO: to be implemented.
}
