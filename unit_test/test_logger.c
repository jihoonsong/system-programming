/**
 * @file  test_logger.c
 * @brief Test functions for logger.c.
 */

#include <string.h>

#include "../logger.c"

#include "test_logger.h"

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
 * @brief The length of input.
 * @see   input
 */
#define INPUT_LEN 64

/**
 * @brief Structure of command elements and expected log.
 */
struct test_log
{
  char *cmd;
  int  argc;
  char *argv[ARGC_MAX + 1];
  char *command;
};

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
static struct test_log _test_logs[] = \
  {(struct test_log){.cmd = "h",
                     .argc = 0,
                     .argv = {NULL},
                     .command = "h"},
  (struct test_log){.cmd = "history",
                    .argc = 0,
                    .argv = {NULL},
                    .command = "history"},
  (struct test_log){.cmd = "du",
                    .argc = 2,
                    .argv = {"12", "3F", NULL},
                    .command = "du 12, 3F"},
  (struct test_log){.cmd = "edit",
                    .argc = 2,
                    .argv = {"23", "DD", NULL},
                    .command = "edit 23, DD"},
  (struct test_log){.cmd = "fill",
                    .argc = 3,
                    .argv = {"4", "37", "1D", NULL},
                    .command = "fill 4, 37, 1D"}};

/**
 * @brief The number of test cases.
 */
static int _test_log_count = (int)(sizeof(_test_logs) /
                                   sizeof(_test_logs[0]));

/**
 * @brief             Test logger_write_log().
 * @param[in] command Test will be done on this.
 */
static void test_logger_write_log(struct test_log *log);

void test_logger(void)
{
  printf("\nStart test logger.\n");


  logger_initialize(INPUT_LEN);

  for(int i = 0; i < _test_log_count; ++i)
  {
    test_logger_write_log(&_test_logs[i]);
  }

  printf("\n");
  printf("Pass: %d\n", _pass_count);
  printf("Fail: %d\n", _fail_count);
  printf("End test logger.\n");
}

static void test_logger_write_log(struct test_log *log)
{
  printf("Test test_logger_write_log() with '%s': ", log->command);

  logger_write_log(log->cmd, log->argc, log->argv);

  if(!strcmp(log->command, _log_tail->command))
  {
    ++_pass_count;
    printf("pass.\n");
  }
  else
  {
    ++_fail_count;
    printf("fail. it should be '%s', not '%s'.\n", log->command, _log_tail->command);
  }
}
