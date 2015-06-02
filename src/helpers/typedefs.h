#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//intArray
typedef struct {
  int first;
  int last;
  int size;
  int fullCapacity;
  int *values;
} IntArray;

//emulate
typedef struct { // stores decoded instrustion in useful format
  uint8_t cond;
  uint8_t instType;
  uint8_t opcode;
  uint8_t rn;
  uint8_t rd;
  uint8_t rm;
  uint8_t rs;
  int32_t operandOffset;
} DecodedInst;

typedef struct {
  uint32_t *reg; // registers 0-12 are general purpose.
  uint32_t *SP;
  uint32_t *LR;
  uint32_t *PC;
  uint32_t *CPSR;
} RegFile;

//symbolmap
typedef struct {
  int fullCapacity;
  int size;
  char **keys;
  int *values;
} SymbolTable;

//parse
typedef enum {
    ADD,SUB,RSB,AND,EOR,ORR,MOV,TST,TEQ,CMP,
    MUL,MLA,
    LDR,STR,
    BEQ,BNE,BGE,BLT,BGT,BLE,B,
    LSL,ANDEQ
} Mnemonics;

typedef enum { Transfer_Post, Transfer_Pre, Transfer_None } IndexType;

typedef enum { Shift_LSL,  Shift_LSR,  Shift_ASR, Shift_ROR, Shift_None } ShiftType;

typedef struct {
  int imm;
  IndexType indexType;
  int rm;
  ShiftType shiftType;
  int shiftOffset;
} TransferAddress;

//tokenise
typedef enum {LABEL,LITERAL,EXPRESSION,OTHER,NEWLINE,ENDFILE} TokenType;

typedef struct {
  char *value;
  TokenType type;
} Token;

typedef struct {
  int size;
  int fullCapacity;
  Token *tokens;
} Tokens;

//definitions
typedef int bool;
typedef int16_t address;
typedef uint32_t instruction;
typedef unsigned char byte;
typedef struct {
  FILE *input;
  FILE *output;
  int length;
  address addr;
  Tokens *tokens;
  IntArray *loadExpr;
} Program;

#endif /* end of include guard: TYPEDEFS_H */
