#include "parse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t generateOffsetField(TransferAddress address) {
  switch (address.indexType) {
    case Transfer_None:
      return generateImmediate(address.rnImm);
    case Transfer_Pre:
      return generateImmediate(address.rnImm);
    case Transfer_Post:
      return generateImmediate(address.rnImm);
  }
}

TransferAddress initEmptyAddress() {
  TransferAddress output;
  output.indexType = Transfer_None;
  output.shiftType = Shift_None;
  output.rnImm = 0;
  output.rm = 0;
  output.shiftOffset = 0;
  return output;
}

TransferAddress initImmediateAddress(int immediateAddress) {
  TransferAddress output = initEmptyAddress();
  output.rnImm = immediateAddress;
  return output;
}

TransferAddress initRegisterAddress(int rn) {
  TransferAddress output = initEmptyAddress();
  output.indexType = Transfer_Pre;
  output.rnImm = rn;
  return output;
}

TransferAddress initOffsetRegisterAddress(int rn, int offset, IndexType indexType) {
  if (offset == 0) {
    return initRegisterAddress(rn);
  }

  TransferAddress output = initEmptyAddress();
  output.rnImm = rn;
  output.shiftOffset = offset;
  output.indexType = indexType;
  return output;
}

TransferAddress initShiftedRegisterAddress(int rn,
                                           int rm,
                                           int shift,
                                           IndexType indexType,
                                           ShiftType shiftType) {
  TransferAddress output = initEmptyAddress();
  output.rm = rm;
  output.rnImm = rn;
  output.shiftOffset = shift;
  output.indexType = indexType;
  output.shiftType = shiftType;
  return output;
}

void parseProgram(SymbolTable *map, Program *program) {
  //loadExprs = malloc(sizeof(IntArray));
  Token *tokenPtr = program->tokens->tokens;
  init(program->loadExpr, program->length);
  while (tokenPtr->type != ENDFILE) {
    parseLine(tokenPtr, program);
    do {
      tokenPtr++;
    } while(tokenPtr->type != NEWLINE);
    tokenPtr++;
    if (tokenPtr->type == OTHER) {
      program -> addr += WORD_SIZE;
    }
  }
  //loaded variables
  instruction nextAddr = (instruction) dequeue(program->loadExpr);
  while (nextAddr != NOT_FOUND) {
    outputData(nextAddr, program);
    nextAddr = (instruction) dequeue(program->loadExpr);
  }
  //free(loadExprs);
}

void parseLine(Token *token, Program *program) {
  if (token->type == OTHER) {
    parseInstruction(token, program);
  }
}

void parseInstruction(Token *token, Program *program) {
  switch(index_of(token->value, mnemonicStrings)) {
    case ADD: case SUB: case RSB: case AND: case EOR:
    case ORR: parseTurnaryDataProcessing(token, program); break;

    case MOV: case TST: case TEQ:
    case CMP: parseBinaryDataProcessing(token, program); break;

    case MUL: parseMul(token, program); break;

    case MLA: parseMla(token, program); break;

    case LDR:
    case STR: parseSingleDataTransfer(token, program); break;

    case BEQ: case BNE: case BGE: case BLT: case BGT: case BLE:
    case B: parseB(token, program); break;

    case LSL: parseLsl(token, program); break;

    case ANDEQ: generateHaltOpcode(program); break;
  }
}

//Parse Instructions
void parseTurnaryDataProcessing(Token *token, Program *program) {
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
    //Register
    i = NOT_SET;
    int rm = map_get(&registerTable, operand_token->value);
    operand = rm & 0xF;
    if ((operand_token + 1)->type != NEWLINE) {
      int shift;
      if ((operand_token + 2)->type == EXPRESSION) {
        char *ptr;
        shift = (int) strtol((operand_token + 2)->value, &ptr, 0);
        operand |= (shift & 0x1F) << 7;
      } else {
        shift = map_get(&registerTable, (operand_token + 2)->value);
        operand |= 1 << 4;
        //printf("shift: 0x%.4x\n", shift);
        operand |= (shift & 0xF) << 8;
      }
      int shift_type = map_get(&shiftTable, (operand_token + 1)->value);
      operand |= (shift_type & 3) << 5;
    }
  }

  generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rd, rn, operand, NOT_NEEDED, i, program);
}

void parseBinaryDataProcessing(Token *token, Program *program) {
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
    //Register
    i = NOT_SET;
    int rm = map_get(&registerTable, operand_token->value);
    operand = rm & 0xF;
    if ((operand_token + 1)->type != NEWLINE) {
      int shift;
      if ((operand_token + 2)->type == EXPRESSION) {
        char *ptr;
        shift = (int) strtol((operand_token + 2)->value, &ptr, 0);
        operand |= (shift & 0x1F) << 7;
      } else {
        shift = map_get(&registerTable, (operand_token + 2)->value);
        operand |= 1 << 4;
        //printf("shift: 0x%.4x\n", shift);
        operand |= (shift & 0xF) << 8;
      }
      int shift_type = map_get(&shiftTable, (operand_token + 1)->value);
      operand |= (shift_type & 3) << 5;
    }
  }

  if (strcmp(token->value,"mov") == 0) {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), rdOrRn, NOT_NEEDED, operand, NOT_SET, i, program);
  } else {
    generateDataProcessingOpcode(map_get(&mnemonicTable, token->value), NOT_NEEDED, rdOrRn, operand, SET, i, program);
  }
}

void parseMul(Token *token, Program *program) {
  //Get args in Token form
  Token *rd_token = token + 1;
  Token *rm_token = token + 2;
  Token *rs_token = token + 3;

  //Get numbers associated to args
  int rd,rm,rs;
  rd = map_get(&registerTable, rd_token->value);
  rm = map_get(&registerTable, rm_token->value);
  rs = map_get(&registerTable, rs_token->value);

  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, NOT_NEEDED, NOT_SET, program);
}

void parseMla(Token *token, Program *program) {
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

  generateMultiplyOpcode(map_get(&mnemonicTable, token->value), rd, rm, rs, rn, SET, program);
}

void parseSingleDataTransfer(Token *token, Program *program) {
  // Define fields
  uint32_t i, p, u, l, rd, rn;
  int32_t offset;

  l = !strcmp(token->value, "ldr");

  Token *rdToken = token + 1;
  Token *addrToken = token + 2;

  rd = map_get(&registerTable, rdToken->value);

  //p, i, rn, offset, u
  if (addrToken->type == EXPRESSION) {
    p = 1;
    rn = map_get(&registerTable, k_PC);
    //Get Expression
    char *ptr;
    uint32_t ex = (uint32_t) strtol(addrToken->value, &ptr, 0);

    bool isMov = (ex <= 0xFF);
    program->length += isMov ? 0 : WORD_SIZE;
    offset = isMov ? ex : program->length - program->addr - ARM_OFFSET;

    u = !isMov;

    if (offset < 0) {
      offset *= -1;
    }

    if (isMov) {
      i = 1;
      generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rd, NOT_NEEDED, offset, NOT_SET, SET, program);
    } else {
      i = 0;
      enqueue(program->loadExpr, ex);
      generateSingleDataTransferOpcode(i, p, u, l, rd, rn, offset, program);
    }
  } else {
    // char *addrValue = malloc(100);
    // strcpy(addrValue, addrToken->value);
    Token *offsetToken = (addrToken + 1);
    if (offsetToken->type == NEWLINE) { //offset = 0
      //Pre-Index, just base register
      p = SET;
      i = NOT_SET;
      u = SET;
      offset = NOT_SET;
      rn = map_get(&registerTable, stripBrackets(addrToken->value));
    }
    else if (isPreIndex(addrToken->value)) {
      //Pre-Index, base and offset
      p = SET;
      rn = map_get(&registerTable, addrToken->value + 1);
      if (offsetToken->type == LITERAL) {
        i = NOT_SET;
        char *ptr;
        offset = strtol(offsetToken->value, &ptr, 0);
        if (offset < 0) {
          offset *= -1;
          u = 0;
        } else {
          u = 1;
        }
      }
      else { //offset is a register, may need to shift
        int rm = map_get(&registerTable, offsetToken->value);
        offset = rm & 0xF;
        Token *shiftTypeToken = (offsetToken + 1);
        if (shiftTypeToken->type != NEWLINE) { // Shift Needed
          int shift;
          Token *shiftToken = (shiftTypeToken + 1);
          if (shiftToken->type == LITERAL) {
            char *ptr;
            shift = (int) strtol(shiftToken->value, &ptr, 0);
            offset |= (shift & 0x1F) << 7;
          } else { // Shift is a register
            shift = map_get(&registerTable, shiftToken->value);
            offset |= SET << 4;
            //printf("shift: 0x%.4x\n", shift);
            offset |= (shift & 0xF) << 8;
          }
          int shift_type = map_get(&shiftTable, shiftTypeToken->value);
          offset |= (shift_type & 3) << 5;
        }
        i = SET;
        u = SET; // TODO: change
      }
    }
    else {
      //Post-Index
      i = SET;
      p = NOT_SET;
      rn = map_get(&registerTable, stripLastBracket(addrToken->value+1));
      char *ptr;
      // stripLastBracket((token+3)->value);
      offset = map_get(&registerTable, (token+3)->value);
      if (offset == NOT_FOUND) {
        i = NOT_SET;
        offset = strtol((token+3)->value, &ptr, 0);
      }
      u = SET; // TODO: change
    }
    generateSingleDataTransferOpcode(i, p, u, l, rd, rn, offset, program);
  }
}

void parseB(Token *token, Program *program) {
  //Get args in Token form
  Token *lblToken = token + 1;

  //Get numbers associated to args
  uint8_t cond; int offset;
  cond = (uint8_t) map_get(&mnemonicTable, token->value);
  offset = map_get(lblToAddr, lblToken->value) - program->addr - ARM_OFFSET;
  generateBranchOpcode(cond, offset >> 2, program);
}

void parseLsl(Token *token, Program *program) {
  Token *rn_token = token + 1;
  Token *operand_token = token + 2;
  int rn = map_get(&registerTable, rn_token->value);
  char *ptr;
  int operand =  (strtol(operand_token->value, &ptr, 0) << 7) & 0xF80;
  operand |= rn & 0xF;
  generateDataProcessingOpcode(map_get(&mnemonicTable, "mov"), rn, NOT_NEEDED, operand, NOT_SET, NOT_SET, program);
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
