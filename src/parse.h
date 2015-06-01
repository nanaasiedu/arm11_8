#ifndef PARSE_H
#define PARSE_H

#include "generate.h"
#include "tokenise.h"
#include "helpers/symbolmap.h"
#include "helpers/table.h"
#include "helpers/intArray.h"
#include "helpers/definitions.h"
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

uint32_t generateOffsetField(TransferAddress address);
TransferAddress initEmptyAddress();
TransferAddress initImmediateAddress(int immediateAddress);
TransferAddress initRegisterAddress(int rn);
TransferAddress initOffsetRegisterAddress(int rn, int offset, IndexType indexType);
TransferAddress initShiftedRegisterAddress(int rn,
                                           int rm,
                                           int shift,
                                           IndexType indexType,
                                           ShiftType shiftType);

#endif /* end of include guard: PARSE_H */
