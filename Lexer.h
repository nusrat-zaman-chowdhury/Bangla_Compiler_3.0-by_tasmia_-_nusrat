#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

using namespace std;

class Lexer
{
private:
    string source;
    size_t position;
    int line;

public:
    Lexer(const string& source);

    vector<Token> tokenize();
};

#endif 