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

/**
 * @brief  Return the starting address that the linked program will be loaded.
 * @return A prograddr.
 */
int memspace_get_progaddr(void);

/**
 * @brief      Return memory of the given number of bytes at the given address.
 * @param[out] A memory of the given number of bytes at the given address.
 * @param[in]  The address of memory to be obtained.
 * @param[in]  The number of bytes to obtain.
 * @return     A memory of the given number of bytes at the given address.
 */
unsigned char *memspace_get_memory(unsigned char *memory,
                                   const int address,
                                   const int byte_count);

/**
 * @brief     Set memory of the given number of bytes at the given address.
 * @param[in] The address of memory to be set.
 * @param[in] A memory to set.
 * @param[in] The number of bytes to set.
 */
void memspace_set_memory(const int address,
                         unsigned char *memory,
                         const int byte_count);

#endif
