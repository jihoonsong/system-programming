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
 * @brief Release breakpoints.
 */
void debugger_terminate(void);

#endif
