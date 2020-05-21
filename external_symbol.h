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
 * @brief             Insert new control section to external symbol table.
 * @param[in] symbol  A name of the control section.
 * @param[in] address A starting address of the control section.
 * @param[in] length  A length of the control section.
 */
void external_symbol_insert_control_section(const char *symbol,
                                            const int address,
                                            const int length);

/**
 * @brief Print the last successfully created external symbol table.
 */
void external_symbol_show_table(void);

/**
 * @brief Release external symbol table.
 */
void external_symbol_terminate(void);

#endif
