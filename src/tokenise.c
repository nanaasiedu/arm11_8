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

void tokens_free(Tokens *tokens) {
  free(tokens);
  free(tokens->tokens);
}
