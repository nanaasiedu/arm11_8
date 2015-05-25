#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assemble.h"
#include "symbolmap.h"
#include "tokenise.h"

FILE *input = NULL, *output = NULL;
//SymbolTable *mnemonicTable = malloc(sizeof(SymbolTable));
Tokens *tokens = NULL;

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  //Setup
  createMnemonicTable();
  tokens = malloc(sizeof(Tokens));
  tokens_init(tokens);
  setUpIO(argv[1], argv[2]);

  //SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));
  tokenise();
  //resolveLabelAddresses(lblToAddr);
  //parseProgram(lblToAddr);
  //map_free(lblToAddr);

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

void parseProgram(SymbolTable *map) {}

void createMnemonicTable() {

  // mnemonicTable->fullCapacity = 23;
  // mnemonicTable->size = 23;
  // mnemonicTable->keys = {"add","sub","rsb","and","eor",
  //                        "orr","mov","tst","teq","cmp",
  //                        "mul","mla",
  //                        "ldr","str",
  //                        "beq","bne", "bge","blt","bgt","ble","b",
  //                        "lsl","andeq"};
  // mnemonicTable->values = {4,2,3,0,1,
  //                          12,13,8,9,10,
  //                          0,1,
  //                          0,0,
  //                          0,1,10,11,12,13,14};
}

void dealloc() {
  fclose(input);
  fclose(output);
  //map_free(mnemonicTable);
  free(tokens);
  free(tokens->tokens);
}
