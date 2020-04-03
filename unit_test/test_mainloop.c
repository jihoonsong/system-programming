/**
 * @file  test_mainloop.c
 * @brief Test functions for mainloop.c.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../mainloop.c"

#include "test_mainloop.h"

/**
 * @def   INPUT_LEN
 * @brief The length of input.
 * @see   input
 */
#define INPUT_LEN 64

/**
 * @brief The input which will be used to test.
 */
static char _input[INPUT_LEN] = {0,};

/**
 * @brief The number of failed test cases.
 */
static int _fail_count = 0;

/**
 * @brief The number of passed test cases.
 */
static int _pass_count = 0;

/**
 * @brief Test cases.
 */
static struct command _test_commands[] = {(struct command){.cmd = "hi",
                                                           .argc = 0,
                                                           .argv = {NULL},
                                                           .handler = shell_execute},
                                          (struct command){.cmd = "he",
                                                           .argc = 0,
                                                           .argv = {NULL},
                                                           .handler = NULL},
                                          (struct command){.cmd = "du",
                                                           .argc = 2,
                                                           .argv = {"10", "20"},
                                                           .handler = memspace_execute},
                                          (struct command){.cmd = "fill",
                                                           .argc = 3,
                                                           .argv = {"10", "20", "30"},
                                                           .handler = memspace_execute},
                                          (struct command){.cmd = "edit",
                                                           .argc = 0,
                                                           .argv = {NULL},
                                                           .handler = memspace_execute},
                                          (struct command){.cmd = "opcode",
                                                           .argc = 0,
                                                           .argv = {NULL},
                                                           .handler = opcode_execute}};

/**
 * @brief The number of test cases.
 */
static int _test_commands_count = (int)(sizeof(_test_commands) /
                                        sizeof(_test_commands[0]));

/**
 * @brief             Test mainloop_assign_handler().
 * @param[in] command Test will be done on this.
 */
static void test_mainloop_assign_handler(const struct command *command);

/**
 * @brief             Generate test input.
 * @param[in] command The input will be generated upon this.
 */
static void test_mainloop_generate_input(const struct command *command);

/**
 * @brief             Test mainloop_tokenize_input().
 * @param[in] command Test will be done on this.
 */
static void test_mainloop_tokenize_input(const struct command *command);

void test_mainloop(void)
{
  for(int i = 0; i < _test_commands_count; ++i)
  {
    test_mainloop_generate_input(&_test_commands[i]);
    test_mainloop_tokenize_input(&_test_commands[i]);
    test_mainloop_assign_handler(&_test_commands[i]);
  }

  printf("\n");
  printf("Pass: %d\n", _pass_count);
  printf("Fail: %d\n", _fail_count);
}

static void test_mainloop_assign_handler(const struct command *command)
{
  _command.cmd = command->cmd;

  printf("Test mainloop_assign_handler() with '%s': ", _command.cmd);
  mainloop_assign_handler();

  if(command->handler == _command.handler)
  {
    ++_pass_count;
    printf("pass.\n");
  }
  else
  {
    ++_fail_count;
    printf("fail. it should be '%p', not '%p'.\n", (void *)command->handler, (void *)_command.handler);
  }
}

static void test_mainloop_generate_input(const struct command *command)
{
  strcpy(_input, command->cmd);
  strcat(_input, " ");
  if(0 < command->argc)
  {
    strcat(_input, command->argv[0]);
  }
  for(int i = 1; i < command->argc; ++i)
  {
    strcat(_input, ", ");
    strcat(_input, command->argv[i]);
  }
  strcat(_input, "\n");
}

static void test_mainloop_tokenize_input(const struct command *command)
{
  bool is_pass = true;

  printf("Test mainloop_tokenize_input() with '%s': ", _input);

  mainloop_tokenize_input(_input);

  if(strcmp(command->cmd, _command.cmd))
  {
    if(is_pass)
    {
      ++_fail_count;
      is_pass = false;
      printf("fail.\n");
    }
    printf("cmd should be '%s', not '%s'.\n", command->cmd, _command.cmd);
  }

  if(command->argc != _command.argc)
  {
    if(is_pass)
    {
      ++_fail_count;
      is_pass = false;
      printf("fail.\n");
    }
    printf("argc should be '%d', not '%d'.\n", command->argc, _command.argc);
  }

  for(int i = 0; i < _command.argc; ++i)
  {
    if(strcmp(command->argv[i], _command.argv[i]))
    {
      if(is_pass)
      {
        ++_fail_count;
        is_pass = false;
        printf("fail.\n");
      }
      printf("argv should be '%s', not '%s'.\n", command->argv[i], _command.argv[i]);
    }
  }
  for(int i = _command.argc; i < command->argc; ++i)
  {
    if(is_pass)
    {
      ++_fail_count;
      is_pass = false;
      printf("fail.\n");
    }
    printf("argv should include '%s'.\n", command->argv[i]);
  }

  if(is_pass)
  {
    ++_pass_count;
    printf("pass.\n");
  }
}
