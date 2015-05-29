#include "assemble.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

address addr = 0;

FILE *input = NULL, *output = NULL;
Tokens *tokens = NULL;

char *mnemonicStrings[23] = {
  "add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp",
  "mul", "mla",
  "ldr", "str",
  "beq", "bne", "bge", "blt", "bgt", "ble", "b",
  "lsl", "andeq"
};

char *registerStrings[16] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

int mnemonicInts[23] = {
  4, 2, 3, 0, 1, 12, 13, 8, 9, 10,
  0, 1,
  0, 0,
  0, 1, 10, 11, 12, 13, 14,
  0, 1
};

int numberOfArguments[23] = {
  3, 3, 3, 3, 3, 3, 2, 2, 2,2 ,
  3, 4,
  2, 2,
  1, 1, 1, 1, 1, 1, 1,
  3, 2
};

int registerInts[16] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

SymbolTable *lblToAddr = NULL;
SymbolTable mnemonicTable = {23, 23, mnemonicStrings, mnemonicInts};
SymbolTable argumentTable = {23, 23, mnemonicStrings, numberOfArguments};
SymbolTable registerTable = {16, 16, registerStrings, registerInts};

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
        currAddr += WORD_SIZE;
      break;
      default: break;
    }
  }
  // map_print(lblToAddr);
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
    if (tokenArray->type == OTHER) {
      addr += WORD_SIZE;
    }
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
    case MOV: case TST: case TEQ:
    case CMP: parseBinaryDataProcessing(token); break;
    case MUL: parseMul(token); break;
    case MLA: parseMla(token); break;
    case LDR:
    case STR: parseSingleDataTransfer(token); break;
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
  int rd,rn,operand,i;
  rd = map_get(&registerTable, rd_token->value);
  rn = map_get(&registerTable, rn_token->value);
  if(operand_token->type == LITERAL) {
    i = 1;
    char *ptr;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    i = 0;
    operand = map_get(&registerTable, operand_token->value);
  }
  generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rd, rn, operand, NOT_NEEDED, i);
}

void parseBinaryDataProcessing(Token *token) {
  Token *rdOrRn_token = token + 1;
  Token *operand_token = token + 2;
  int rdOrRn,operand,i;
  rdOrRn = map_get(&registerTable, rdOrRn_token->value);
  if(operand_token->type == LITERAL) {
    char *ptr;
    i = 1;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    i = 0;
    operand = map_get(&registerTable, operand_token->value);
  }
  if (strcmp(token->value,"mov") == 0) {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rdOrRn, NOT_NEEDED, operand, NOT_SET, i);
  } else {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), NOT_NEEDED, rdOrRn, operand, SET, i);
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
  //set rn to 0 as rn not needed ; A is not set
  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, NOT_NEEDED, NOT_SET);
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
  // A is set
  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, rn, SET);
}

void parseSingleDataTransfer(Token *token) {
  uint32_t cond, i, p, u, l, rd, rn;
  int offset;
  cond = 14;
  if (strcmp(token->value, "ldr") == 0) {
    l = SET;
  }
  else {
    l = NOT_SET;
  }
  Token *rdToken = token + 1;
  Token *addrToken = token + 2;
  rd = map_get(&registerTable, rdToken->value);
  //p, i, rn, offset, u
  if (addrToken->type == EXPRESSION) {
    p = 1;
    i = 1;
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
      generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rd, NOT_NEEDED, offset, NOT_SET, SET);
    } else {
      generateSingleDataTransferOpcode(cond, i, p, u, l, rd, rn, offset);
    }
  }
  else {
    char *addrValue = malloc(100);
    strcpy(addrValue, addrToken->value);
    if ((token+3)->type == NEWLINE) {
      //Pre-Index, just base register
      p = SET;
      i = NOT_SET;
      rn = map_get(&registerTable, stripBrackets(addrValue));
      offset = NOT_SET;
      u = SET;
    } else if (isPreIndex(addrValue)) {
      //Pre-Index, base and offset
      i = SET;
      p = SET;
      rn = map_get(&registerTable, addrValue+1);
      char *ptr;
      offset = (int) strtol((token + 3)->value, &ptr, 0);
      u = SET;
    } else {
      //Post-Index
      i = NOT_SET;
      p = SET;
      rn = map_get(&registerTable, addrValue+1);
      char *ptr;
      stripLastBracket((token+3)->value);
      offset = (int) strtol((token+3)->value, &ptr, 0);
      u = 1;
    }
    free(addrValue);
    generateSingleDataTransferOpcode(cond, i, p, u, l, rd, rn, offset);
  }
}

void parseB(Token *token) {
  uint8_t cond; int offset;
  Token *lblToken = token + 1;
  cond = (uint8_t) map_get(&mnemonicTable, token->value);
  offset = map_get(lblToAddr, lblToken->value) - addr - ARM_OFFSET;
  printf("%d - %d - %d = %d\n",  map_get(lblToAddr, lblToken->value), addr, ARM_OFFSET, offset);
  generateBranchOpcode(cond, offset);
}

void parseLsl(Token *token) {

}

//Generators
void generateDataProcessingOpcode(int32_t opcode,
                                  int32_t rd,
                                  int32_t rn,
                                  int32_t operand,
                                  int32_t S,
                                  int32_t i) {
  //Instrustion withh all condition
  instruction instr = 14;
  //Append all fields
  instr  = instr  << 28;
  instr |= i      << 25;
  instr |= opcode << 21;
  instr |= S      << 20;
  instr |= rn     << 16;
  instr |= rd     << 12;

  //If immediate must calculate rotation
  if (i == 1 && operand > 0xFF) {
    int rotation = 32;
    int32_t imm = operand;
    while (imm % 4 == 0) {
      rotation--;
      imm = imm >> 2;
    }
    instr |= (rotation & 0xf) << 8;
    instr |= imm & 0xff;
  } else {
    instr |= operand &  0xfff;
  }
  outputData(instr);
}

void generateMultiplyOpcode(int32_t opcode,
                            int32_t rd,
                            int32_t rm,
                            int32_t rs,
                            int32_t rn,
                            int32_t A) {
  instruction instr = 14;
  instr = instr << 28;
  A = A << 21;
  instr |= A;
  rd = rd << 16;
  instr |= rn;
  rn = rn << 12;
  instr |= rd;
  rs = rs << 8;
  instr |= rs;
  //bits 7 to 4 = 1001;
  instr |= 9 << 4;
  instr |= rm;
  outputData(instr);
}

void generateBranchOpcode(int32_t cond, int32_t offset) {
  instruction instr = cond;
  instr = instr << 28;
  //bits 27 to 24 = 1010;
  instr |= 0xA << 24;
  instr |= offset & 0xFFFFFF;
  outputData(instr);
}

void generateSingleDataTransferOpcode(uint32_t cond,
                                      uint32_t i,
                                      uint32_t p,
                                      uint32_t u,
                                      uint32_t l,
                                      uint32_t rd,
                                      uint32_t rn,
                                      uint32_t offset) {

  uint32_t instr = 14;
  instr = instr << 28;
  //bits 27,26 = 01
  instr |= 1 << 26;
  i = i << 25;
  instr |= i;
  p = p << 24;
  instr |= p;
  u = u << 23;
  instr |= u;
  l = l << 20;
  instr |= l;
  rn = rn << 16;
  instr |= rn;
  rd = rd << 12;
  instr |= rd;
  instr |= offset;
  outputData(instr);
}

void generateHaltOpcode() {
  int32_t instr = 0;
  outputData(instr);
}

void outputData(uint32_t i) {
  uint8_t b0,b1,b2,b3;
  uint32_t littleEndian_format = 0;

  b0 = (uint8_t) (i  & 0xff);
  b1 = (uint8_t) ((i >> 8) & 0xff);
  b2 = (uint8_t) ((i >> 16) & 0xff);
  b3 = (uint8_t) ((i >> 24) & 0xff);

  littleEndian_format = (littleEndian_format | b0) << 8;
  littleEndian_format = (littleEndian_format | b1) << 8;
  littleEndian_format = (littleEndian_format | b2) << 8;
  littleEndian_format = (littleEndian_format | b3);

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
  // print_tokens(tokens);
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

void stripLastBracket(char *str) {
  str[strlen(str)-1] = '\0';
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
