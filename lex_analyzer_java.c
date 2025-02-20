/*Write a getNextToken() to generate tokens for the Java code below:
   class Test {
       public static void main(String[] args) {
           int sum = 0;
           for (int i = 0; i < 10; i++) {
               sum += i;
           }
           System.out.println(sum);
       }
   }
   - Keywords: class, public, static, void, int, for
   - Operators: +=, ++, <, >=
   - String literals: "sum"
   - Handle nested braces and parentheses.*/
   
   #include <stdio.h>
   #include <string.h>
   #include <stdlib.h>
   #include <ctype.h>
   
   #define MAX_BUF_SIZE 100
   
   typedef enum {
       KEYWORD, IDENTIFIER, OPERATOR, NUMBER, STRING_LITERAL, SPECIAL_SYMBOL, UNKNOWN
   } TokenType;
   
   typedef struct {
       TokenType type;
       char value[MAX_BUF_SIZE];
   } Token;
   
   FILE *sourceFile;
   char currentChar;
   int line = 1, column = 0;
   
   void getNextChar() {
       currentChar = fgetc(sourceFile);
       column++;
       if (currentChar == '\n') {
           line++;
           column = 0;
       }
   }
   
   void skipWhitespace() {
       while (isspace(currentChar)) {
           getNextChar();
       }
   }
   
   int isKeyword(char *word) {
       const char *keywords[] = {"class", "public", "static", "void", "int", "for"};
       for (int i = 0; i < 6; i++) {
           if (strcmp(word, keywords[i]) == 0) {
               return 1;
           }
       }
       return 0;
   }
   
   Token getNextToken() {
       Token token;
       token.value[0] = '\0';
   
       skipWhitespace();
   
       if (currentChar == EOF) {
           token.type = UNKNOWN;
           return token;
       }
   
       // Identifiers and keywords
       if (isalpha(currentChar)) {
           int i = 0;
           token.value[i++] = currentChar;
           getNextChar();
           while (isalnum(currentChar) || currentChar == '_') {
               token.value[i++] = currentChar;
               getNextChar();
           }
           token.value[i] = '\0';
   
           if (isKeyword(token.value)) {
               token.type = KEYWORD;
           } else {
               token.type = IDENTIFIER;
           }
           return token;
       }
   
       // Numbers
       if (isdigit(currentChar)) {
           int i = 0;
           while (isdigit(currentChar)) {
               token.value[i++] = currentChar;
               getNextChar();
           }
           token.value[i] = '\0';
           token.type = NUMBER;
           return token;
       }
   
       // String literals
       if (currentChar == '"') {
           int i = 0;
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
           token.type = STRING_LITERAL;
           return token;
       }
   
       // Operators
       if (strchr("+-*/%=<>", currentChar)) {
           int i = 0;
           token.value[i++] = currentChar;
           getNextChar();
           if ((token.value[0] == '+' || token.value[0] == '-' || token.value[0] == '<' || token.value[0] == '>') && currentChar == '=') {
               token.value[i++] = currentChar;
               getNextChar();
           }
           token.value[i] = '\0';
           token.type = OPERATOR;
           return token;
       }
   
       // Special symbols (including nested braces and parentheses)
       if (strchr("{}[](),;.", currentChar)) {
           token.type = SPECIAL_SYMBOL;
           token.value[0] = currentChar;
           token.value[1] = '\0';
           getNextChar();
           return token;
       }
   
       // Unknown token
       token.type = UNKNOWN;
       token.value[0] = currentChar;
       token.value[1] = '\0';
       getNextChar();
       return token;
   }
   
   void printToken(Token token) {
       const char *typeStr;
       switch (token.type) {
           case KEYWORD: typeStr = "KEYWORD"; break;
           case IDENTIFIER: typeStr = "IDENTIFIER"; break;
           case OPERATOR: typeStr = "OPERATOR"; break;
           case NUMBER: typeStr = "NUMBER"; break;
           case STRING_LITERAL: typeStr = "STRING_LITERAL"; break;
           case SPECIAL_SYMBOL: typeStr = "SPECIAL_SYMBOL"; break;
           case UNKNOWN: typeStr = "UNKNOWN"; break;
           default: typeStr = "UNKNOWN"; break;
       }
       printf("Type: %s, Value: %s, Line: %d, Column: %d\n", typeStr, token.value, line, column);
   }
   
   int main() {
       sourceFile = fopen("program.java", "r");
       if (sourceFile == NULL) {
           printf("Could not open file.\n");
           return 1;
       }
   
       getNextChar();
       while (currentChar != EOF) {
           Token token = getNextToken();
           if (token.type != UNKNOWN || token.value[0] != '\0') {
               printToken(token);
           }
       }
   
       fclose(sourceFile);
       return 0;
   }
   
