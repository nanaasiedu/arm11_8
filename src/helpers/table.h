#ifndef TABLE_H
#define TABLE_H

#include "symbolmap.h"
#include <stdlib.h>

//Set arrays for symbol tables
extern char *mnemonicStrings[23];

extern char *registerStrings[16];

extern int mnemonicInts[23];

extern int numberOfArguments[23];

extern int registerInts[16];

//Set symbol tables
SymbolTable *lblToAddr;
SymbolTable mnemonicTable;
SymbolTable argumentTable;
SymbolTable registerTable;

#endif /* end of include guard: TABLE_H */
