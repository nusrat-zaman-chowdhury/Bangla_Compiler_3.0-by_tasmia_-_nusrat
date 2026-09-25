#include "ASTPrinter.h"
#include <iostream>

using namespace std;

static string pad(int depth)
{
    return string(depth * 2, ' ');
}

static void printExpr(Expr* node, int depth)
{
    if (NumberExpr* n = dynamic_cast<NumberExpr*>(node))
    {
        cout << pad(depth) << "Number(" << n->value << ")\n";
        return;
    }
    if (IdExpr* id = dynamic_cast<IdExpr*>(node))
    {
        cout << pad(depth) << "Id(" << id->name << ")\n";
        return;
    }
    if (BinaryExpr* b = dynamic_cast<BinaryExpr*>(node))
    {
        cout << pad(depth) << "BinaryExpr(" << b->op << ")\n";
        printExpr(b->left, depth + 1);
        printExpr(b->right, depth + 1);
        return;
    }
    cout << pad(depth) << "UnknownExpr\n";
}

static void printStmt(Stmt* node, int depth)
{
    if (DeclarationStmt* d = dynamic_cast<DeclarationStmt*>(node))
    {
        cout << pad(depth) << "Declaration(" << d->type << " " << d->name << ")\n";
        if (d->value != nullptr) printExpr(d->value, depth + 1);
        return;
    }
    if (AssignmentStmt* a = dynamic_cast<AssignmentStmt*>(node))
    {
        cout << pad(depth) << "Assignment(" << a->name << ")\n";
        printExpr(a->value, depth + 1);
        return;
    }
    if (PrintStmt* p = dynamic_cast<PrintStmt*>(node))
    {
        cout << pad(depth) << "Print\n";
        printExpr(p->value, depth + 1);
        return;
    }
    if (IfStmt* i = dynamic_cast<IfStmt*>(node))
    {
        cout << pad(depth) << "If(" << i->op << ")\n";
        cout << pad(depth + 1) << "Condition-Left:\n";
        printExpr(i->left, depth + 2);
        cout << pad(depth + 1) << "Condition-Right:\n";
        printExpr(i->right, depth + 2);
        cout << pad(depth + 1) << "Then:\n";
        for (Stmt* s : i->thenPart) printStmt(s, depth + 2);
        if (!i->elsePart.empty())
        {
            cout << pad(depth + 1) << "Else:\n";
            for (Stmt* s : i->elsePart) printStmt(s, depth + 2);
        }
        return;
    }
    if (WhileStmt* w = dynamic_cast<WhileStmt*>(node))
    {
        cout << pad(depth) << "While(" << w->op << ")\n";
        cout << pad(depth + 1) << "Condition-Left:\n";
        printExpr(w->left, depth + 2);
        cout << pad(depth + 1) << "Condition-Right:\n";
        printExpr(w->right, depth + 2);
        cout << pad(depth + 1) << "Body:\n";
        for (Stmt* s : w->body) printStmt(s, depth + 2);
        return;
    }
    cout << pad(depth) << "UnknownStmt\n";
}

void printAST(Program* program)
{
    cout << "Program\n";
    for (Stmt* s : program->statements)
        printStmt(s, 1);
} 