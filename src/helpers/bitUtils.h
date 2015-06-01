#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int getBit(uint32_t x, int pos);
uint32_t getBinarySeg(uint32_t x, uint32_t start, uint32_t length);
int rotr8(uint8_t x, int n);
int rotr32(uint32_t x, int n);
void setField(uint32_t *instr, int end, uint32_t value);

#endif /* end of include guard: UTILS_H */
