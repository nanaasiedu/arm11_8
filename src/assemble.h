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
void setUpIO(char *in, char *out, Program *program);
void outputData(uint32_t i, Program *program);

//Compile
void resolveLabelAddresses(Program *program);

//Helper

void arm11Tokeniser(Program *program, char *token);

#endif /* end of include guard: ASSEMBLE */
