#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbolmap.h"

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef WORD_SIZE
#define WORD_SIZE 4
#endif

char* compile(FILE *stream);
bool hasLabel(char *str);
bool isBlankLine(char *str);
void firstPass(SymbolTable *map, FILE *stream);
void secondPass(SymbolTable *map);

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguements");
    exit(EXIT_FAILURE);
  }

  FILE *input;
  FILE *output;

  //Get input file
  if ((input = fopen(argv[1], "r")) == NULL) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  //Get output file
  if ((output = fopen(argv[2], "w")) == NULL) {
    perror(argv[2]);
    exit(EXIT_FAILURE);
  }

  char *data = compile(input);
  
  //Write data to output file
  if (fputs(data, output) <= 0) {
    perror("Write to binary file failed.");
    exit(EXIT_FAILURE);
  }

  fclose(input);
  fclose(output);

  return EXIT_SUCCESS;
}

#pragma mark - Compile

char* compile(FILE *stream) {
  // char *output;

  SymbolTable *lblToAddr = malloc(sizeof(SymbolTable));

  firstPass(lblToAddr, stream);

  //TODO: Second Pass - Florian
  secondPass(lblToAddr);

  map_free(lblToAddr);
  free(lblToAddr);

  return "Done";
}

void firstPass(SymbolTable *map, FILE *stream){
  char buffer[512];
  map_init(map); //Set up Symbol Table
  int currAddr = 0;
  //Loop over each line getting labels and putting them
  //into the map with their respective addresses
  while (fgets(buffer, sizeof(buffer), stream) != NULL) {
    if (!isBlankLine(buffer)) {
      if (hasLabel(buffer)) {
        char *label;
        label = strtok(buffer, ":");
        map_set(map, label, currAddr);
      }
      currAddr += WORD_SIZE;
    } else {
      continue;
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
