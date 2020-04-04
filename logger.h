/**
 * @file  logger.h
 * @brief A logger that logs executed commands.
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

/**
 * @brief                  Receives the length of input.
 * @param[in] input_length The length of input.
 */
void logger_initialize(const int input_len);

/**
 * @brief  View all logs.
 * @return The number of logs.
 */
int logger_view_log(void);

/**
 * @brief          Write log about the given command information.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void logger_write_log(char *cmd, int argc, char *argv[]);

#endif
