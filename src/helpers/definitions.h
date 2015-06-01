#ifndef DEFINITION_H
#define DEFINITION_H

#include "stdint.h"

#define WORD_SIZE 4
#define ARM_OFFSET 8
#define k_PC "r15"
#define NOT_FOUND -1
#define SET 1
#define NOT_SET 0
#define NOT_NEEDED NOT_SET
#define N_MNEMONICS 23

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE !TRUE
#endif

typedef int16_t address;
typedef uint32_t instruction;
typedef unsigned char byte;

// instruction types
#define BRANCH 128
#define DATA_TRANS 64
#define MULT 32
#define DATA_PROC 16
#define HALT 0

// condition types
#define EQ 0
#define NE 1
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14

// opcodes
#define OP_AND 0
#define OP_EOR 1
#define OP_SUB 2
#define OP_RSB 3
#define OP_ADD 4
#define OP_TST 8
#define OP_TEQ 9
#define OP_CMP 10
#define OP_ORR 12
#define OP_MOV 13

// shift types
#define SFT_LSL 0
#define SFT_LSR 1
#define SFT_ASR 2
#define SFT_ROR 3

#endif /* end of include guard: DEFINITION_H */
