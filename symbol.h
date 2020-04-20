/**
 * @file  symbol.h
 * @brief A symbol table used when assembly.
 */

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

/**
 * @brief Initialize symbol table.
 */
void symbol_initialize(void);

/**
 * @brief Print error msg.
 */
void symbol_show_error_msg(void);

/**
 * @brief Release symbol table.
 */
void symbol_terminate(void);

#endif
