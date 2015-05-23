#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include "symbolmap.h"

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef WORD_SIZE
#define WORD_SIZE 4
#endif

//IO
void setUpIO(char *in, char *out);
void outputData(char *data);
//Compile
char* compile();
void firstPass(SymbolTable *map);
void secondPass(SymbolTable *map);
//Helper
bool hasLabel(char *str);
bool isBlankLine(char *str);

#endif /* end of include guard: ASSEMBLE */
