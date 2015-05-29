#ifndef TEST_H
#define TEST_H

void runAllTests(void);
void testingDataProc(void);
void testingDataTrans(void);
void testingExecute(void);
void testingExecuteBranch(void);
void testFetch(void);
void testGetInstType(void);
void testDecodeForDataProc(void);
void testDecodeForMult(void);
void testDecodeForDataTrans(void);
void testDecodeForBranch(void);

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

#endif /* end of include guard: TEST_H */
