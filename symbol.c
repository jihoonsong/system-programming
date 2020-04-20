/**
 * @file  symbol.c
 * @brief A symbol table used when assembly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

/**
 * @def   SYMBOL_TABLE_LEN
 * @brief The length of symbol hash table. Equals to the number of alphabets.
 */
#define SYMBOL_TABLE_LEN 26

/**
 * @brief An enum of error, possibly occurs during assembly.
 */
enum symbol_error
{
  NONE,
};

/**
 * @brief Structure of symbol elements.
 */
struct symbol
{
  /** A pointer to the next symbol element. */
  struct symbol *next;
  /** A locctr value. */
  int           locctr;
  /** A symbol string. */
  char          symbol[];
};

/**
 * @brief A hash table of symbols, made during the last successful assembly.
 */
struct symbol *_saved_symbol_table[SYMBOL_TABLE_LEN] = {NULL,};

/**
 * @brief The last occured error during assembly.
 */
enum symbol_error _symbol_error = NONE;

/**
 * @brief A hash table of symbols, which is under construction.
 */
struct symbol *_working_symbol_table[SYMBOL_TABLE_LEN] = {NULL,};

void symbol_initialize(void)
{
  symbol_terminate();

  _symbol_error = NONE;
  memset(_saved_symbol_table, 0, SYMBOL_TABLE_LEN);
  memset(_working_symbol_table, 0, SYMBOL_TABLE_LEN);
}

void symbol_show_error_msg(void)
{
  switch(_symbol_error)
  {
    default:
      // Do nothing.
      break;
  }
}

void symbol_terminate(void)
{
  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _saved_symbol_table[i];
    while(walk)
    {
      struct symbol *del = walk;
      walk = walk->next;
      free(del);
    }

    walk = _working_symbol_table[i];
    while(walk)
    {
      struct symbol *del = walk;
      walk = walk->next;
      free(del);
    }
  }
}
