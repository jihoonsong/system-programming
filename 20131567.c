/**
 * @file  20131567.c
 * @brief The starting point of this program.
 */

#include "mainloop.h"

/**
 * @brief Initialize states and start main loop.
 *        Clean up memory when the main loop is over.
 */
int main(void)
{
  mainloop_initialize();
  mainloop_launch();
  mainloop_terminate();

  return 0;
}
