#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble.h"
#include "symbolmap.h"

FILE *input = NULL, *output = NULL;

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguments");
    exit(EXIT_FAILURE);
  }

  setUpIO(argv[1], argv[2]);

  SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));
  tokenise();
  resolveLabelAddresses(lblToAddr);
  parseProgram(lblToAddr);
  map_free(lblToAddr);

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
  char buffer[512];
  map_init(map); //Set up Symbol Table
  int currAddr = 0;
  //Loop over each line getting labels and putting them
  //into the map with their respective addresses
  while (fgets(buffer, sizeof(buffer), input) != NULL) {
    if (!isBlankLine(buffer)) {
      if (hasLabel(buffer)) {
        char *label;
        label = strtok(buffer, ":");
        map_set(map, label, currAddr);
      }
      currAddr += WORD_SIZE;
    }
  }

  map_print(map);
}

#pragma mark - Helper Functions
//TODO: Tokenise
void tokenise() {

}
