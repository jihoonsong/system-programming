/**
 * @file  assembler.h
 * @brief A handler of assembler related commands.
 */

#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void assembler_execute(const char *cmd,
                       const int argc,
                       const char *argv[]);

#endif
