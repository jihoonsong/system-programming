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
 * @brief Release all allocated memories.
 */
void logger_terminate(void);

/**
 * @brief  View all logs.
 * @return The number of logs.
 */
const int logger_view_log(void);

/**
 * @brief          Write log about the given command information.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void logger_write_log(const char *cmd, const int argc, const char *argv[]);

#endif
