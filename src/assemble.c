#include "assemble.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>



int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  // Setup
  Program *pProgram = calloc(sizeof(Program), 1);
  pProgram->tokens = malloc(sizeof(Tokens));
  pProgram->loadExpr = queueInit();
  tokens_init(pProgram->tokens);
  lblToAddr = malloc(sizeof(SymbolTable));
  map_init(lblToAddr);
  setUpIO(argv[1], argv[2], pProgram);

  // Run Program
  char *sep = " ,\n";
  tokenise(pProgram, sep, arm11Tokeniser);
  resolveLabelAddresses(pProgram);
  parseProgram(lblToAddr, pProgram);

  // Deallocation
  fclose(pProgram->input);
  fclose(pProgram->output);
  queueFree(pProgram->loadExpr);
  tokens_free(pProgram->tokens);
  map_free(lblToAddr);
  free(pProgram);

  return EXIT_SUCCESS;
}

void arm11Tokeniser(Program *program, char *token) {
  if (isLabel(token)) {
    char *pch = strstr(token, ":");
    strncpy(pch, "", 1);
    tokens_add(program->tokens, token, LABEL);
  } else if (isLiteral(token)) {
    token++; // Remove #
    tokens_add(program->tokens, token, LITERAL);
  } else if (isExpression(token)) {
    token++; // Remove =
    tokens_add(program->tokens, token, EXPRESSION);
  } else {
    tokens_add(program->tokens, token, OTHER);
  }
}

void resolveLabelAddresses(Program *program) {
  address currAddr = 0;
  Token *tokenPtr = (program->tokens)->tokens;
  while (tokenPtr->type != ENDFILE) {
    if (tokenPtr->type == LABEL) {
      map_set(lblToAddr, tokenPtr->value, currAddr+WORD_SIZE);
    }
    do {
      tokenPtr++;
    } while(tokenPtr->type != NEWLINE); // Goes to end of line
    tokenPtr++; // Goes to next line
    if (tokenPtr->type == OTHER) {
      currAddr += WORD_SIZE;
    }
  }
  program->length = (int) currAddr;
}

// I/O Functions

void setUpIO(char *in, char *out, Program *program) {
  if ((program->input = fopen(in, "r")) == NULL) {
    perror(in);
    exit(EXIT_FAILURE);
  }

  if ((program->output = fopen(out, "wa")) == NULL) {
    perror(out);
    exit(EXIT_FAILURE);
  }
}

void outputData(instruction i, Program *program) {
  //Gets each byte of i
  unsigned char b0,b1,b2,b3 = 0;
  b0 = i         & 0xff;
  b1 = (i >> 8)  & 0xff;
  b2 = (i >> 16) & 0xff;
  b3 = (i >> 24) & 0xff;

  instruction lei = b0;
  lei = (lei << 8) | b1;
  lei = (lei << 8) | b2;
  lei = (lei << 8) | b3;

  // Print to file
  if (program->output != NULL) {
    fprintf(program->output, "%c%c%c%c", b0, b1, b2, b3);
  }

  printf("0x%.4x: 0x%.8x\n", program->addr, lei);

}
