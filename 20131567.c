/**
 * @file 20131567.c
 * @brief The starting point of this program.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief The length of input string. Its value can be modified
 *        if it is long enough to hold command and arguments.
 */
#define INPUT_LEN 64

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
