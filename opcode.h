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
 * @brief              Get format of mnemonic.
 * @param[in] mnemonic A mnemonic to be examined.
 * @return             1.0 if format 1, 2.0 if format 2, 3.5 if format 3/4, and
 *                     0.0 if invalid mnemonic.
 */
float opcode_get_format(const char *mnemonic);

/**
 * @brief              Return opcode of the mnemonic.
 * @param[in] mnemonic A mnemonic to be searched.
 * @return             Opcode if corresponding opcode is found, -1 otherwise.
 */
int opcode_get_opcode(const char *mnemonic);

/**
 * @brief Create opcode hash table.
 */
void opcode_initialize(void);

/**
 * @brief              Check if the mnemonic is opcode.
 * @param[in] mnemonic A mnemonic to be validated.
 * @return             True if corresponding opcode is found, false otherwise.
 */
bool opcode_is_opcode(const char *mnemonic);

/**
 * @brief Release hash table.
 */
void opcode_terminate(void);

#endif
