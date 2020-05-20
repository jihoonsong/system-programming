/**
 * @file  loader.h
 * @brief A handler of loader related commands.
 */

#ifndef __LOADER_H__
#define __LOADER_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void loader_execute(const char *cmd,
                    const int argc,
                    const char *argv[]);

#endif
