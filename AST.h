#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

using namespace std;

// =====================================
// EXPRESSION NODES
// =====================================

struct Expr
{
    virtual ~Expr() {}
};

struct NumberExpr : Expr
{
    string value;
    bool real;

    NumberExpr(string value, bool real)
    {
        this->value = value;
        this->real = real;
    }
};

struct IdExpr : Expr
{
    string name;

    IdExpr(string name)
    {
        this->name = name;
    }
};

struct BinaryExpr : Expr
{
    Expr* left;
    string op;
    Expr* right;

    BinaryExpr(
        Expr* left,
        string op,
        Expr* right)
    {
        this->left = left;
        this->op = op;
        this->right = right;
    }
};

// =====================================
// STATEMENT NODES
// =====================================

struct Stmt
{
    virtual ~Stmt() {}
};

struct DeclarationStmt : Stmt
{
    string type;
    string name;
    Expr* value;

    DeclarationStmt(
        string type,
        string name,
        Expr* value = nullptr)
    {
        this->type = type;
        this->name = name;
        this->value = value;
    }
};

struct AssignmentStmt : Stmt
{
    string name;
    Expr* value;

    AssignmentStmt(
        string name,
        Expr* value)
    {
        this->name = name;
        this->value = value;
    }
};

struct PrintStmt : Stmt
{
    Expr* value;

    PrintStmt(Expr* value)
    {
        this->value = value;
    }
};

struct IfStmt : Stmt
{
    Expr* left;
    string op;
    Expr* right;

    vector<Stmt*> thenPart;
    vector<Stmt*> elsePart;

    IfStmt(
        Expr* left,
        string op,
        Expr* right)
    {
        this->left = left;
        this->op = op;
        this->right = right;
    }
};

struct WhileStmt : Stmt
{
    Expr* left;
    string op;
    Expr* right;

    vector<Stmt*> body;

    WhileStmt(
        Expr* left,
        string op,
        Expr* right)
    {
        this->left = left;
        this->op = op;
        this->right = right;
    }
};

// =====================================
// PROGRAM NODE
// =====================================

struct Program
{
    vector<Stmt*> statements;
};

#endif 