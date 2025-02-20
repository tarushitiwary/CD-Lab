/*Design a lexical analyser for a JavaScript program with arrow functions.
   - Recognize "let", "const", "function", and "return" as keywords.
   - Identify the "=>" arrow function token.
   - Treat comments // and /* */ as "single tokens" */
//  - Handle string literals enclosed in both single and double quotes 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    KEYWORD, IDENTIFIER, OPERATOR, STRING_LITERAL, COMMENT, SPECIAL_SYMBOL, UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

const char *keywords[] = {"let", "const", "function", "return"};
const int num_keywords = 4;

char currentChar;

void getNextChar(FILE *file) {
    currentChar = fgetc(file);
}

void skipWhitespace(FILE *file) {
    while (isspace(currentChar)) {
        getNextChar(file);
    }
}

int isKeyword(const char *str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

Token getNextToken(FILE *file) {
    Token token;
    token.type = UNKNOWN;
    token.value[0] = '\0';

    skipWhitespace(file);

    if (currentChar == EOF) {
        return token;
    }

    if (isalpha(currentChar) || currentChar == '_') {
        // Identifiers and keywords
        int i = 0;
        while (isalnum(currentChar) || currentChar == '_') {
            token.value[i++] = currentChar;
            getNextChar(file);
        }
        token.value[i] = '\0';
        token.type = isKeyword(token.value) ? KEYWORD : IDENTIFIER;
        return token;
    }

    if (currentChar == '/') {
        getNextChar(file);
        if (currentChar == '/') {
            // Single-line comment
            int i = 0;
            token.value[i++] = '/';
            token.value[i++] = '/';
            getNextChar(file);
            while (currentChar != '\n' && currentChar != EOF) {
                token.value[i++] = currentChar;
                getNextChar(file);
            }
            token.value[i] = '\0';
            token.type = COMMENT;
            return token;
        } else if (currentChar == '*') {
            // Multi-line comment
            int i = 0;
            token.value[i++] = '/';
            token.value[i++] = '*';
            getNextChar(file);
            while (currentChar != EOF) {
                token.value[i++] = currentChar;
                if (currentChar == '*') {
                    getNextChar(file);
                    if (currentChar == '/') {
                        token.value[i++] = '/';
                        getNextChar(file);
                        break;
                    }
                } else {
                    getNextChar(file);
                }
            }
            token.value[i] = '\0';
            token.type = COMMENT;
            return token;
        }
    }

    if (currentChar == '=') {
        getNextChar(file);
        if (currentChar == '>') {
            // Arrow function
            strcpy(token.value, "=>");
            token.type = OPERATOR;
            getNextChar(file);
            return token;
        }
    }

    if (currentChar == '"' || currentChar == '\'') {
        // String literals
        char quote = currentChar;
        int i = 0;
        token.value[i++] = currentChar;
        getNextChar(file);
        while (currentChar != quote && currentChar != EOF) {
            token.value[i++] = currentChar;
            getNextChar(file);
        }
        if (currentChar == quote) {
            token.value[i++] = currentChar;
            getNextChar(file);
        }
        token.value[i] = '\0';
        token.type = STRING_LITERAL;
        return token;
    }

    // Single character tokens (special symbols)
    token.value[0] = currentChar;
    token.value[1] = '\0';
    token.type = SPECIAL_SYMBOL;
    getNextChar(file);
    return token;
}

int main() {
    FILE *file = fopen("example.js", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    getNextChar(file);
    Token token;
    while (currentChar != EOF) {
        token = getNextToken(file);
        if (token.value[0] != '\0') {
            printf("Type: %d, Value: %s\n", token.type, token.value);
        }
    }

    fclose(file);
    return 0;
}

