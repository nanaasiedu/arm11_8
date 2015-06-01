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

extern char *shiftStrings[4];
extern int shiftInts[4];

//Set symbol tables
extern SymbolTable *lblToAddr;
extern SymbolTable mnemonicTable;
extern SymbolTable argumentTable;
extern SymbolTable registerTable;
extern SymbolTable shiftTable;

#endif /* end of include guard: TABLE_H */
