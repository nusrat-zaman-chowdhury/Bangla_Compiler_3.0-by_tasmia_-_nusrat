#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>

using namespace std;

class Parser
{
private:

    vector<Token> tokens;
    size_t position;
    bool error;

    Token current();

    void advance();

    bool check(TokKind type);

    bool match(TokKind type);

    bool expect(
        TokKind type,
        string message
    );

    void recover();

    Program* program();

    Stmt* statement();

    Stmt* declaration();

    Stmt* assignment();

    Stmt* printStatement();

    Stmt* ifStatement();

    Stmt* whileStatement();

    void block(vector<Stmt*>& statements);

    Expr* expression();

    Expr* term();

    Expr* factor();

    struct Condition
    {
        Expr* left;
        string op;
        Expr* right;
    };

    Condition condition();

public:

    Parser(const vector<Token>& tokens);

    Program* parse();

    bool hasError();
};

#endif 



