#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include <stdint.h>

#include "tokenise.h"
#include "generate.h"
#include "helpers/table.h"
#include "parse.h"
#include "helpers/symbolmap.h"

//IO
void setUpIO(char *in, char *out);
void outputData(uint32_t i);

//Compile
void resolveLabelAddresses();

//Helper
void tokenise();
void dealloc();

#endif /* end of include guard: ASSEMBLE */
