#ifndef TOKENISE_H
#define TOKENISE_H

#include "helpers/definitions.h"
#include "helpers/symbolmap.h"

void tokens_init(Tokens *tokens);
void tokens_add(Tokens *tokens, char *value, TokenType type);
//Helpers
bool isLabel(char *string);
bool isLiteral(char *string);
bool isExpression(char *string);
bool isNewLine(char *string);
void tokens_free(Tokens *tokens);

int mnemonic_name(Token *token);

char* print_tokentype(TokenType type);
void print_tokens(Tokens *tokens);

#endif /* end of include guard: TOKENISE_H */
