#ifndef TOKENISE_H
#define TOKENISE_H

typedef int bool;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef enum {LABEL,LITERAL,OTHER,NEWLINE,ENDFILE} TokenType;

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
void tokens_print(Tokens *tokens);
//Helpers
bool isLabel(char *string);
bool isLiteral(char *string);
char* print_tokentype(TokenType type);

#endif /* end of include guard: TOKENISE_H */
