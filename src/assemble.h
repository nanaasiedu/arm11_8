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
<<<<<<< HEAD
void createMnemonicTable();
void dealloc();
//Tokens
void tokenise();
=======
void tokenise();
int index_of(Token *token);
>>>>>>> af76d07516246369c890218e9f7c8ef6d3c4ceda

#endif /* end of include guard: ASSEMBLE */
