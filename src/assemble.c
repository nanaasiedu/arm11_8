#include "assemble.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

FILE *input = NULL, *output = NULL;
Tokens *tokens = NULL;

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  //Setup
  tokens = malloc(sizeof(Tokens));
  tokens_init(tokens);
  lblToAddr = malloc(sizeof(SymbolTable));
  map_init(lblToAddr);
  setUpIO(argv[1], argv[2]);

  tokenise();
  resolveLabelAddresses();
  parseProgram(lblToAddr, tokens);

  dealloc();

  return EXIT_SUCCESS;
}

void setUpIO(char *in, char *out) {
  if ((input = fopen(in, "r")) == NULL) {
    perror(in);
    exit(EXIT_FAILURE);
  }

  if ((output = fopen(out, "wa")) == NULL) {
    perror(out);
    exit(EXIT_FAILURE);
  }
}

void resolveLabelAddresses() {
  address currAddr = 0;
  for (size_t i = 0; i < tokens->size; i++) {
    Token token = tokens->tokens[i];
    switch (token.type) {
      case LABEL:
        map_set(lblToAddr, token.value, currAddr);
      break;
      case NEWLINE:
        currAddr += WORD_SIZE; // TODO: Incorrect
      break;
      default: break;
    }
  }
}

void outputData(uint32_t i) {
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

  //Print to file
  printf("0x%.4x: 0x%.8x\n", addr, littleEndian_format);

  if (output != NULL) {
    fprintf(output, "%c%c%c%c", b0, b1, b2, b3);
  }
}


void tokenise() {
  char line[512];
  while (fgets(line, sizeof(line), input) != NULL) {
    char *sep = " ,\n";
    char *token;
    for (token = strtok(line, sep); token; token = strtok(NULL, sep)) {
      if (isLabel(token)) {
        char *pch = strstr(token, ":");
        strncpy(pch, "", 1);
        tokens_add(tokens, token, LABEL);
      }
      else if (isLiteral(token)) {
        token++;
        tokens_add(tokens, token, LITERAL);
      }
      else if (isExpression(token)) {
        token++;
        tokens_add(tokens, token, EXPRESSION);
      }
      else {
        tokens_add(tokens, token, OTHER);
      }
    }
    tokens_add(tokens, "nl", NEWLINE);
  }
  tokens_add(tokens, "end", ENDFILE);
}

int index_of(char *value, char **arr) {
  for (int i = 0; i < 23; i++) {
    if (strcmp(arr[i], value) == 0) {
       return i;
    }
  }
  return NOT_FOUND;
}

char* stripBrackets(char *str) {
  stripLastBracket(str);
  return ++str;
}

char* stripLastBracket(char *str) {
  str[strlen(str)-1] = '\0';
  return str;
}

bool isPreIndex(char *str) {
  return !(strchr(str, ']') == &str[strlen(str)-1]);
}

void dealloc() {
  fclose(input);
  fclose(output);
  map_free(lblToAddr);
  tokens_free(tokens);
}
