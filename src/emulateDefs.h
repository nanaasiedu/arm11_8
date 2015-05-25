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
  uint32_t *SP = &reg[13];
  uint32_t *LR = &reg[14];
  uint32_t *PC = &reg[15];
  uint32_t *CPSR = &reg[16];
}

// #ifndef WORD_SIZE
// #define WORD_SIZE 4
// #endif

/*
//IO
void setUpIO(char *in, char *out);
void outputData(char *data);
//Compile
char* compile();
void firstPass(SymbolTable *map);
void secondPass(SymbolTable *map);
//Helper
bool hasLabel(char *str);
bool isBlankLine(char *str);
*/

#endif /* end of include guard: EMULATEDEFS */
