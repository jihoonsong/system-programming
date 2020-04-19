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
void opcode_execute(const char *cmd, const int argc, const char *argv[]);

/**
 * @brief Create opcode hash table.
 */
void opcode_initialize(void);

/**
 * @brief              Check if the mnemonic is opcode.
 * @param[in] mnemonic A mnemonic to be validated.
 * @return             True if corresponding opcode is found, false otherwise.
 */
bool opcode_is_opcode(char *mnemonic);

/**
 * @brief Release hash table.
 */
void opcode_terminate(void);

#endif
