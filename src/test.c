#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

int getBinarySegment(int x, int start, int length);
int ipow(int x, int y);

int main(void) {
  printf ("binary"BYTETOBINARYPATTERN"\n", BYTETOBINARY(getBinarySegment(101,7,8)));
  printf("num: %d\n", getBinarySegment(101,7,8));
  printf ("binary"BYTETOBINARYPATTERN"\n", BYTETOBINARY(getBinarySegment(101,2,3)));
  printf("num: %d\n", getBinarySegment(101,2,3));
  return EXIT_SUCCESS;
}

int getBinarySegment(int x, int start, int length) {
  //PRE: sizeof(x) > start > 0 / length > 0
  //POST: res = int value of binary segment between start and end
  int acc = ipow(2,start); // an accumulator which will set the positions of the bits with the segment we want to return

  for (int i = 1; i < length; i++) {
    acc += ipow(2,start-i);
  }

  return (x & acc) >> (start - (length - 1));
}

int ipow(int x, int y) {
  // POST: returns x^y cast as an int
  return (int)pow(x,y);
}
