/**
 * @file  symbol.c
 * @brief A symbol table used during assembly.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

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
 * @brief Structure of register.
 */
struct reg
{
  /** A locctr value. */
  int  locctr;
  /** A symbol string. */
  char *symbol;
};

/**
 * @brief Structure of symbol error.
 */
struct _symbol_error
{
  enum symbol_error type;
  int               line;
  char              keyword[];
};

/**
 * @brief A const variable that holds the length of symbol hash table.
 *        Equals to the number of alphabets.
 */
static const int SYMBOL_TABLE_LEN = 26;

/**
 * @brief The last occured error during assembly.
 */
static struct _symbol_error *_error = NULL;

/**
 * @brief A hash table of registers.
 */
static struct reg _register_table[] = {(struct reg){.symbol = "A", .locctr = 0},
                                       (struct reg){.symbol = "X", .locctr = 1},
                                       (struct reg){.symbol = "L", .locctr = 2},
                                       (struct reg){.symbol = "PC", .locctr = 8},
                                       (struct reg){.symbol = "SW", .locctr = 9},
                                       (struct reg){.symbol = "B", .locctr = 3},
                                       (struct reg){.symbol = "S", .locctr = 4},
                                       (struct reg){.symbol = "T", .locctr = 5},
                                       (struct reg){.symbol = "F", .locctr = 6}};

/**
 * @brief A const variable that holds the length of register table.
 */
static const int REGISTER_TABLE_LEN = (int)(sizeof(_register_table) /
                                            sizeof(_register_table[0]));

/**
 * @brief A hash table of symbols, made during the last successful assembly.
 */
static struct symbol **_saved_symbol_table = NULL;

/**
 * @brief A hash table of symbols, which is under construction.
 */
static struct symbol **_working_symbol_table = NULL;

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

int symbol_get_locctr(const char *symbol)
{
  if(!symbol)
  {
    return -1;
  }

  for(int i = 0; i < REGISTER_TABLE_LEN; ++i)
  {
    if(!strcmp(symbol, _register_table[i].symbol))
    {
      return _register_table[i].locctr;
    }
  }

  for(int i = 0; i < SYMBOL_TABLE_LEN; ++i)
  {
    struct symbol *walk = _working_symbol_table[i];
    while(walk)
    {
      if(!strcmp(symbol, walk->symbol))
      {
        return walk->locctr;
      }

      walk = walk->next;
    }
  }

  return -1;
}

void symbol_initialize(void)
{
  symbol_terminate();

  _error                = NULL;
  _saved_symbol_table   = NULL;
  _working_symbol_table = NULL;
}

bool symbol_insert_symbol(const char *symbol, const int locctr)
{
  if(!_working_symbol_table)
  {
    printf("symbol: symbol table does not exist\n");
    return false;
  }

  if(symbol_is_exist(symbol))
  {
    printf("symbol: symbol '%s' already exists\n", symbol);
    return false;
  }

  struct symbol *new_symbol = malloc(sizeof(*new_symbol) +
                                     sizeof(char) * (strlen(symbol) + 1));
  new_symbol->next = NULL;
  new_symbol->locctr = locctr;
  strcpy(new_symbol->symbol, symbol);

  int key = symbol[0] - 'A';

  if(!_working_symbol_table[key])
  {
    _working_symbol_table[key] = new_symbol;
  }
  else
  {
    struct symbol *walk = _working_symbol_table[key];

    if(symbol_compare_string(walk->symbol, symbol) > 0)
    {
      _working_symbol_table[key] = new_symbol;
      new_symbol->next = walk;
    }
    else
    {
      struct symbol *prev = walk;
      walk = walk->next;
      while(walk && symbol_compare_string(walk->symbol, symbol) < 0)
      {
        prev = walk;
        walk = walk->next;
      }

      prev->next = new_symbol;
      new_symbol = walk;
    }
  }

  return true;
}

bool symbol_is_exist(const char *symbol)
{
  if(!_working_symbol_table)
  {
    printf("symbol: symbol table does not exist\n");
    return false;
  }

  for(int i = 0; i < REGISTER_TABLE_LEN; ++i)
  {
    if(!strcmp(symbol, _register_table[i].symbol))
    {
      return true;
    }
  }

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

bool symbol_is_register(const char *symbol)
{
  if(!symbol)
  {
    return false;
  }

  for(int i = 0; i < REGISTER_TABLE_LEN; ++i)
  {
    if(!strcmp(symbol, _register_table[i].symbol))
    {
      return true;
    }
  }

  return false;
}

void symbol_new_table(void)
{
  symbol_release_working_table();

  _working_symbol_table = malloc(SYMBOL_TABLE_LEN *
                                 sizeof(*_working_symbol_table));
  memset(_working_symbol_table,
         0,
         SYMBOL_TABLE_LEN * sizeof(*_working_symbol_table));
}

void symbol_save_table(void)
{
  symbol_release_saved_table();
  _saved_symbol_table = _working_symbol_table;
  _working_symbol_table = NULL;
}

void symbol_set_error(const enum symbol_error error,
                      const int line,
                      const char *keyword)
{
  if(_error)
  {
    free(_error);
  }

  _error = malloc(sizeof(*_error) + sizeof(char) * (strlen(keyword) + 1));
  _error->type = error;
  _error->line = line;
  strcpy(_error->keyword, keyword);
}

void symbol_show_error_msg(void)
{
  if(!_error)
  {
    return;
  }

  switch(_error->type)
  {
    case DUPLICATE_SYMBOL:
      printf("symbol: (line %d) symbol '%s' duplicate\n",
          _error->line,
          _error->keyword);
      break;
    case INVALID_OPCODE:
      printf("symbol: (line %d) opcode '%s' is invalid\n",
          _error->line,
          _error->keyword);
      break;
    case INVALID_OPERAND:
      printf("symbol: (line %d) operand '%s' is invalid\n",
          _error->line,
          _error->keyword);
      break;
    case REQUIRED_ONE_OPERAND:
      printf("symbol: (line %d) mnemonic '%s' requires one operand\n",
          _error->line,
          _error->keyword);
      break;
    case REQUIRED_TWO_OPERANDS:
      printf("symbol: (line %d) mnemonic '%s' requires two operands\n",
          _error->line,
          _error->keyword);
      break;
    default:
      // Do nothing.
      break;
  }
}

void symbol_show_table(void)
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
      printf("%s\t", walk->symbol);
      printf("%04X\n", walk->locctr);

      walk = walk->next;
    }
  }
}

void symbol_terminate(void)
{
  if(_error)
  {
    free(_error);
  }
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
