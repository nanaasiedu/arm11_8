#include "assemble.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


FILE *input = NULL, *output = NULL;
Tokens *tokens = NULL;
char *mnemonicStrings[23] = {
  "add","sub","rsb","and","eor","orr","mov","tst","teq","cmp",
  "mul","mla",
  "ldr","str",
  "beq","bne", "bge","blt","bgt","ble","b",
  "lsl","andeq"
};

int mnemonicInts[23] = {
  4,2,3,0,1,12,13,8,9,10,
  0,1,
  0,0,
  0,1,10,11,12,13,14,
  0,1
}; //for special cases not sure

int numberOfArguments[23] = {
  3,3,3,3,3,3,2,2,2,2,
  3,4,
  2,2,
  1,1,1,1,1,1,1,
  3,2
};

SymbolTable mnemonicTable = {23,23,mnemonicStrings,mnemonicInts};

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  //Setup
  tokens = malloc(sizeof(Tokens));
  tokens_init(tokens);
  setUpIO(argv[1], argv[2]);

  SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));
  map_init(lblToAddr);
  tokenise();
  resolveLabelAddresses(lblToAddr);
  //parseProgram(lblToAddr);

  map_free(lblToAddr);
  dealloc();

  return EXIT_SUCCESS;
}

#pragma mark - IO

void setUpIO(char *in, char *out) {
  if ((input = fopen(in, "r")) == NULL) {
    perror(in);
    exit(EXIT_FAILURE);
  }

  if ((output = fopen(out, "w")) == NULL) {
    perror(out);
    exit(EXIT_FAILURE);
  }
}

#pragma mark - Compile

void resolveLabelAddresses(SymbolTable *map) {
  address currAddr = 0;
  for (size_t i = 0; i < tokens->size; i++) {
    Token token = tokens->tokens[i];
    switch (token.type) {
      case LABEL:
        map_set(map, token.value, currAddr);
      break;
      case NEWLINE:
        currAddr += WORD_SIZE;
      break;
      default: break;
    }
  }
  map_print(map);
}

void parseProgram(SymbolTable *map) {
  int i = 0;
  Token *tokenArray = tokens->tokens;
  while (tokenArray[i].type != ENDFILE) {
    parseLine(tokenArray+i);
    i++;
  }
}

void parseLine(Token *token) {
  if (token->type == OTHER) {
    parseInstruction(token);
  }
}


void parseInstruction(Token *token) {
  switch(index_of(token)) {
    case ADD: parseAdd();
    break;
    case SUB: parseSub();
    break;
    case RSB: parseRsb();
    break;
    case AND: parseAnd();
    break;
    case EOR: parseEor();
    break;
    case ORR: parseOrr();
    break;
    case MOV: parseMov();
    break;
    case TST: parseTst();
    break;
    case TEQ: parseTeq();
    break;
    case CMP: parseCmp();
    break;
    case MUL: parseMul();
    break;
    case MLA: parseMla();
    break;
    case BEQ: parseBeq();
    break;
    case BNE: parseBne();
    break;
    case BGE: parseBge();
    break;
    case BLT: parseBlt();
    break;
    case BGT: parseBgt();
    break;
    case BLE: parseBle();
    break;
    case B: parseB();
    break;
    case LSL: parseLsl();
    break;
    case ANDEQ: parseAndeq();
    break;

  }

}

//Parse Instructions

void parseAdd() {

}
void parseSub() {

}
void parseRsb() {

}
void parseAnd() {

}
void parseEor() {

}
void parseOrr() {

}
void parseMov() {

}
void parseTst() {

}
void parseTeq() {

}
void parseCmp() {

}
void parseMul() {

}
void parseMla() {

}
void parseBeq() {

}
void parseBne() {

}
void parseBge() {

}
void parseBlt() {

}
void parseBgt() {

}
void parseBle() {

}
void parseB() {

}
void parseLsl() {

}
void parseAndeq() {

}

#pragma mark - Helper Functions

void tokenise() {
  char line[512];
  while (fgets(line, sizeof(line), input) != NULL) {
    char *sep = " ,\n";
    char *token;
    for (token = strtok(line, sep); token; token = strtok(NULL, sep)) {
      if (isLabel(token)) {
        char *pch = strstr(token, ":");
        strncpy(pch, "\0", 1);
        tokens_add(tokens, token, LABEL);
      }
      else if (isLiteral(token)) {
        token++;
        tokens_add(tokens, token, LITERAL);
      }
      else {
        tokens_add(tokens, token, OTHER);
      }
    }
    tokens_add(tokens, "nl", NEWLINE);
  }
  tokens_add(tokens, "end", ENDFILE);
  tokens_print(tokens);
}

int index_of(Token *token) {
  int i = 0;
  while (strcmp(token->value,mnemonicStrings[i]) != 0) {
    i++;
  }
  return i;
}

void dealloc() {
  fclose(input);
  fclose(output);
  //map_free(mnemonicTable);
  tokens_free(tokens);
}
