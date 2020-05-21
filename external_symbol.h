/**
 * @file  external_symbol.h
 * @brief A external symbol table used during linking and loading.
 */

#ifndef __EXTERNAL_SYMBOL_H__
#define __EXTERNAL_SYMBOL_H__

/**
 * @brief Initialize external symbol table.
 */
void external_symbol_initialize(void);

/**
 * @brief Print the last successfully created external symbol table.
 */
void external_symbol_show_table(void);

/**
 * @brief Release external symbol table.
 */
void external_symbol_terminate(void);

#endif
