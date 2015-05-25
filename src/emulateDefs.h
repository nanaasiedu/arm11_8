#ifndef EMULATEDEFS
#define EMULATEDEFS

#include <stdlib.h>
#include <stdint.h>

const int BRANCH = 128;
const int DATA_TRANS = 64;
const int MULT = 32;
const int DATA_PROC = 16;

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct {
  uint8_t cond;
  uint8_t instType;
  uint8_t opcode;
  uint8_t rn;
  uint8_t rd;
  uint8_t rm;
  uint8_t rs;
  int32_t operandOffset;
} DecodedInst;

struct regFile {
  uint32_t reg[17]; // registers 0-12 are general purpose.
  uint32_t *SP;
  uint32_t *LR;
  uint32_t *PC;
  uint32_t *CPSR;
}

/* Helper functions for emulate */


#endif /* end of include guard: EMULATEDEFS */
