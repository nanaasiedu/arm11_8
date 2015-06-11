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

void parseProgram(SymbolTable *map, Program *program);
void parseLine(Token *token, Program *program);
void parseInstruction(Token *token, Program *program);
void parseTurnaryDataProcessing(Token *token, Program *program); //Try to combine
void parseBinaryDataProcessing(Token *token, Program *program);
void parseMul(Token *token, Program *program);
void parseBranch(Token *token, Program *program);
void parseLsl(Token *token, Program *program);
void parseSingleDataTransfer(Token *token, Program *program);

//Helpers
int index_of(char *value, char **arr);
char* stripBrackets(char *str);
char* stripLastBracket(char *str);
bool isPreIndex(char *str);
void generateShift(Token *shiftTypeToken, int32_t *currOffset);

#endif /* end of include guard: PARSE_H */
