/**
 * @file  logger.c
 * @brief A logger that logs executed commands.
 */

#include <stdio.h>

/**
 * @brief An element of log list.
 */
struct log
{
  /** A pointer to the next log element. */
  struct log *next;
  /** A string of executed command. It has a compact length */
  char       command[];
};

/**
 * @brief A pointer to a head of log list.
 */
struct log *_log_head;

/**
 * @brief A pointer to a tail of log list.
 */
struct log *_log_tail;

/**
 * @brief A length of input. Used to set the length of temporal char
 *        array that stores command
 */
static int INPUT_LEN = 0;

void logger_initialize(const int input_len)
{
  INPUT_LEN = input_len;
}

void logger_write_log(char *cmd, int argc, char *argv[])
{
  // TODO: to be implemented.
  printf("logger_write_log() is called\n");
}
