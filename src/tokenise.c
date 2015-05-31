#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tokenise.h"

void tokens_init(Tokens *tokens) {
  tokens->size = 0;
  tokens->fullCapacity = 100;
  tokens->tokens = malloc(sizeof(Token) * tokens->fullCapacity);
}

void tokens_add(Tokens *tokens, char *value, TokenType type) {
  Token *token = &tokens->tokens[tokens->size];
  token->value = malloc(sizeof(char)*512);
  strcpy(token->value, value);
  token->type = type;
  tokens->size++;
}

bool isLabel(char *string) {
  int length = strlen(string);
  return isalpha(string[0]) && (string[length-1] == ':');
}

bool isLiteral(char *string) {
  return string[0] == '#';
}

bool isExpression(char *string) {
  return string[0] == '=';
}

bool isNewLine(char *string) {
  return string[0] == '\n';
}

void tokens_free(Tokens *tokens) {
  free(tokens);
  free(tokens->tokens);
}

void print_tokens(Tokens *tokens) {
  for (size_t i = 0; i < tokens->size; i++) {
    printf("Token: %s, %s\n",
      tokens->tokens[i].value,
      print_tokentype(tokens->tokens[i].type));
  }
}

//Helper Functions
char* print_tokentype(TokenType type) {
  switch (type) {
    case LABEL:
      return "Label";
    case LITERAL:
      return "Literal";
    case EXPRESSION:
      return "Expression";
    case OTHER:
      return "Other";
    case NEWLINE:
      return "NewLine";
    case ENDFILE:
      return "ENDFILE";
    default:
      return "ERROR";
  }
}
