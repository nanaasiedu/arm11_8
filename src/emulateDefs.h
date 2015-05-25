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

#define MEM16BIT 65536 // Number of memory locations in a 16 bit address

void cleanup(void);

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
