/**
 * @file  symbol.h
 * @brief A symbol table used when assembly.
 */

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

/**
 * @brief An enum of errors that can occur during assembly.
 */
enum symbol_error
{
  NONE,
  DUPLICATE_SYMBOL,
  INVALID_OPCODE,
  REQUIRED_ONE_OPERAND,
  REQUIRED_TWO_OPERANDS,
};

/**
 * @brief Initialize symbol table.
 */
void symbol_initialize(void);

/**
 * @brief            Insert the symbol if it is not duplicate.
 * @param[in] symbol A symbol to be inserted.
 * @param[in] locctr A locctr of the symbol.
 * @return           True on success, false otherwise.
 */
bool symbol_insert_symbol(const char *symbol, const int locctr);

/**
 * @brief            Check if the symbol exists in symbol table.
 * @param[in] symbol A symbol to be validated.
 * @return           True if exists, false otherwise.
 */
bool symbol_is_exist(const char *symbol);

/**
 * @brief Create new working symbol table. The new symbol table
 *        has registers as symbols initially.
 */
void symbol_new_table(void);

/**
 * @brief Save current symbol table. This function should be called
 *        when assembly is successfully done.
 */
void symbol_save_table(void);

/**
 * @brief           Set error flag
 * @param[in] error The error occured.
 */
void symbol_set_error(const enum symbol_error error,
                      const int line,
                      const char *keyword);

/**
 * @brief Print error msg.
 */
void symbol_show_error_msg(void);

/**
 * @brief Print the last successfully created symbol table.
 */
void symbol_show_table(void);

/**
 * @brief Release symbol table.
 */
void symbol_terminate(void);

#endif
