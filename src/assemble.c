#include "assemble.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

address addr = 0;

FILE *input = NULL, *output = NULL;
Tokens *tokens = NULL;

char *mnemonicStrings[23] = {
  "add","sub","rsb","and","eor","orr","mov","tst","teq","cmp",
  "mul","mla",
  "ldr","str",
  "beq","bne", "bge","blt","bgt","ble","b",
  "lsl","andeq"
};

char *mnemonicStrings[23] = {
  "add","sub","rsb","and","eor","orr","mov","tst","teq","cmp",
  "mul","mla",
  "ldr","str",
  "beq","bne", "bge","blt","bgt","ble","b",
  "lsl","andeq"
};

char *registerStrings[16] = {
  "r0","r1","r3","r4","r5","r6","r7",
  "r8","r9","r10","r11","r12","r13","r14","r15"
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

int registerInts[16] {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
}; 

SymbolTable *lblToAddr = NULL;
SymbolTable mnemonicTable = {23,23,mnemonicStrings,mnemonicInts};
SymbolTable argumentTable = {23,23,mnemonicStrings,numberOfArguments};
SymbolTable registerTable = {16,16,registerStrings,registerInts};

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  //Setup
  tokens = malloc(sizeof(Tokens));
  tokens_init(tokens);
  setUpIO(argv[1], argv[2]);

  lblToAddr = malloc(sizeof(SymbolTable));
  map_init(lblToAddr);
  tokenise();
  resolveLabelAddresses();
  parseProgram(lblToAddr);

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

void resolveLabelAddresses() {
  address currAddr = 0;
  for (size_t i = 0; i < tokens->size; i++) {
    Token token = tokens->tokens[i];
    switch (token.type) {
      case LABEL:
        map_set(lblToAddr, token.value, currAddr);
      break;
      case NEWLINE:
        currAddr += WORD_SIZE;
      break;
      default: break;
    }
  }
  map_print(lblToAddr);
}

void parseProgram(SymbolTable *map) {
  Token *tokenArray = tokens->tokens;
  while (tokenArray->type != ENDFILE) {
    parseLine(tokenArray);
    do {
      tokenArray++;
    } while(tokenArray->type != NEWLINE);
    tokenArray++;
    addr += WORD_SIZE;
  }
}

void parseLine(Token *token) {
  if (token->type == OTHER) {
    parseInstruction(token);
  }
}

void parseInstruction(Token *token) {
  switch(map_get(&mnemonicTable, token->value)) {
    case ADD: parseAdd(token); break;
    case SUB: parseSub(token); break;
    case RSB: parseRsb(token); break;
    case AND: parseAnd(token); break;
    case EOR: parseEor(token); break;
    case ORR: parseOrr(token); break;
    case MOV: parseMov(token); break;
    case TST: parseTst(token); break;
    case TEQ: parseTeq(token); break;
    case CMP: parseCmp(token); break;
    case MUL: parseMul(token); break;
    case MLA: parseMla(token); break;
    case LDR:
    case BEQ:
    case BNE:
    case BGE:
    case BLT:
    case BGT:
    case BLE:
    case B: parseB(token); break;
    case LSL: parseLsl(token); break;
    case ANDEQ: generateHaltOpcode(); break;
  }
}

//Parse Instructions
void parseTurnaryDataProcessing(Token *token) {
  token *rd_token = token + 1;
  token *rn_token = token + 2;
  token *operand_token = token + 3;
  int rd,rn,operand; 
  rd = map_get(registerTable, rd->value);
  rn = map_get(registerTable, rn->value);
  if(operand->type == LITERAL) {
    int numberRepresented;
    sscanf((operand->value),"%d",&numberRepresented);
    operand = numberRepresented;
  } else {
    operand = map_get(registerTable, operand->value);
  }
  generateDataProcessingOpcode();
}

void parseBinaryDataProcessing(Token *token) {
  token *rd_token = token + 1;
  token *operand_token = token + 2;
  int rd,operand; 
  rd = map_get(registerTable, rd->value);
  if(operand->value == LITERAL) {
    int numberRepresented;
    sscanf((operand->value),"%d",&numberRepresented);
    operand = numberRepresented;
  } else {
    operand = map_get(registerTable, operand->value);
  }
  generateDataProcessingOpcode();
}

void parseMul(Token *token) {
  token *rd_token = token + 1;
  token *rm_token = token + 2;
  token *rs_token = token + 3;
  int rd,rm,rs; 
  rd = map_get(registerTable, rd->value);
  rm = map_get(registerTable, rm->value);
  rs = map_get(registerTable, rs->value);
  generateMultiplyOpcode();
}

void parseMla(Token *token) {
  token *rd_token = token + 1;
  token *rm_token = token + 2;
  token *rs_token = token + 3;
  token *rn_token = token + 4;
  int rd,rm,rs,rn; 
  rd = map_get(registerTable, rd->value);
  rm = map_get(registerTable, rm->value);
  rs = map_get(registerTable, rs->value);
  rn = map_get(registerTable, rn->value);
  generateMultiplyOpcode();
}

void parseB(Token *token) {
  uint8_t cond; uint32_t offset;
  cond = (uint8_t) map_get(&mnemonicTable, token->value);
  offset = (uint32_t) addr - map_get(lblToAddr, token->value);
  generateBranchOpcode(cond, offset);
}
void parseLsl(Token *token) {

}

//Generators
void generateBranchOpcode(uint8_t cond, uint32_t offset) {

}

void generateHaltOpcode() {

}

#pragma mark - Helper Functions

void tokenise() {
  char line[512];
  while (fgets(line, sizeof(line), input) != NULL) {
    char *sep = " ,\n[]";
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
  tokens_print(tokens);
}

// int index_of(Token *token, char *arr) {
//   int i = 0;
//   while (strcmp(token->value,arr[i]) != 0) {
//     i++;
//   }
//   return i;
// }

void dealloc() {
  fclose(input);
  fclose(output);
  map_free(lblToAddr);
  tokens_free(tokens);
}
