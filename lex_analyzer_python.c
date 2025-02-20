/*Design a lexical analyser to generate tokens for a Python program with the "class" construct.

Identify keywords like "class", "def", "import", and "return".
Consider indentation levels as a part of tokenization.
String literals, numbers, operators, and special symbols should be handled.
Maintain a local symbol table for identifiers.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_BUF_SIZE 100
#define HASH_TABLE_SIZE 100

typedef enum {
    KEYWORD, IDENTIFIER, OPERATOR, NUMBER, STRING_LITERAL, INDENT, DEDENT, SPECIAL_SYMBOL, UNKNOWN
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

//indent variables
int indentStack[MAX_BUF_SIZE];
int indentTop = -1;

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
    symbolTable[index] = newSymbol; 
}

void getNextChar(){
    currentChar = fgetc(sourceFile);
    if (currentChar =='\n'){
        currentRow++;
        currentCol = 1;
    }
    else{
        currentCol++;
    }
}

//track indedent level
int countLeadingSpaces(){
    int count=0;
    while(currentChar == ' '){
        count++;
        getNextChar();
    }
    return count;
}

int isKeyword (char *word){
    const char *keywords[] = {"class" , "def", "import", "return"};
    for (int i=0; i<4; i++){
        if(strcmp(word,keywords[i]) == 0){
            return 1;
        }
    }
    return 0;
}


void skipWhitespace(){
    while(isspace(currentChar) && currentChar != '\n'){
        getNextChar();
    }
}

void skipComment(){
    while(currentChar != '\n' && currentChar != EOF){
        getNextChar();
    }
}

Token getNextToken(){
    Token token;
    token.row = currentRow;
    token.col = currentCol;
    token.value[0] = '\0';
    skipWhitespace();

    if(currentChar == EOF){
        token.type = UNKNOWN;
        return token;
    }

    //handle indentation
    if(currentCol == 1){
        int currentIndent = countLeadingSpaces();

        if(currentIndent > indentStack[indentTop]){
            indentStack[++indentTop] = currentIndent;
            token.type = INDENT;
            strcpy(token.value, "INDENT");
            return token;
        }
        while(currentIndent < indentStack[indentTop]){
            indentTop--;
            token.type = DEDENT;
            strcpy(token.value,"DEDENT");
            return token;
        }
    }

    //operators
    if(strchr("+-*/%=<>!&|^~", currentChar)){
        token.type = OPERATOR;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1] = '\0';
        return token;
    }

    //special symbols 
    if(strchr("{}[]():;,.", currentChar)){
        token.type = SPECIAL_SYMBOL;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1] = '\0';
        return token;
    }

    //number
    if(isdigit(currentChar)){
        int i=0;
        token.type = NUMBER;
        while(isdigit(currentChar)){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i] = '\0';
        return token;
    }

    //id or keyword
    if(isalpha(currentChar) || currentChar == '_'){
        int i=0;
        token.type = IDENTIFIER;
        token.value[i++] = currentChar;
        getNextChar();

        while(isalnum(currentChar) || currentChar == '_'){
            token.value[i++] = currentChar;
            getNextChar();
        }

        token.value[i] = '\0';
        if (isKeyword(token.value)){
            token.type = KEYWORD;
            addToSymbolTable(token.value, KEYWORD);
        }
        else{
            token.type = IDENTIFIER;
            addToSymbolTable(token.value, IDENTIFIER);
        }
        return token;
    }

    //string literal
    if(currentChar == '"' || currentChar == '\'' ){
        char quoteType = currentChar;
        int i=0;
        token.type = STRING_LITERAL;
        token.value[i++] = currentChar;
        getNextChar();
        while(currentChar != quoteType && currentChar != EOF){
            token.value[i++] = currentChar;
            getNextChar();
        }
        if (currentChar == quoteType){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i]='\0';
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
    for(int i=0; i<HASH_TABLE_SIZE; i++){
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
        case INDENT: typeStr = "INDENT"; break;
        case DEDENT: typeStr = "DEDENT"; break;
        case SPECIAL_SYMBOL: typeStr = "SPECIAL_SYMBOL"; break;
        case UNKNOWN: typeStr = "UNKNOWN"; break;
        default: typeStr = "UNKNOWN"; break;
    }
    printf("Row: %d, Col: %d, Type: %s, Value: %s\n", token.row, token.col, typeStr, token.value);
}

int main() {
    sourceFile = fopen("program.py", "r");
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
