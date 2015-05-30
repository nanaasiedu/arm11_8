#ifndef GENERATE_H
#define GENERATE_H

#include <stdint.h>
#include <stdlib.h>
#include "definitions.h"

void generateDataProcessingOpcode(int32_t opcode, int32_t rd, int32_t rn, int32_t operand, int32_t S, int32_t i);
void generateMultiplyOpcode(int32_t opcode, int32_t rd, int32_t rm, int32_t rs, int32_t rn, int32_t A);
void generateSingleDataTransferOpcode(uint32_t cond, uint32_t i, uint32_t p, uint32_t u, uint32_t l, uint32_t rd, uint32_t rn, uint32_t offset);
void generateBranchOpcode(int32_t cond, int32_t offset);
void generateHaltOpcode();

extern void outputData(uint32_t i);

#endif /* end of include guard: GENERATE_H */
