#include "assemble.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>



int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  Program program = (const Program){ 0 };

  //Setup
  Program *pProgram = malloc(sizeof(program));
  lblToAddr = malloc(sizeof(SymbolTable));
  map_init(lblToAddr);
  setUpIO(argv[1], argv[2], pProgram);

  tokenise(pProgram);
  resolveLabelAddresses(pProgram);
  parseProgram(lblToAddr, pProgram);

  //deallocation
  fclose(program.input);
  fclose(program.output);
  map_free(lblToAddr);
  free(pProgram);

  return EXIT_SUCCESS;
}

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

void resolveLabelAddresses(Program *program) {
  address currAddr = 0;
  Token *tokenPtr = (program->tokens).tokens;
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

void outputData(uint32_t i, Program *program) {
  //Gets each byte of i
  uint8_t b0,b1,b2,b3;
  b0 = (uint8_t) ( i        & 0xff);
  b1 = (uint8_t) ((i >> 8)  & 0xff);
  b2 = (uint8_t) ((i >> 16) & 0xff);
  b3 = (uint8_t) ((i >> 24) & 0xff);

  //Convert to little endian format
  instruction littleEndian_format = 0;
  littleEndian_format = (littleEndian_format | b0) << 8;
  littleEndian_format = (littleEndian_format | b1) << 8;
  littleEndian_format = (littleEndian_format | b2) << 8;
  littleEndian_format = (littleEndian_format | b3);

  // Print to file
  // printf("0x%.4x: 0x%.8x\n", addr, littleEndian_format);

  if (program->output != NULL) {
    fprintf(program->output, "%c%c%c%c", b0, b1, b2, b3);
  }
}

void tokenise(Program *program) {
  char line[512];
  while (fgets(line, sizeof(line), program->input) != NULL) {
    char *sep = " ,\n";
    char *token = strtok(line, sep);
    bool isBlankLine = token == NULL;
    while (token != NULL) {
      if (isLabel(token)) {
        char *pch = strstr(token, ":");
        strncpy(pch, "", 1);
        tokens_add(&program->tokens, token, LABEL);
      } else if (isLiteral(token)) {
        token++; // Remove #
        tokens_add(&program->tokens, token, LITERAL);
      } else if (isExpression(token)) {
        token++; // Remove =
        tokens_add(&program->tokens, token, EXPRESSION);
      } else {
        tokens_add(&program->tokens, token, OTHER);
      }
      token = strtok(NULL, sep);
    }
    if (!isBlankLine) {
      tokens_add(&program->tokens, "nl", NEWLINE);
    }
  }
  tokens_add(&program->tokens, "end", ENDFILE);
}
