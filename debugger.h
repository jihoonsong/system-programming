/**
 * @file  debugger.h
 * @brief A handler of debugger related commands.
 */

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void debugger_execute(const char *cmd,
                      const int  argc,
                      const char *argv[]);

/**
 * @brief Initialize debugger.
 */
void debugger_initialize(void);

/**
 * @brief                     Set registers value and program length.
 * @param[in] program_address A starting address of loaded program.
 * @param[in] program_length  A length of loaded program.
 */
void debugger_prepare_run(const int program_address, const int program_length);

/**
 * @brief Release breakpoints.
 */
void debugger_terminate(void);

#endif
