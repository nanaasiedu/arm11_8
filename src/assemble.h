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

typedef enum {label,literal,other,newline,endfile} TokenType;

typedef struct {
  char *value;
  TokenType type
} Token;

//IO
void setUpIO(char *in, char *out);
//Compile
void resolveLabelAddresses(SymbolTable *map);
void parseProgram(SymbolTable *map);
//Helper
void tokenise();
void createMnemonicTable();

#endif /* end of include guard: ASSEMBLE */
