#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "AST.h"
#include <map>
#include <vector>

using namespace std;

struct Symbol
{
    string name;
    string type;
    bool initialized;
};

class SymbolTable
{
private:
    map<string, Symbol> variables;

public:
    bool define(
        string name,
        string type,
        bool initialized,
        string& error
    );

    Symbol* lookup(string name);

    void print();
};


class SemanticAnalyzer
{
private:
    SymbolTable table;
    vector<string> errors;

    string expressionType(Expr* node);

    void analyzeStatement(Stmt* node);

public:
    bool analyze(Program* program);

    void printErrors();

    void printSymbols();
};

#endif 