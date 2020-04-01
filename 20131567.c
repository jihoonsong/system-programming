/**
 * @file 20131567.c
 * @brief The starting point of this program.
 */

#include <stdbool.h>
#include <stdio.h>

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
  }
}

void terminate(void)
{
  // TODO: to be implemented.
}
