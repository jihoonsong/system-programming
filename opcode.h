/**
 * @file  opcode.h
 * @brief A handler of opcode related commands.
 */

#ifndef __OPCODE_H__
#define __OPCODE_H__

/**
 * @brief          Receives command and executes the command.
 * @param[in] cmd  A type of the command.
 * @param[in] argc The number of arguments.
 * @param[in] argv An list of arguments.
 */
void opcode_execute(char *cmd, int argc, char *argv[]);

#endif
