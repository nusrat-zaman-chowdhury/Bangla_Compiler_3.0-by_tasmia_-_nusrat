#include "CodeGenerator.h"
#include <fstream>

using namespace std;

CodeGenerator::CodeGenerator()
{
    indent = 0;
}


string CodeGenerator::spaces()
{
    return string(
        indent * 4,
        ' '
    );
}


// =====================================
// EXPRESSION → PYTHON
// =====================================

string CodeGenerator::expression(
    Expr* node)
{
    if (NumberExpr* n =
        dynamic_cast<NumberExpr*>(node))
    {
        return n->value;
    }

    if (IdExpr* id =
        dynamic_cast<IdExpr*>(node))
    {
        return id->name;
    }

    if (BinaryExpr* b =
        dynamic_cast<BinaryExpr*>(node))
    {
        return "(" +
               expression(b->left) +
               " " +
               b->op +
               " " +
               expression(b->right) +
               ")";
    }

    return "0";
}


// =====================================
// STATEMENT → PYTHON
// =====================================

void CodeGenerator::statement(
    Stmt* node,
    string& output)
{
    // Declaration
    if (DeclarationStmt* d =
        dynamic_cast<DeclarationStmt*>(node))
    {
        string value;

        if (d->value != nullptr)
            value = expression(d->value);
        else if (d->type == "বাস্তব")
            value = "0.0";
        else
            value = "0";

        output +=
            spaces() +
            d->name +
            " = " +
            value +
            "\n";

        return;
    }


    // Assignment
    if (AssignmentStmt* a =
        dynamic_cast<AssignmentStmt*>(node))
    {
        output +=
            spaces() +
            a->name +
            " = " +
            expression(a->value) +
            "\n";

        return;
    }


    // Print
    if (PrintStmt* p =
        dynamic_cast<PrintStmt*>(node))
    {
        output +=
            spaces() +
            "print(" +
            expression(p->value) +
            ")\n";

        return;
    }


    // IF
    if (IfStmt* i =
        dynamic_cast<IfStmt*>(node))
    {
        output +=
            spaces() +
            "if " +
            expression(i->left) +
            " " +
            i->op +
            " " +
            expression(i->right) +
            ":\n";

        indent++;

        if (i->thenPart.empty())
            output += spaces() + "pass\n";

        for (Stmt* s : i->thenPart)
            statement(s, output);

        indent--;

        if (!i->elsePart.empty())
        {
            output +=
                spaces() +
                "else:\n";

            indent++;

            for (Stmt* s : i->elsePart)
                statement(s, output);

            indent--;
        }

        return;
    }


    // WHILE
    if (WhileStmt* w =
        dynamic_cast<WhileStmt*>(node))
    {
        output +=
            spaces() +
            "while " +
            expression(w->left) +
            " " +
            w->op +
            " " +
            expression(w->right) +
            ":\n";

        indent++;

        if (w->body.empty())
            output += spaces() + "pass\n";

        for (Stmt* s : w->body)
            statement(s, output);

        indent--;
    }
}


// =====================================
// GENERATE
// =====================================

string CodeGenerator::generate(
    Program* program)
{
    indent = 0;

    string output =
        "# Generated Python code\n\n";

    for (Stmt* s : program->statements)
    {
        statement(s, output);
    }

    return output;
}


// =====================================
// SAVE
// =====================================

bool CodeGenerator::save(
    Program* program,
    string filename)
{
    ofstream file(filename);

    if (!file)
        return false;

    file << generate(program);

    file.close();

    return true; 
} 