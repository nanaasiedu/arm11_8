#ifndef EMULATEDEFS
#define EMULATEDEFS

#include <stdlib.h>
#include <stdint.h>

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MEM16BIT
#define MEM16BIT 65536
#endif

// Instruction types
const int BRANCH = 128;
const int DATA_TRANS = 64;
const int MULT = 32;
const int DATA_PROC = 16;

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
  uint32_t *reg; // registers 0-12 are general purpose.
  uint32_t *SP;
  uint32_t *LR;
  uint32_t *PC;
  uint32_t *CPSR;
};

void execute(uint16_t cond);
void dealloc(void);
void enterC(void);
void loadFileToMem(char const *file);
void clearRegfile (struct regFile rf);
int32_t fetch(uint8_t *mem);
DecodedInst decode(int32_t instruction);
uint8_t getInstType(int32_t instruction);

// dont keep getFlags()
//***********************

void decodeForDataProc(int32_t instruction, DecodedInst di);
void decodeForMult(int32_t instruction, DecodedInst di);
void decodeForDataTrans(int32_t instruction, DecodedInst di);
void decodeForBranch(int32_t instruction, DecodedInst di);

#endif /* end of include guard: EMULATEDEFS */
