#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include <stdint.h>

#include "tokenise.h"
#include "table.c"
#include "generate.h"
#include "parse.h"
#include "symbolmap.h"

//IO
void setUpIO(char *in, char *out);
void outputData(uint32_t i);

//Compile
void resolveLabelAddresses();

//Helper
void dealloc();

//Tokens
void tokenise();
int mnemonic_name(Token *token);

#endif /* end of include guard: ASSEMBLE */
