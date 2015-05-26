#ifndef TOKENISE_H
#define TOKENISE_H

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef enum {LABEL,LITERAL,EXPRESSION,OTHER,NEWLINE,ENDFILE} TokenType;

typedef struct {
  char *value;
  TokenType type;
} Token;

typedef struct {
  int size;
  int fullCapacity;
  Token *tokens;
} Tokens;

void tokens_init(Tokens *tokens);
void tokens_add(Tokens *tokens, char *value, TokenType type);
void print_tokens(Tokens *tokens);
//Helpers
bool isLabel(char *string);
bool isLiteral(char *string);
bool isExpression(char *string);
char* print_tokentype(TokenType type);
void tokens_free(Tokens *tokens);

#endif /* end of include guard: TOKENISE_H */
