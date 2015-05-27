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

char *registerStrings[16] = {
  "r0","r1","r2","r3","r4","r5","r6","r7",
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

int registerInts[16] = {
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

  if ((output = fopen(out, "wa")) == NULL) {
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
    do {
      tokenArray++;
    } while(tokenArray->type == NEWLINE);
    addr += WORD_SIZE;
  }
}

void parseLine(Token *token) {
  if (token->type == OTHER) {
    parseInstruction(token);
  }
}

void parseInstruction(Token *token) {
  switch(index_of(token->value, mnemonicStrings)) {
    case ADD: case SUB: case RSB: case AND: case EOR:
    case ORR: parseTurnaryDataProcessing(token); break;
    case MOV: case TST: case TEQ: case CMP: case MUL:
    case MLA: parseBinaryDataProcessing(token); break;
    case LDR: parseLdr(token); break;
    case STR: parseStr(token); break;
    case BEQ: case BNE: case BGE: case BLT: case BGT: case BLE:
    case B: parseB(token); break;
    case LSL: parseLsl(token); break;
    case ANDEQ: generateHaltOpcode(); break;
  }
}

//Parse Instructions
void parseTurnaryDataProcessing(Token *token) {
  Token *rd_token = token + 1;
  Token *rn_token = token + 2;
  Token *operand_token = token + 3;
  int rd,rn,operand;
  rd = map_get(&registerTable, rd_token->value);
  rn = map_get(&registerTable, rn_token->value);
  if(operand_token->type == LITERAL) {
    char *ptr;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    operand = map_get(&registerTable, operand_token->value);
  }
  generateDataProcessingOpcode(map_get(&mnemonicTable, token->value),rd,rn,operand,0);
}

void parseBinaryDataProcessing(Token *token) {
  Token *rdOrRn_token = token + 1;
  Token *operand_token = token + 2;
  int rdOrRn,operand;
  rdOrRn = map_get(&registerTable, rdOrRn_token->value);
  if(operand_token->type == LITERAL) {
    char *ptr;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    operand = map_get(&registerTable, operand_token->value);
  }
  if (strcmp(token->value,"mov") == 0) {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value),rdOrRn,0,operand,0);
  } else {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value),0,rdOrRn,operand,1);
  }
}

void parseMul(Token *token) {
  Token *rd_token = token + 1;
  Token *rm_token = token + 2;
  Token *rs_token = token + 3;
  int rd,rm,rs;
  rd = map_get(&registerTable, rd_token->value);
  rm = map_get(&registerTable, rm_token->value);
  rs = map_get(&registerTable, rs_token->value);
  generateMultiplyOpcode(map_get(&mnemonicTable, token->value),rd,rm,rs,0,0);
}

void parseMla(Token *token) {
  Token *rd_token = token + 1;
  Token *rm_token = token + 2;
  Token *rs_token = token + 3;
  Token *rn_token = token + 4;
  int rd,rm,rs,rn;
  rd = map_get(&registerTable, rd_token->value);
  rm = map_get(&registerTable, rm_token->value);
  rs = map_get(&registerTable, rs_token->value);
  rn = map_get(&registerTable, rn_token->value);
  generateMultiplyOpcode(map_get(&mnemonicTable, token->value),rd,rm,rs,rn,1);
}

void parseLdr(Token *token) {
  uint32_t cond, i, p, u, l, rd, rn;
  int offset;
  cond = 14;
  l = 1;
  Token *rdToken = token+1;
  Token *addrToken = token+2;
  rd = map_get(&registerTable, rdToken->value);
  //p, i, rn, offset, u
  if (addrToken->type == EXPRESSION) {
    p = 1;
    i = 0;
    rn = map_get(&registerTable, PC);
    char *ptr;
    uint32_t ex = (uint32_t) strtol(addrToken->value, &ptr, 0);
    offset = ex - addr;
    if (offset < 0) {
      offset *= -1;
      u = 0;
    } else {
      u = 1;
    }
    if (offset <= 0xFF) {
      generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rd, rn, offset, 0);
    } else {
      generateSingleDataTransferOpcode(cond, i, p, u, l, rd, rn, offset);
    }
  }
  else {
    i = 1;
    if ((token+3)->type == NEWLINE) {
      //Pre-Index, just base register
      p = 1;
      rn = map_get(&registerTable, stripBrackets(addrToken->value));
      offset = 0;
      u = 1;
    } else if (isPreIndex(addrToken->value)) {
      //Pre-Index, base and offset
      p = 1;
      rn = map_get(&registerTable, addrToken->value+1);
      char *ptr;
      offset = (int) strtol((token+3)->value, &ptr, 0);
      u = 1;
    } else {
      //Post-Index
    }
  }
}

void parseStr(Token *token) {

}

void parseB(Token *token) {
  uint8_t cond; int offset;
  Token *lblToken = token+1;
  cond = (uint8_t) map_get(&mnemonicTable, token->value);
  offset = map_get(lblToAddr, lblToken->value) - addr - ARM_OFFSET;
  generateBranchOpcode(cond, offset);
}

void parseLsl(Token *token) {

}

//Generators
void generateDataProcessingOpcode(int32_t opcode, int32_t rd, int32_t rn, int32_t operand, int32_t S) {
  int32_t instr = 14;
  instr = instr << 28;
  opcode = opcode << 21;
  instr |= opcode;
  S = S << 20;
  instr |= S;
  rn = rn << 16;
  instr |= rn;
  rd = rd << 12;
  instr |= rd;
  instr |= operand;
  outputData(instr);
}

void generateMultiplyOpcode(int32_t opcode, int32_t rd, int32_t rm, int32_t rs, int32_t rn, int32_t A) {
  int32_t instr = 14;
  instr = instr << 28;
  int32_t nonParameter;
  nonParameter = 0;
  nonParameter = nonParameter << 22;
  instr |= nonParameter;
  A = A << 21;
  instr |= A;
  nonParameter = 0;
  nonParameter = nonParameter << 20;
  instr |= nonParameter;
  rd = rd << 16;
  instr |= rn;
  rn = rn << 12;
  instr |= rd;
  rs = rs << 8;
  instr |= rs;
  nonParameter = 9;
  nonParameter = nonParameter << 4;
  instr |= nonParameter;
  instr |= rm;
  outputData(instr);
}

void generateBranchOpcode(int32_t cond, int32_t offset) {
  int32_t instr = cond;
  instr = instr << 28;
  int32_t nonParameter;
  nonParameter = 10;
  nonParameter = nonParameter << 24;
  instr |= nonParameter;
  instr |= offset;
  outputData(instr);
}

void generateHaltOpcode() {
  int32_t instr = 0;
  outputData(instr);
}

void outputData(uint32_t i) {
  printf("Output: %.8x\n", i);
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
  print_tokens(tokens);
}

int index_of(char *value, char **arr) {
  for (int i = 0; i < 23; i++) {
    if (strcmp(arr[i], value) == 0) {
       return i;
    }
  }
  return -1;
}

char* stripBrackets(char *str) {
  char *pch = strstr(str, "]");
  strncpy(pch, "\0", 1);
  return str+1;
}

bool isPreIndex(char *str) {
  return FALSE;
}

void dealloc() {
  fclose(input);
  fclose(output);
  map_free(lblToAddr);
  tokens_free(tokens);
}
