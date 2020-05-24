/**
 * @file  external_symbol.h
 * @brief A external symbol table used during linking and loading.
 */

#ifndef __EXTERNAL_SYMBOL_H__
#define __EXTERNAL_SYMBOL_H__

/**
 * @brief                     Return an address of the given external symbol.
 * @param[in] symbol          A name of symbol.
 * @return                    An address of the given external symbol.
 */
int external_symbol_get_address(const char *symbol);

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
 * @brief                     Insert new symbol to external symbol table.
 * @param[in] control_section A name of control section the symbol belongs to.
 * @param[in] symbol          A name of the symbol.
 * @param[in] address         A starting address of the symbol.
 */
void external_symbol_insert_symbol(const char *control_section,
                                   const char *symbol,
                                   const int address);

/**
 * @brief Print the last successfully created external symbol table.
 */
void external_symbol_show_table(void);

/**
 * @brief Release external symbol table.
 */
void external_symbol_terminate(void);

#endif
