void getNextChar(){
    currentchar = fgetc(sourceFile);

    if(currentChar == '\n'){
        currentRow++;
        currentCol=1;
    }
    else{
        currentCol++;
    }
}

int isKeyword(char *word){
    const char* keyword[] = {"if", "else", "main", "while"};
    for(int i=0; i<4; i++){
        if(strcmp(word,keyword)){
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

    //operators
    if(strchr("+-*/%=<>!&|^~", currentChar)){
        token.type = OPERATOR;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1]='\0';
        return token;
    }

    //special symbols
    if(strchr("[]{}():;'.,", currentChar)){
        token.type = SPECIAL_SYMBOL;
        token.value[0] = currentChar;
        getNextChar();
        token.value[1]='\0';
        return token;
    }

    //number
    if(isdigit(currentChar)){
        token.type = NUMBER;
        int i=0;
        while(isdigit(currentChar)){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i]='\0';
        return token;
    }

    //id or keyword
    if(isalpha(currentChar) || currentChar == '_'){
        token.type = IDENTIFIER;
        int i=0;
        token.value[i++] = currentChar;
        getNextChar();
        
        while(isalnum(currentChar) || currentChar == '_'){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i]='\0';
        if(isKeyword(token.value)){
            token.type = KEYWORD;
            addToSymbolTable(token.value, token.type);
        }
        else{
            token.type = IDENTIFIER;
            addToSymbolTable(token.value, token.type);
        }
        return token;
    }

    //string literal
    if(currentChar == '"' || currentChar == '\''){
        char quoteType = currentChar;
        token.type = STRING_LITERAL;
        int i=0;
        token.value[i++] = currentChar;
        getNextChar();

        while(currentChar != quoteType && currentChar != EOF){
            token.value[i++] = currentChar;
            getNextChar();
        }
        if(currentChar == quoteType){
            token.value[i++] = currentChar;
            getNextChar();
        }
        token.value[i]='\0';
        return token;
    }

    token.type = UNKNOWN;
    token.value[0] = currentChar;
    token.value[1] = '\0';
    getNextChar();
    return token;
}
