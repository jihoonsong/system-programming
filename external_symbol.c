/**
 * @file  external_symbol.c
 * @brief A external symbol table used during linking and loading.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "external_symbol.h"

/**
 * @brief Structure of external symbol elements.
 */
struct external_symbol
{
  /** A pointer to the next external_symbol element. */
  struct external_symbol *next;
  /** An address value. */
  int                    address;
  /** A symbol string. */
  char                   symbol[];
};

/**
 * @brief Structure of control section elements.
 */
struct control_section
{
  /** A pointer to the next control_section element. */
  struct control_section *next;
  /** A list of symbols the control section has. */
  struct external_symbol *symbols;
  /** An address value. */
  int                    address;
  /** A length of control section. */
  int                    length;
  /** A symbol string. */
  char                   symbol[];
};

/**
 * @brief A hash table of symbols, made during the last successful assembly.
 */
static struct control_section *_external_symbol_table = NULL;

void external_symbol_initialize(void)
{
  external_symbol_terminate();

  _external_symbol_table = NULL;
}

void external_symbol_show_table(void)
{
  if(!_external_symbol_table)
  {
    return;
  }
}

void external_symbol_terminate(void)
{
  if(!_external_symbol_table)
  {
    return;
  }

  struct control_section *section = _external_symbol_table;
  while(section)
  {
    struct external_symbol *symbol = section->symbols;
    while(symbol)
    {
      struct external_symbol *del = symbol;
      symbol = symbol->next;
      free(del);
    }

    struct control_section *del = section;
    section = section->next;
    free(del);
  }
  _external_symbol_table = NULL;
}
