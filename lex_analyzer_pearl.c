/*Write a getNextToken ( ) to generate tokens for the perl script given below.
#! /usr/bin/perl
#get total number of arguments passed.
$n = scalar (@_);
$sum = 0;
foreach $item(@_) {
$sum += $item;
}
$average = $sum + $n;
#! Represents path which has to be ignored by getNextToken().
# followed by any character other than ! represents comments.
$n followed by any identifier should be treated as a single token.
Scalar, foeach are considered as keywords.
@_, += are treated as single tokens.
Remaining symbols are tokenized accordingly*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_BUF_SIZE 100
#define HASH_TABLE_SIZE 100

typedef enum {
    KEYWORD, IDENTIFIER, OPERATOR, NUMBER, STRING_LITERAL, COMMENT, SPECIAL_SYMBOL, UNKNOWN
} TokenType;

typedef struct {
    int row;
    int col;
    TokenType type;
    char value[MAX_BUF_SIZE];
} Token;

typedef struct Symbol {
    char name[MAX_BUF_SIZE];
    int row;
    int col;
    TokenType type;
    struct Symbol *next;
} Symbol;

Symbol *symbolTable[HASH_TABLE_SIZE] = {NULL};
FILE *sourceFile;
int currentRow = 1, currentCol = 1;
char currentChar;

int hash(char *str){
    int hash=0;
    while (*str){
        hash = (hash * 31 + *str) % HASH_TABLE_SIZE;
        str++;
    }
    return hash;
}

void addToSymbolTable(char *name, TokenType type){
    
    int index = hash(name);
    Symbol *entry = symbolTable[index];
    
    //check for duplicates
    while(entry != NULL){
        if(strcmp(entry->name,name)==0){
            return; //entry already exists
        }
        entry = entry->next;
    }

    Symbol *newSymbol = (Symbol *)malloc(sizeof(Symbol));
    strcpy(newSymbol->name,name);
    newSymbol->row = currentRow;
    newSymbol->col = currentCol;
    newSymbol->type = type;
    newSymbol->next = symbolTable[index];
    symbolTable[index]= newSymbol; 
}

void getNextChar() {
    currentChar = fgetc(sourceFile);
    if (currentChar == '\n') {
        currentRow++;
        currentCol = 1;
    } else {
        currentCol++;
    }
}

int isKeyword(char *word) {
    const char *keywords[] = {"scalar", "foreach"};
    for (int i = 0; i < 2; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void skipWhitespace() {
    while (isspace(currentChar)) {
        getNextChar();
    }
}

void skipComment() {
    while (currentChar != '\n' && currentChar != EOF) {
        getNextChar();
    }
}

Token getNextToken() {
    Token token;
    token.row = currentRow;
    token.col = currentCol;
    token.value[0] = '\0';
    skipWhitespace();

    if (currentChar == EOF) {
        token.type = UNKNOWN;
        return token;
    }

    if (currentChar == '#') {
        getNextChar();
        if (currentChar == '!') {
            //ignore (#!)
            while (currentChar != '\n' && currentChar != EOF) {
                getNextChar();
            }
            return getNextToken();
        } else {
            //skip regular comments 
            skipComment();
            return getNextToken();
        }
    }

    if (currentChar == '$') {
        int i = 0;
        token.type = IDENTIFIER;
        token.value[i++] = currentChar; //add $ to token value
        getNextChar();

        if(currentChar == 'n'){ //special check for $n as single token
            token.value[i++]=currentChar; //add n to token 
            getNextChar();

            //continue collecting identifiers
            while (isalnum(currentChar) || currentChar == '_') {
                token.value[i++] = currentChar;
                getNextChar();
            }
        }
        else{
            //general identifier starting with $
            while (isalnum(currentChar) || currentChar == '_') {
                token.value[i++] = currentChar;
                getNextChar();
            }  
        }      
            token.value[i] = '\0';
            addToSymbolTable(token.value, token.type);
            return token;
    }

    if (currentChar == '@') {
        int i = 0;
        token.type = IDENTIFIER;
        token.value[i++] = currentChar;  // add '@'
        getNextChar();

        if (currentChar == '_') {        // special check for '@_' pattern
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        addToSymbolTable(token.value, token.type);
        return token;
    }    

    if(currentChar == '+'){
        int i = 0;
        token.type = IDENTIFIER;
        token.value[i++] = currentChar;
        getNextChar();

        if(currentChar == '='){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        addToSymbolTable(token.value, token.type);
        return token;
    }

    //id or keyword
    if (isalpha(currentChar)) {
        int i = 0;
        token.type = IDENTIFIER;
        token.value[i++] = currentChar;
        getNextChar();
        while (isalnum(currentChar) || currentChar == '_') {
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        if (isKeyword(token.value)) {
            token.type = KEYWORD;
            addToSymbolTable(token.value,token.type);
        }
        else{
            token.type = IDENTIFIER;
            addToSymbolTable(token.value, token.type);
        }
        return token;
    }

    //number
    if (isdigit(currentChar)) {
        int i = 0;
        token.type = NUMBER;
        while (isdigit(currentChar)) {
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        return token;
    }

    //operator
    if (strchr("+-*/^%=!-|&><", currentChar)) {
        token.type = OPERATOR;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1] = '\0';
        return token;
    }

    //special symbol
    if (strchr("{}[](),.:;?", currentChar)) {
        token.type = SPECIAL_SYMBOL;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1] = '\0';
        return token;
    }

    //string literal
    if (currentChar == '"') {
        int i = 0;
        token.type = STRING_LITERAL;
        token.value[i++] = currentChar;
        getNextChar();
        while (currentChar != '"' && currentChar != EOF) {
            token.value[i++] = currentChar;
            getNextChar();
        }
        if (currentChar == '"') {
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        return token;
    }

    //unknown token
    token.type = UNKNOWN;
    token.value[0] = currentChar;
    token.value[1] = '\0';
    getNextChar();
    return token;
}

void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("Name\tRow\tCol\n");
    for (int i=0; i< HASH_TABLE_SIZE; i++){
        Symbol *current = symbolTable[i];
        while (current != NULL) {
            printf("%s\t%d\t%d\n", current->name, current->row, current->col);
            current = current->next;
    }
    }
}

void printToken(Token token) {
    const char *typeStr;
    switch (token.type) {
        case KEYWORD: typeStr = "KEYWORD"; break;
        case IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case OPERATOR: typeStr = "OPERATOR"; break;
        case NUMBER: typeStr = "NUMBER"; break;
        case STRING_LITERAL: typeStr = "STRING_LITERAL"; break;
        case COMMENT: typeStr = "COMMENT"; break;
        case SPECIAL_SYMBOL: typeStr = "SPECIAL_SYMBOL"; break;
        case UNKNOWN: typeStr = "UNKNOWN"; break;
        default: typeStr = "UNKNOWN"; break;
    }
    printf("Row: %d, Col: %d, Type: %s, Value: %s\n", token.row, token.col, typeStr, token.value);
}

int main() {
    sourceFile = fopen("program.pl", "r");
    if (sourceFile == NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    getNextChar();
    while (currentChar != EOF) {
        Token token = getNextToken();
        if (token.type != UNKNOWN) {
            printToken(token);
        }
    }

    printSymbolTable();
    fclose(sourceFile);
    return 0;
}
