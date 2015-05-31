#ifndef PARSE_H
#define PARSE_H

#include "generate.h"
#include "tokenise.h"
#include "helpers/symbolmap.h"
#include "helpers/definitions.h"
#include "helpers/table.h"
#include "helpers/intArray.h"
#include <stdint.h>

extern Tokens *tokens;

typedef enum {
    ADD,SUB,RSB,AND,EOR,ORR,MOV,TST,TEQ,CMP,
    MUL,MLA,
    LDR,STR,
    BEQ,BNE,BGE,BLT,BGT,BLE,B,
    LSL,ANDEQ
} Mnemonics;

extern address addr;
extern int programLength;

void parseProgram(SymbolTable *map, Tokens *tokens);
void parseLine(Token *token);
void parseInstruction(Token *token);
void parseTurnaryDataProcessing(Token *token);
void parseBinaryDataProcessing(Token *token);
void parseMul(Token *token);
void parseMla(Token *token);
void parseB(Token *token);
void parseLsl(Token *token);
void parseSingleDataTransfer(Token *token);

int index_of(char *value, char **arr);
char* stripBrackets(char *str);
char* stripLastBracket(char *str);
bool isPreIndex(char *str);

#endif /* end of include guard: PARSE_H */
