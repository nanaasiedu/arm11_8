#include "generate.h"

void generateDataProcessingOpcode(int32_t opcode,
                                  int32_t rd,
                                  int32_t rn,
                                  int32_t operand,
                                  int32_t s,
                                  int32_t i) {
  //Cond set to 1110
  instruction instr = 0xe << 28;

  //Append all fields
  instr |= i      << 25;
  instr |= opcode << 21;
  instr |= s      << 20;
  instr |= rn     << 16;
  instr |= rd     << 12;

  //If immediate must calculate rotation
  if (i == SET && operand > 0xff) {
    int rotation = 32;
    int32_t imm = operand;
    while (imm % 4 == 0) {
      rotation--;
      imm = imm >> 2;
    } // TODO: Take another look at this
    instr |= (rotation & 0xf) << 8;
    instr |= imm & 0xff;
  } else {
    instr |= operand & 0xfff;
  }

  outputData(instr);
}

void generateMultiplyOpcode(int32_t opcode,
                            int32_t rd,
                            int32_t rm,
                            int32_t rs,
                            int32_t rn,
                            int32_t a) {
  //Cond set to 1110
  instruction instr = 0xe << 28; // COND define

  //Append all fields
  instr |= a     << 21;
  instr |= rd    << 16;
  instr |= rn    << 12;
  instr |= rs    << 8;
  //bits 7 to 4 = 1001;
  instr |= 9     << 4; // TODO: define
  instr |= rm;

  outputData(instr);
}

void generateBranchOpcode(int32_t cond, int32_t offset) {
  instruction instr = cond << 28;

  //Append all fields
  //bits 27 to 24 = 1010;
  instr |= 0xa << 24; // TODO: define
  instr |= offset & 0xffffff;

  outputData(instr);
}

void generateSingleDataTransferOpcode(uint32_t cond,
                                      uint32_t i,
                                      uint32_t p,
                                      uint32_t u,
                                      uint32_t l,
                                      uint32_t rd,
                                      uint32_t rn,
                                      uint32_t offset) {
  //Cond set to 1110
  instruction instr = 0x39 << 26;

  //Append all fields
  //bits 27,26 = 01
  // TODO: maybe look at making instructions
  instr |= i     << 25;
  instr |= p     << 24;
  instr |= u     << 23;
  instr |= l     << 20;
  instr |= rn    << 16;
  instr |= rd    << 12;
  instr |= offset & 0xFFF;
  outputData(instr);
}

void generateHaltOpcode() {
  int32_t instr = 0;
  outputData(instr);
}
