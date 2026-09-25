#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include <string>

using namespace std;

class CodeGenerator
{
private:
    int indent;

    string spaces();

    string expression(Expr* node);

    void statement(
        Stmt* node,
        string& output
    );

public:
    CodeGenerator();

    string generate(Program* program);

    bool save(
        Program* program,
        string filename
    );
};

#endif 