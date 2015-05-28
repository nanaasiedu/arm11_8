#ifndef ASSEMBLE
#define ASSEMBLE

#include <stdlib.h>
#include <stdint.h>
#include "symbolmap.h"
#include "tokenise.h"

#ifndef WORD_SIZE
#define WORD_SIZE 4
#endif

#ifndef ARM_OFFSET
#define ARM_OFFSET 8
#endif

#ifndef PC
#define PC "r15"
#endif

#ifndef NOT_FOUND
#define NOT_FOUND -1
#endif

#ifndef NOT_NEEDED
#define NOT_NEEDED 0
#endif

#ifndef S_NOT_SET
#define S_NOT_SET 0
#endif

#ifndef S_SET
#define S_SET 1
#endif

#ifndef A_NOT_SET
#define A_NOT_SET 0
#endif

#ifndef A_SET
#define A_SET 1
#endif

#ifndef I_SET
#define I_SET 1
#endif

typedef int16_t address;
typedef int32_t instruction;

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
void outputData(uint32_t i);
//Compile
void resolveLabelAddresses();
void parseProgram(SymbolTable *map);
void parseLine(Token *token);
void parseInstruction(Token *token);
//Parse Instructions
void parseTurnaryDataProcessing(Token *token);
void parseBinaryDataProcessing(Token *token);
void parseMul(Token *token);
void parseMla(Token *token);
void parseB(Token *token);
void parseLsl(Token *token);
void parseSingleDataTransfer(Token *token);
//Opcode Generators
void generateDataProcessingOpcode(int32_t opcode, int32_t rd, int32_t rn, int32_t operand, int32_t S, int32_t i);
void generateMultiplyOpcode(int32_t opcode, int32_t rd, int32_t rm, int32_t rs, int32_t rn, int32_t A);
void generateSingleDataTransferOpcode(uint32_t cond, uint32_t i, uint32_t p, uint32_t u, uint32_t l, uint32_t rd, uint32_t rn, uint32_t offset);
void generateBranchOpcode(int32_t cond, int32_t offset);
void generateHaltOpcode();
//Helper
void dealloc();
int index_of(char *value, char **arr);
char* stripBrackets(char *str);
char* stripLastBracket(char *str);
bool isPreIndex(char *str);
//Tokens
void tokenise();
int mnemonic_name(Token *token);

#endif /* end of include guard: ASSEMBLE */
