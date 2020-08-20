
#include "calculator.hpp"
#include <string>
#include <cmath>
#include <iostream>
#include <ctype.h>

// Scanner implementation

// You may have to modify this constructor, although it might not be neccessary.
Scanner::Scanner() : line(1), 
                     value(""), expr(""), \
                     expr_len(0), index(0) {

    // WRITEME
}

//Match a char to a token or return T_EOF if no match
Token Scanner::strToToken(char s){
    switch (s) {
        case '+':
            return T_PLUS;
        case '-':
            return T_MINUS;
        case '*':
            return T_MULTIPLY;
        case '/':
            return T_DIVIDE;
        case '(':
            return T_OPENPAREN;
        case ')':
            return T_CLOSEPAREN;
        case ';':
            return T_SEMICOLON;
        case '\n':
            return T_NEWLN;
        default:
            return T_EOF;
    }
}
//Check for whitespaces
bool isWhiteSpace(char ch){
    return ch==' ' || ch=='\t'|| ch == '\n';
}
//Check is there are random spaces in between numbers. There cant be spaces in between mod either
bool Scanner::isInputValid(std::string expr, char ch){
    //
    if (ch == 'm'){
        char n;
        std::string expected="mod";
        for(int i=1; i < 3; i++){
            n= getchar();
            //Guard against reading end of file ( we would have "mo")
            //If there is a random char in between mod we throw error
            if (n == EOF || n !=expected[i]){
                scanError(this->line, n);
            } 
        }
        //If it was false it would have exited on error
        return true;
    }

    //If there is a space in between numbers.
    if(ch == ' ' && isdigit(expr[expr.length()])){
        return false;
    }

    //At this point it is irrelevant if it is a white space
    if(isWhiteSpace(ch)){
        return true;
    }
    //Return false if the return token was EOF 
    //since it should never be that unless it isnt matched to a valid symbol
    if(strToToken(ch) == T_EOF){
        return false;
    }
    return true;
}
Token Scanner::nextToken() {
    if ( expr == ""){
        char ch;
        bool white=false;
        char postInt;
        while((ch = getchar()) != EOF){
            //Check if input is valid
            if(isInputValid(this->expr, ch) || isdigit(ch)){
                //Newline might need to be added?
                if( !isWhiteSpace(ch)){
                    //If last valid was a number and there has been a white space and the current char is a num
                    if(isdigit(ch) && white && isdigit(expr[expr.length()-1])){
                       scanError(lineNumber(), postInt);
                    }
                    //if ch == 'm' and it was valid then mod was actually used
                    if(ch == 'm')
                        expr+="mod";
                    else
                        expr+=ch;
                    //Reset to false since it was no whitespace
                    white= false;
                }
                else{
                    //Save the type of space after int just in case it is error.
                    if(isdigit(expr[expr.length()-1])){
                        postInt=ch;
                    }
                    //set to true for whitespace
                    white=true;
                }
                if(ch == '\n'){
                    line++;
                }

            }
            //If input invalid
            else{
                scanError(this->line,ch);
            }  
        }
        this->expr_len= expr.length();
        
    }
    //If index is equal to expr_len then we have reached the end of file
    if (index == expr_len){
        return T_EOF;
    }
    //Obtain the next token from the stored string at the current index
    char c= expr[this->index];
    if (isdigit(c)){
        if(value == ""){

            int count=index;
            while(isdigit(expr[count])){
                this->value+= expr[count];
                count+=1;
            }
        }

        return T_NUMBER;
    }
    //If character is "m" then the next two characters are "od" .
    // It is valid, else we would have thrown a scanner error
    if (c == 'm'){
        
        return T_MODULO;
    }
    //convert character to equivalent Token
    Token T = strToToken(c);

    return T;
}

void Scanner::eatToken(Token toConsume) {
    //Since we advanced index once before just advance twice more.
    if(toConsume == T_MODULO){
        index+=2;
    }
    //For numbers the index needs to advance by the length of the number
    if (toConsume ==  T_NUMBER){
        index+=value.length();
        value="";
    }
    else{
        index++;    
    }
    
}

int Scanner::lineNumber() {
    return this->line;
}

//Convert the string to a number value
long Scanner::getNumberValue() { 
    long l=std::stol(value);
    if(l>INT_MAX || l < INT_MIN){
        outOfBoundsError(lineNumber(), l);
    }
    return l;
}



// Parser implementation

Parser::Parser(bool eval) : evaluate(eval), value("") {
    // WRITEME
}

void Parser::match(Token t){
    if(scanner.nextToken() != t){
        mismatchError(scanner.lineNumber(), t, scanner.nextToken());
    }
}

void Parser::parse() {
    start();
    match(T_EOF);
}

void Parser::start() {
    long result= exprList();
    //DO STUFF
    
}

long Parser::factor(){
    //At the very bottom of the tree we can only find (, ), or numbers
    long result=0;
    if(scanner.nextToken() == T_OPENPAREN){
        match(T_OPENPAREN);
      
        scanner.eatToken(T_OPENPAREN);

        //Cannot have a call to exprlist anymore at this point
        result= addSub();

        //After the recursion ends we check if the parenthesis match.
        match(T_CLOSEPAREN);
        
        scanner.eatToken(T_CLOSEPAREN);

    }
    else if (scanner.nextToken() == T_NUMBER){
        match(T_NUMBER);
        result= scanner.getNumberValue();
        scanner.eatToken(T_NUMBER);
       
    }
    else{
        parseError(scanner.lineNumber(), scanner.nextToken());
    }
    checkOutOfBounds(result);
    return result;
}
long Parser::exprList(){
    return exprListHelper(addSub());
    
}
long Parser::exprListHelper(long l){
    long left= l;
    //Print result
    value+= std::to_string(left)+'\n';
    Token t= scanner.nextToken();
    if(t == T_SEMICOLON){
        match(T_SEMICOLON);
        scanner.eatToken(t);
        left= addSub();
        checkOutOfBounds(left);
        left=exprListHelper(left);
        value+=std::to_string(left)+'\n';
        return left;

    }
    if(evaluate){
        std::cout<<value;
    }
    
    checkOutOfBounds(left);
    return left;

}

long Parser::addSub(){
    return addSubHelper(multDiv());
}
long Parser::addSubHelper(long l){
    long result= l;
    Token t= scanner.nextToken();
    //MultDiv() could return an actual number or it can be the empty string
    if(t==T_PLUS){
        match(T_PLUS);
        scanner.eatToken(t);
        result= l + multDiv();
        checkOutOfBounds(result);
        //result =
        result=addSubHelper(result);
    }
    else if(t ==  T_MINUS){
        match(T_MINUS);
        scanner.eatToken(t);
        result= l - multDiv();
        checkOutOfBounds(result);

        result= addSubHelper(result);
    }
    checkOutOfBounds(result);
    return result;
}

long Parser::multDiv(){
    return multDivHelper(factor());
}
//perform division and check for division by zero
long division(long l, long r, int line){
    long result=l;
    if(r != 0){
            result= l / r;
    }
    else{
        divideByZeroError(line, result);
    }
    return result;
}
//Perform mod operation and prevent x mod 0 cases
long mod(long l, long r, int line){
    long result=l;
    if(r != 0){
            result= l % r;
    }
    else{
        divideByZeroError(line, result);
    }
    return result;

}
//Check that result is < INT_MAX and > INT_MIN
void Parser::checkOutOfBounds(long n){
    if (n>INT_MAX || n < INT_MIN){
        outOfBoundsError(scanner.lineNumber(), n);
    }
}

long Parser::multDivHelper(long l){
    long result=l;
    Token t= scanner.nextToken();
    if(t==T_MULTIPLY){
        match(T_MULTIPLY);
        scanner.eatToken(t);
        result= l * factor();

        checkOutOfBounds(result);
        result= multDivHelper(result);
    }
    else if(t == T_DIVIDE){
        match(T_DIVIDE);
        scanner.eatToken(t);
        long r= factor();
        //Perform division and check division by zero
        result= division(l,r, scanner.lineNumber());
        checkOutOfBounds(result);
        result= multDivHelper(result);
    }
    else if (t == T_MODULO){
        match(T_MODULO);
        scanner.eatToken(t);
        long r= factor();
        result= mod(l,r,scanner.lineNumber());
        checkOutOfBounds(result);
        result= multDivHelper(result);
    }
    checkOutOfBounds(result);
    return result;
   
}
