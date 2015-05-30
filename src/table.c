#include "table.h"

const char *mnemonicStrings[23] = {
  "add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp",
  "mul", "mla",
  "ldr", "str",
  "beq", "bne", "bge", "blt", "bgt", "ble", "b",
  "lsl", "andeq"
};

const char *registerStrings[16] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

const char mnemonicInts[23] = {
  4, 2, 3, 0, 1, 12, 13, 8, 9, 10,
  0, 1,
  0, 0,
  0, 1, 10, 11, 12, 13, 14,
  0, 1
};

const char numberOfArguments[23] = {
  3, 3, 3, 3, 3, 3, 2, 2, 2,2 ,
  3, 4,
  2, 2,
  1, 1, 1, 1, 1, 1, 1,
  3, 2
};

const char registerInts[16] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};
