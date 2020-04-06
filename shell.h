/**
 * @file  shell.h
 * @brief A handler of shell related commands.
 */

#ifndef __SHELL_H__
#define __SHELL_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void shell_execute(const char *cmd, const int argc, const char *argv[]);

#endif
