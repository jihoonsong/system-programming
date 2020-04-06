/**
 * @file  logger.c
 * @brief A logger that logs executed commands.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void logger_terminate(void)
{
  struct log *walk = _log_head;

  while(walk)
  {
    struct log *del = walk;
    walk = walk->next;
    free(del);
  }
}

void logger_initialize(const int input_len)
{
  INPUT_LEN = input_len;
}

const int logger_view_log(void)
{
  int        count = 0;
  struct log *walk = _log_head;

  while(walk)
  {
    ++count;
    printf("%d\t", count);
    printf("%s\n", walk->command);

    walk = walk->next;
  }

  return count;
}

void logger_write_log(const char *cmd, const int argc, const char *argv[])
{
  char command[INPUT_LEN];

  strcpy(command, cmd);
  if(0 < argc)
  {
    strcat(command, " ");
    strcat(command, argv[0]);
    for(int i = 1; i < argc; ++i)
    {
      strcat(command, ", ");
      strcat(command, argv[i]);
    }
  }

  struct log *new_log = malloc(sizeof(*new_log) +
                               sizeof(char) * (strlen(command) + 1));
  new_log->next = NULL;
  strcpy(new_log->command, command);

  if(!_log_head)
  {
    _log_head = new_log;
  }
  else
  {
    _log_tail->next = new_log;
  }
  _log_tail = new_log;
}
