#pragma once

#include "helpers.hpp"

#include <climits>

// SCANNER DEFINITION

// You can freely add member fields and functions to this class.
class Scanner {
    std::string expr;
    int expr_len;
    int index;
    int line;
    std::string value;
public:
    // You really need to implement these four methods for the scanner to work.
    Token nextToken();
    void eatToken(Token);
    int lineNumber();
    long getNumberValue();
    bool isInputValid(std::string expr, char ch);
    Token strToToken(char s);
    
    Scanner();
};

// PARSER DEFINITION

// You can freely add member fields and functions to this class.
class Parser {

    Scanner scanner;
    Token T;
    bool evaluate;
    std::string value;
    void start();
    void match(Token t);
    void checkOutOfBounds(long n);
   

public:
    void parse();

    long exprList();
    long exprListHelper(long l);
    long addSub();
    long addSubHelper(long l);
    long multDiv();
    long multDivHelper(long l);
    long factor();
    
    Parser(bool);
};