#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble.h"
#include "symbolmap.h"


FILE *input = NULL, *output = NULL;
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
  0,1,10,11,12,13,14
};                               

SymbolTable mnemonicTable = {23,23,mnemonicStrings,mnemonicInts}; 
Token *tokens;

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }
  
  //Setup
  setUpIO(argv[1], argv[2]);
 
  //SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));
  tokenise();
  //resolveLabelAddresses(lblToAddr);
  //parseProgram(lblToAddr);
  //map_free(lblToAddr);

  fclose(input);
  fclose(output);

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
//TODO: resolve completely redo
void resolveLabelAddresses(SymbolTable *map){
  // char buffer[512];
  // map_init(map); //Set up Symbol Table
  // int currAddr = 0;
  // //Loop over each line getting labels and putting them
  // //into the map with their respective addresses
  // while (fgets(buffer, sizeof(buffer), input) != NULL) {
  //   if (!isBlankLine(buffer)) {
  //     if (hasLabel(buffer)) {
  //       char *label;
  //       label = strtok(buffer, ":");
  //       map_set(map, label, currAddr);
  //     }
  //     currAddr += WORD_SIZE;
  //   }
  // }

  // map_print(map);
}

void parseProgram(SymbolTable *map) {
  int i = 0;
  while (tokens[i].type != ENDFILE) {
    parseLine(tokens+i);
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
    case SUB: parseSub()
    break;
    case RSB: parseRsb()
    break;
    case AND: parseAnd()
    break;
    case EOR: parseEor()
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

#pragma mark - Helper Functions
//TODO: Tokenise
void tokenise() {

}

int index_of(Token *token) {
  int i = 0;
  while (strcmp(token->value,mnemonicStrings[i]) != 0) {
    i++;
  } 
  return i; 
}








