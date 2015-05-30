#ifndef TABLE_H
#define TABLE_H

#include "symbolmap.h"
#include <stdlib.h>

//Set arrays for symbol tables
extern const char **mnemonicStrings;

extern const char **registerStrings;

extern const char *mnemonicInts;

extern const char *numberOfArguments;

extern const char *registerInts;

//Set symbol tables
SymbolTable *lblToAddr = NULL;
const SymbolTable mnemonicTable = {23, 23, mnemonicStrings, mnemonicInts};
const SymbolTable argumentTable = {23, 23, mnemonicStrings, numberOfArguments};
const SymbolTable registerTable = {16, 16, registerStrings, registerInts};

#endif /* end of include guard: TABLE_H */
