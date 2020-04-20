/**
 * @file  symbol.c
 * @brief A symbol table used when assembly.
 */

#include <stdbool.h>
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
struct symbol **_saved_symbol_table = NULL;

/**
 * @brief The last occured error during assembly.
 */
enum symbol_error _symbol_error = NONE;

/**
 * @brief A hash table of symbols, which is under construction.
 */
struct symbol **_working_symbol_table = NULL;

/**
 * @brief          Compare two strings.
 * @param[in] str1 The first string to be compared.
 * @param[in] str2 The second string to be compared.
 * @return         Return negative if str1 < str2, positive if str1 > str2,
                   and 0 if str1 == str2.
 */
static int symbol_compare_string(const char *str1, const char *str2);

/**
 * @brief Release _saved_symbol_table.
 */
static void symbol_release_saved_table(void);

/**
 * @brief Release _working_symbol_table.
 */
static void symbol_release_working_table(void);

void symbol_initialize(void)
{
  symbol_terminate();

  _symbol_error         = NONE;
  _saved_symbol_table   = NULL;
  _working_symbol_table = NULL;
}

bool symbol_is_exist(const char *symbol)
{
  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _working_symbol_table[i];
    while(walk)
    {
      if(!strcmp(symbol, walk->symbol))
      {
        return true;
      }

      walk = walk->next;
    }
  }

  return false;
}

void symbol_new_table(void)
{
  symbol_release_working_table();

  _working_symbol_table = malloc(SYMBOL_TABLE_LEN *
                                 sizeof(*_working_symbol_table));
  memset(_working_symbol_table, 0, SYMBOL_TABLE_LEN);

  // TODO: Add registers.
}

void symbol_save_table(void)
{
  symbol_release_saved_table();
  _saved_symbol_table = _working_symbol_table;
  _working_symbol_table = NULL;
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

void symbol_show_table(void)
{
  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _saved_symbol_table[i];
    while(walk)
    {
      printf("%s\t", walk->symbol);
      printf("%4X\n", walk->locctr);

      walk = walk->next;
    }
  }
}

void symbol_terminate(void)
{
  symbol_release_saved_table();
  symbol_release_working_table();
}

static int symbol_compare_string(const char *str1, const char *str2)
{
  int str1_len = strlen(str1);
  int str2_len = strlen(str2);
  int min_len = str1_len < str2_len ? str1_len : str2_len;

  for(int i = 0; i < min_len; ++i)
  {
    if(str1[i] != str2[i])
    {
      return str1[i] - str2[i];
    }
  }

  return str1_len - str2_len;
}

static void symbol_release_saved_table(void)
{
  if(!_saved_symbol_table)
  {
    return;
  }

  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _saved_symbol_table[i];
    while(walk)
    {
      struct symbol *del = walk;
      walk = walk->next;
      free(del);
    }
  }
  free(_saved_symbol_table);
  _saved_symbol_table = NULL;
}

static void symbol_release_working_table(void)
{
  if(!_working_symbol_table)
  {
    return;
  }

  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _working_symbol_table[i];
    while(walk)
    {
      struct symbol *del = walk;
      walk = walk->next;
      free(del);
    }
  }
  free(_working_symbol_table);
  _working_symbol_table = NULL;
}
