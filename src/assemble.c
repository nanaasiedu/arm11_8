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
};

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
  parseProgram(lblToAddr);

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
  switch(mnemonic_name(token)) {
    case ADD: parseAdd(token);
    break;
    case SUB: parseSub(token);
    break;
    case RSB: parseRsb(token);
    break;
    case AND: parseAnd(token);
    break;
    case EOR: parseEor(token);
    break;
    case ORR: parseOrr(token);
    break;
    case MOV: parseMov(token);
    break;
    case TST: parseTst(token);
    break;
    case TEQ: parseTeq(token);
    break;
    case CMP: parseCmp(token);
    break;
    case MUL: parseMul(token);
    break;
    case MLA: parseMla(token);
    break;
    case BEQ: parseBeq(token);
    break;
    case BNE: parseBne(token);
    break;
    case BGE: parseBge(token);
    break;
    case BLT: parseBlt(token);
    break;
    case BGT: parseBgt(token);
    break;
    case BLE: parseBle(token);
    break;
    case B: parseB(token);
    break;
    case LSL: parseLsl(token);
    break;
    case ANDEQ: parseAndeq(token);
    break;

  }

}

//Parse Instructions

void parseAdd(Token *token) {

}
void parseSub(Token *token) {

}
void parseRsb(Token *token) {

}
void parseAnd(Token *token) {

}
void parseEor(Token *token) {

}
void parseOrr(Token *token) {

}
void parseMov(Token *token) {

}
void parseTst(Token *token) {

}
void parseTeq(Token *token) {

}
void parseCmp(Token *token) {

}
void parseMul(Token *token) {

}
void parseMla(Token *token) {

}
void parseBeq(Token *token) {

}
void parseBne(Token *token) {

}
void parseBge(Token *token) {

}
void parseBlt(Token *token) {

}
void parseBgt(Token *token) {

}
void parseBle(Token *token) {

}
void parseB(Token *token) {

}
void parseLsl(Token *token) {

}
void parseAndeq(Token *token) {

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

int mnemonic_name(Token *token) {
  int i = 0;
  while (strcmp(token->value,mnemonicStrings[i]) != 0) {
    i++;
  }
  return i;
}

void dealloc() {
  fclose(input);
  fclose(output);
  // map_free(mnemonicTable);
  tokens_free(tokens);
}
