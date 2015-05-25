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

#ifndef N_MNEMONICS
#define N_MNEMONICS 23
#endif

typedef enum {LABEL,LITERAL,OTHER,NEWLINE,ENDFILE} TokenType;

typedef enum {
    ADD,SUB,RSB,AND,EOR,ORR,MOV,TST,TEQ,CMP,
    MUL,MLA,
    LDR,STR,
    BEQ,BNE,BGE,BLT,BGT,BLE,B,
    LSL,ANDEQ
} Mnemonics;

typedef struct {
  char *value;
  TokenType type;
} Token;

//IO
void setUpIO(char *in, char *out);
//Compile
void resolveLabelAddresses(SymbolTable *map);
void parseProgram(SymbolTable *map);
void parseLine(Token *token);
void parseInstruction(Token *token);
//ParseInstructions
void parseAdd();
void parseSub();
void parseRsb();
void parseAnd();
void parseEor();
void parseOrr();
void parseMov();
void parseTst();
void parseTeq();
void parseCmp();
void parseMul();
void parseMla();
void parseBeq();
void parseBne();
void parseBge();
void parseBlt();
void parseBgt();
void parseBle();
void parseB();
void parseLsl();
void parseAndeq();
//Helper
void tokenise();
int index_of(Token *token);

#endif /* end of include guard: ASSEMBLE */
