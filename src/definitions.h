#ifndef DEFINITION_H
#define DEFINITION_H

#define WORD_SIZE 4
#define ARM_OFFSET 8
#define PC "r15"
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
#define FALSE 0
#endif

typedef int16_t address;
typedef int32_t instruction;
typedef unsigned char byte;
#endif /* end of include guard: DEFINITION_H */
