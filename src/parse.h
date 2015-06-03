#ifndef PARSE_H
#define PARSE_H

#include "generate.h"
#include "tokenise.h"
#include "helpers/symbolmap.h"
#include "helpers/table.h"
#include "helpers/queue.h"
#include "helpers/definitions.h"
#include "helpers/bitUtils.h"
#include <stdint.h>

extern Tokens *tokens;
extern address addr;
extern int programLength;

void parseProgram(SymbolTable *map, Program *program);
void parseLine(Token *token, Program *program);
void parseInstruction(Token *token, Program *program);
void parseTurnaryDataProcessing(Token *token, Program *program);
void parseBinaryDataProcessing(Token *token, Program *program);
void parseMul(Token *token, Program *program);
void parseMla(Token *token, Program *program);
void parseB(Token *token, Program *program);
void parseLsl(Token *token, Program *program);
void parseSingleDataTransfer(Token *token, Program *program);

int index_of(char *value, char **arr);
char* stripBrackets(char *str);
char* stripLastBracket(char *str);
bool isPreIndex(char *str);

#endif /* end of include guard: PARSE_H */
