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
void parseLdr(Token *token);
void parseStr(Token *token);
//Opcode Generators
<<<<<<< HEAD
void generateDataProcessingOpcode(int8_t opcode, int8_t rd, int8_t rn, int16_t operand, int8_t S);
void generateMultiplyOpcode(int8_t opcode, int8_t rd, int8_t rm, int8_t rs, int8_t rn, int8_t A);
void generateBranchOpcode(uint8_t cond, int offset);
void generateSingleDataTransferOpcode(uint32_t cond, uint32_t i, uint32_t p, uint32_t u, uint32_t l, uint32_t rd, uint32_t rn, uint32_t offset);
=======
void generateDataProcessingOpcode(int32_t opcode, int32_t rd, int32_t rn, int32_t operand, int32_t S);
void generateMultiplyOpcode(int32_t opcode, int32_t rd, int32_t rm, int32_t rs, int32_t rn, int32_t A);
void generateBranchOpcode(int32_t cond, int32_t offset);
>>>>>>> upstream/master
void generateHaltOpcode();
//Helper
void dealloc();
int index_of(char *value, char **arr);
char* stripBrackets(char *str);
bool isPreIndex(char *str);
//Tokens
void tokenise();
int mnemonic_name(Token *token);

#endif /* end of include guard: ASSEMBLE */
