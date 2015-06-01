#include <stdlib.h>
#include <stdint.h>
#include "bitUtils.h"

int getBit(uint32_t x, int pos) {
  //returns 1 bit value of the bit at position pos of x
  // e.g getBit(10010011, 0) = 1
  return (x >> pos) & 1;
}

uint32_t getBinarySeg(uint32_t x, uint32_t start, uint32_t length) {
  //PRE: sizeof(x) > start >= 0 / start >= length > 0
  //POST: returns uint value of the binary segment starting from start
  //with length as specified from the parameter
  //e.g. getBinarySeg(0xAF, 15, 4) = 0xA
  long mask = (1 << length) - 1;
  return (x >> (start-length+1)) & mask;
}

int rotr8(uint8_t x, int n) {
  // PRE: x is an unsigned 8 bit number (note x may be any type with 8 or more bits). n is the number x will be rotated by.
  // POST: rotr8 will return the 8 bit value of x rotated n spaces to the right
  uint8_t a = (x & ((1 << n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

int rotr32(uint32_t x, int n) {
  // PRE: x is an unsigned 32 bit number (note x may be any type with 32 or more bits). n is the number x will be rotated by.
  // POST: rotr32 will return the 32 bit value of x rotated n spaces to the right
  uint32_t a = (x & ((1 << n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

void setField(uint32_t *instr, int start, int end, uint32_t value) {
  uint32_t mask = 1 << ((end - start) - 1);
  if (value <= mask) {
    *instr |= (value & mask) << end;
  }
  else {
    perror("Value too large for field.");
    exit(EXIT_FAILURE);
  }
}
