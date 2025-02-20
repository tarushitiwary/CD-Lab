void skipWhitespace(){
    while(isspace(currentChar) && currentChar != '\n'){
        getnextChar();
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
        token.value[0]=currentChar;
        getNextChar();
        token.value[1]='\0';
        return token;
    }

    //special symbols
    if(strchr("[]{}():;,.",currentChar)){
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
        while(isdigit(currentChar))
    }

}