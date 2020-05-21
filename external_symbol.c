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

void external_symbol_insert_control_section(const char *symbol,
                                            const int address,
                                            const int length)
{
  struct control_section *new_section = \
    malloc(sizeof(*new_section) +
           sizeof(char) * (strlen(symbol) + 1));
  new_section->next = NULL;
  new_section->symbols = NULL;
  new_section->address = address;
  new_section->length = length;
  strcpy(new_section->symbol, symbol);

  if(!_external_symbol_table)
  {
    _external_symbol_table = new_section;
  }
  else
  {
    struct control_section *walk = _external_symbol_table;
    while(walk->next)
    {
      walk = walk->next;
    }
    walk->next = new_section;
  }
}

void external_symbol_insert_symbol(const char *control_section,
                                   const char *symbol,
                                   const int address)
{
  struct external_symbol *new_symbol = \
    malloc(sizeof(*new_symbol) +
           sizeof(char) * (strlen(symbol) + 1));
  new_symbol->next = NULL;
  new_symbol->address = address;
  strcpy(new_symbol->symbol, symbol);

  struct control_section *section = _external_symbol_table;
  while(strcmp(control_section, section->symbol))
  {
    section = section->next;
  }

  if(!section->symbols)
  {
    section->symbols = new_symbol;
  }
  else
  {
    struct external_symbol *walk = section->symbols;
    while(walk->next)
    {
      walk = walk->next;
    }
    walk->next = new_symbol;
  }
}

void external_symbol_show_table(void)
{
  if(!_external_symbol_table)
  {
    return;
  }

  printf("Control\tSymbol\tAddress\tLength\n");
  printf("section\tname\n");
  printf("--------------------------------\n");

  int                    total_length = 0;
  struct control_section *section     = _external_symbol_table;
  while(section)
  {
    printf("%-6s\t%6s\t%2s%04X%s\t%s%04X\n",
        section->symbol,
        " ",
        " ", section->address, " ",
        " ", section->length);

    struct external_symbol *symbol = section->symbols;
    while(symbol)
    {
      printf("%-6s\t%6s\t%2s%04X\n",
          " ",
          symbol->symbol,
          " ", symbol->address);

      symbol = symbol->next;
    }

    total_length += section->length;
    section = section->next;
  }

  printf("--------------------------------\n");
  printf("%6s\t%3sTotal length %s%04X\n", " ", " ", " ", total_length);
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
