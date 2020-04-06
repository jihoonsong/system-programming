/**
 * @file  memspace.h
 * @brief A handler of memspace related commands.
 */

#ifndef __MEMSPACE_H__
#define __MEMSPACE_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void memspace_execute(const char *cmd, const int argc, const char *argv[]);

#endif
