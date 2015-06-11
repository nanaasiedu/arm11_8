#ifndef GENERATE_H
#define GENERATE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers/definitions.h"
#include "helpers/bitUtils.h"

//Generate Instructions from individual components
void generateDataProcessingInstruction(int32_t opcode, int32_t rd, int32_t rn, int32_t operand, int32_t S, int32_t i, Program *program);
void generateMultiplyInstruction(int32_t opcode, int32_t rd, int32_t rm, int32_t rs, int32_t rn, int32_t A, Program *program);
void generateSingleDataTransferInstruction(uint32_t i, uint32_t p, uint32_t u, uint32_t l, uint32_t rd, uint32_t rn, uint32_t offset, Program *program);
void generateBranchInstruction(int32_t cond, int32_t offset, Program *program);
void generateHaltInstruction(Program *program);

//Helper
uint32_t generateImmediate(uint32_t value);

extern void outputData(uint32_t i, Program *program);

#endif /* end of include guard: GENERATE_H */
