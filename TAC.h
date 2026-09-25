#ifndef TAC_H
#define TAC_H

#include "AST.h"
#include <vector>

using namespace std;

struct TAC
{
    string op;
    string arg1;
    string arg2;
    string result;

    TAC(
        string op,
        string arg1 = "",
        string arg2 = "",
        string result = "")
    {
        this->op = op;
        this->arg1 = arg1;
        this->arg2 = arg2;
        this->result = result;
    }
};

class TACGenerator
{
private:
    vector<TAC> code;

    int tempCount;
    int labelCount;

    string newTemp();
    string newLabel();

    string generateExpression(Expr* node);

    void generateStatement(Stmt* node);

public:
    TACGenerator();

    vector<TAC> generate(Program* program);

    static void print(
        const vector<TAC>& code
    );
};

#endif 