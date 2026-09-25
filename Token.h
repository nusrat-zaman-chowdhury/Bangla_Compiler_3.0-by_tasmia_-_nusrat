#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum class TokKind
{
    START,
    END,
    DECLARE,
    INT_TYPE,
    REAL_TYPE,
    IF,
    ELSE,
    WHILE,
    PRINT,

    IDENTIFIER,
    INTEGER,
    DECIMAL,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,

    SEMICOLON,

    END_OF_FILE,
    UNKNOWN
};

struct Token
{
    TokKind type;
    string value;
    int line;
};

string getTokenTypeName(TokKind type);

#endif 