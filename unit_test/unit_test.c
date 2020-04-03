/**
 * @file  unit_test.c
 * @brief The starting point of unit test.
 */

#include <stdio.h>

#include "test_mainloop.h"
#include "test_memspace.h"
#include "test_opcode.h"
#include "test_shell.h"

int main(void)
{
  test_mainloop();
  test_memspace();
  test_opcode();
  test_shell();

  return 0;
}
