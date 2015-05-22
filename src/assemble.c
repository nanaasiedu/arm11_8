#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//TODO: Binary writer
//TODO: Symbol Table
//TODO: Tokeniser
//TODO: Instructions

int main(int argc, char **argv) {

  if (argc != 3) {
    perror("Incorrect Number of Arguements");
    exit(EXIT_FAILURE);
  }

  FILE *input;
  FILE *output;

  if ((input = fopen(argv[1], "r")) == NULL) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  if ((output = fopen(argv[2], "w")) == NULL) {
    perror(argv[2]);
    exit(EXIT_FAILURE);
  }

  printf("Correct Number of arguments\n");

  printf("Compiles...\n");

  printf("Now we write to file: ");

  char *data = "This is the data we want to write to the file";

  printf("%s\n", data);
  //Write data to output file
  if (fputs(data, output) <= 0) {
    perror("Write to binary file failed.");
    exit(EXIT_FAILURE);
  }

  fclose(input);
  fclose(output);

  return EXIT_SUCCESS;
}
