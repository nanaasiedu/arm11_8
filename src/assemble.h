#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include <stdint.h>
#include "symbolmap.h"
#include "tokenise.h"

#ifndef WORD_SIZE
#define WORD_SIZE 4
#endif

#ifndef NOT_FOUND
#define NOT_FOUND -1
#endif

typedef uint16_t address;
typedef uint32_t instruction;

#ifndef N_MNEMONICS
#define N_MNEMONICS 23
#endif

typedef enum {
    ADD,SUB,RSB,AND,EOR,ORR,MOV,TST,TEQ,CMP,
    MUL,MLA,
    LDR,STR,
    BEQ,BNE,BGE,BLT,BGT,BLE,B,
    LSL,ANDEQ
} Mnemonics;


//IO
void setUpIO(char *in, char *out);
//Compile
void resolveLabelAddresses(SymbolTable *map);
void parseProgram(SymbolTable *map);
void parseLine(Token *token);
void parseInstruction(Token *token);
//Parse Instructions
void parseAdd(Token *token);
void parseSub(Token *token);
void parseRsb(Token *token);
void parseAnd(Token *token);
void parseEor(Token *token);
void parseOrr(Token *token);
void parseMov(Token *token);
void parseTst(Token *token);
void parseTeq(Token *token);
void parseCmp(Token *token);
void parseMul(Token *token);
void parseMla(Token *token);
void parseBeq(Token *token);
void parseBne(Token *token);
void parseBge(Token *token);
void parseBlt(Token *token);
void parseBgt(Token *token);
void parseBle(Token *token);
void parseB(Token *token);
void parseLsl(Token *token);
void parseAndeq(Token *token);
//Helper
void dealloc();
//Tokens
void tokenise();
int mnemonic_name(Token *token);

#endif /* end of include guard: ASSEMBLE */
