#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include "symbolmap.h"
#include "tokenise.h"

#ifndef WORD_SIZE
#define WORD_SIZE 4
#endif

#ifndef NOT_FOUND
#define NOT_FOUND -1
#endif


//IO
void setUpIO(char *in, char *out);
//Compile
void resolveLabelAddresses(SymbolTable *map);
void parseProgram(SymbolTable *map);
//Helper
void createMnemonicTable();
void dealloc();
//Tokens
void tokenise();

#endif /* end of include guard: ASSEMBLE */
