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
  char *data = compile();
  outputData(data);

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

void outputData(char *data) {
  if (fputs(data, output) <= 0) {
    perror("Write to binary file failed.");
    exit(EXIT_FAILURE);
  }

  fclose(input);
  fclose(output);
}

#pragma mark - Compile

char* compile() {
  // char *output;

  SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));

  firstPass(lblToAddr, input);

  //TODO: Second Pass - Florian
  secondPass(lblToAddr);

  map_free(lblToAddr);

  return "Done";
}

void firstPass(SymbolTable *map){
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

void secondPass(SymbolTable *map) {

}

#pragma mark - Helper Functions

bool hasLabel(char *str) {
  const char labelSep = ':';
  return strchr(str, labelSep) != NULL;
}

bool isBlankLine(char *str) {
  return str[0] == '\n';
}
