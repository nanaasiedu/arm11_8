#include "parse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

address addr = 0;

IntArray *loadExprs = NULL;

void parseProgram(SymbolTable *map, Tokens *tokens) {
  loadExprs = malloc(sizeof(IntArray));
  Token *tokenPtr = tokens->tokens;
  init(loadExprs, programLength);
  while (tokenPtr->type == NEWLINE) {
    tokenPtr++;
  }
  while (tokenPtr->type != ENDFILE) {
    parseLine(tokenPtr);
    do {
      tokenPtr++;
    } while(tokenPtr->type != NEWLINE);
    do {
      tokenPtr++;
    } while(tokenPtr->type == NEWLINE);
    if (tokenPtr->type == OTHER) {
      addr += WORD_SIZE;
    }
  }
  //loaded variables
  instruction nextAddr = (instruction) dequeue(loadExprs);
  while (nextAddr != NOT_FOUND) {
    outputData(nextAddr);
    nextAddr = (instruction) dequeue(loadExprs);
  }
  free(loadExprs);
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
  //Get args in Token form
  Token *rd_token = token + 1;
  Token *rn_token = token + 2;
  Token *operand_token = token + 3;

  //Get numbers associated to args
  int rd,rn,operand,i;
  rd = map_get(&registerTable, rd_token->value);
  rn = map_get(&registerTable, rn_token->value);

  //Find if operand is an immediate value (i = 1)
  if(operand_token->type == LITERAL) {
    i = SET;
    char *ptr;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    i = NOT_SET;
    operand = map_get(&registerTable, operand_token->value);
  }

  generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rd, rn, operand, NOT_NEEDED, i);
}

void parseBinaryDataProcessing(Token *token) {
  //Get args in Token form
  Token *rdOrRn_token = token + 1;
  Token *operand_token = token + 2;

  //Get numbers associated to args
  int rdOrRn,operand,i;
  rdOrRn = map_get(&registerTable, rdOrRn_token->value);
  //Find if operand is an immediate value (i = 1)
  if(operand_token->type == LITERAL) {
    char *ptr;
    i = SET;
    operand = (int) strtol(operand_token->value, &ptr, 0);
  } else {
    i = NOT_SET;
    operand = map_get(&registerTable, operand_token->value);
  }

  if (strcmp(token->value,"mov") == 0) {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rdOrRn, NOT_NEEDED, operand, NOT_SET, i);
  } else {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), NOT_NEEDED, rdOrRn, operand, SET, i);
  }
}

void parseMul(Token *token) {
  //Get args in Token form
  Token *rd_token = token + 1;
  Token *rm_token = token + 2;
  Token *rs_token = token + 3;

  //Get numbers associated to args
  int rd,rm,rs;
  rd = map_get(&registerTable, rd_token->value);
  rm = map_get(&registerTable, rm_token->value);
  rs = map_get(&registerTable, rs_token->value);

  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, NOT_NEEDED, NOT_SET);
}

void parseMla(Token *token) {
  //Get args in Token form
  Token *rd_token = token + 1;
  Token *rm_token = token + 2;
  Token *rs_token = token + 3;
  Token *rn_token = token + 4;

  //Get numbers associated to args
  int rd,rm,rs,rn;
  rd = map_get(&registerTable, rd_token->value);
  rm = map_get(&registerTable, rm_token->value);
  rs = map_get(&registerTable, rs_token->value);
  rn = map_get(&registerTable, rn_token->value);

  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, rn, SET);
}

void parseSingleDataTransfer(Token *token) {
  uint32_t cond, i, p, u, l, rd, rn;
  int32_t offset;
  cond = 14;
  l = !strcmp(token->value, "ldr");
  Token *rdToken = token + 1;
  Token *addrToken = token + 2;
  rd = map_get(&registerTable, rdToken->value);
  //p, i, rn, offset, u
  if (addrToken->type == EXPRESSION) {
    p = 1;
    rn = map_get(&registerTable, k_PC);
    char *ptr;
    uint32_t ex = (uint32_t) strtol(addrToken->value, &ptr, 0);
    bool isMov = (ex <= 0xFF);
    offset = -addr;
    offset += isMov ? ex : programLength - ARM_OFFSET;

    if (offset < 0) {
      offset *= -1; // COULD BE WRONG
      u = 0;
    } else {
      u = 1;
    }

    // u = 1;

    if (isMov) {
      i = 1;
      generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rd, NOT_NEEDED, offset, NOT_SET, SET);
    } else {
      i = 0;
      enqueue(loadExprs, ex);
      programLength += WORD_SIZE;
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
      i = NOT_SET;
      p = SET;
      rn = map_get(&registerTable, addrValue+1);
      char *ptr;
      offset = strtol((token + 3)->value, &ptr, 0);
      if (offset < 0) {
        offset *= -1; // COULD BE WRONG
        u = 0;
      } else {
        u = 1;
      }
    } else {
      //Post-Index
      i = SET;
      p = NOT_SET;
      rn = map_get(&registerTable, stripLastBracket(addrValue+1));
      char *ptr;
      // stripLastBracket((token+3)->value);
      offset = map_get(&registerTable, (token+3)->value);
      if (offset == -1) {
        offset = (int) strtol((token+3)->value, &ptr, 0);
      }
      u = 1;
    }
    free(addrValue);
    generateSingleDataTransferOpcode(cond, i, p, u, l, rd, rn, offset);
  }
}

void parseB(Token *token) {
  //Get args in Token form
  Token *lblToken = token + 1;

  //Get numbers associated to args
  uint8_t cond; int offset;
  cond = (uint8_t) map_get(&mnemonicTable, token->value);
  offset = map_get(lblToAddr, lblToken->value) - addr - ARM_OFFSET;
  printf("offset: %.8x\n", map_get(lblToAddr, lblToken->value));
  generateBranchOpcode(cond, offset >> 2);
}

void parseLsl(Token *token) {
  Token *rn_token = token + 1;
  Token *operand_token = token + 2;
  int rn = map_get(&registerTable, rn_token->value);
  char *ptr;
  int operand =  (strtol(operand_token->value, &ptr, 0) << 7) & 0xF80;
  operand |= rn & 0xF;
  generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rn, NOT_NEEDED, operand, NOT_SET, NOT_SET);
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
