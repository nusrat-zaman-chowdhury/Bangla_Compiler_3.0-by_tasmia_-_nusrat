#include "TAC.h"
#include <iostream>

using namespace std;

TACGenerator::TACGenerator()
{
    tempCount = 0;
    labelCount = 0;
}


string TACGenerator::newTemp()
{
    return "t" + to_string(tempCount++);
}


string TACGenerator::newLabel()
{
    return "L" + to_string(labelCount++);
}


// =====================================
// EXPRESSION
// =====================================

string TACGenerator::generateExpression(
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
        string left =
            generateExpression(b->left);

        string right =
            generateExpression(b->right);

        string temp = newTemp();

        code.push_back(
            TAC(
                b->op,
                left,
                right,
                temp
            )
        );

        return temp;
    }

    return "0";
}


// =====================================
// STATEMENT
// =====================================

void TACGenerator::generateStatement(
    Stmt* node)
{
    // Declaration
    if (DeclarationStmt* d =
        dynamic_cast<DeclarationStmt*>(node))
    {
        if (d->value != nullptr)
        {
            string value =
                generateExpression(d->value);

            code.push_back(
                TAC(
                    "=",
                    value,
                    "",
                    d->name
                )
            );
        }
        else
        {
            string value =
                d->type == "বাস্তব"
                ? "0.0"
                : "0";

            code.push_back(
                TAC(
                    "=",
                    value,
                    "",
                    d->name
                )
            );
        }

        return;
    }


    // Assignment
    if (AssignmentStmt* a =
        dynamic_cast<AssignmentStmt*>(node))
    {
        string value =
            generateExpression(a->value);

        code.push_back(
            TAC(
                "=",
                value,
                "",
                a->name
            )
        );

        return;
    }


    // Print
    if (PrintStmt* p =
        dynamic_cast<PrintStmt*>(node))
    {
        string value =
            generateExpression(p->value);

        code.push_back(
            TAC(
                "print",
                value
            )
        );

        return;
    }


    // IF
    if (IfStmt* i =
        dynamic_cast<IfStmt*>(node))
    {
        string condition =
            generateExpression(
                new BinaryExpr(
                    i->left,
                    i->op,
                    i->right
                )
            );

        string elseLabel = newLabel();
        string endLabel = newLabel();

        code.push_back(
            TAC(
                "if_false",
                condition,
                "",
                elseLabel
            )
        );

        for (Stmt* s : i->thenPart)
            generateStatement(s);

        if (!i->elsePart.empty())
        {
            code.push_back(
                TAC(
                    "goto",
                    "",
                    "",
                    endLabel
                )
            );

            code.push_back(
                TAC(
                    "label",
                    "",
                    "",
                    elseLabel
                )
            );

            for (Stmt* s : i->elsePart)
                generateStatement(s);

            code.push_back(
                TAC(
                    "label",
                    "",
                    "",
                    endLabel
                )
            );
        }
        else
        {
            code.push_back(
                TAC(
                    "label",
                    "",
                    "",
                    elseLabel
                )
            );
        }

        return;
    }


    // WHILE
    if (WhileStmt* w =
        dynamic_cast<WhileStmt*>(node))
    {
        string startLabel = newLabel();
        string endLabel = newLabel();

        code.push_back(
            TAC(
                "label",
                "",
                "",
                startLabel
            )
        );

        string condition =
            generateExpression(
                new BinaryExpr(
                    w->left,
                    w->op,
                    w->right
                )
            );

        code.push_back(
            TAC(
                "if_false",
                condition,
                "",
                endLabel
            )
        );

        for (Stmt* s : w->body)
            generateStatement(s);

        code.push_back(
            TAC(
                "goto",
                "",
                "",
                startLabel
            )
        );

        code.push_back(
            TAC(
                "label",
                "",
                "",
                endLabel
            )
        );
    }
}


// =====================================
// GENERATE
// =====================================

vector<TAC> TACGenerator::generate(
    Program* program)
{
    for (Stmt* s : program->statements)
    {
        generateStatement(s);
    }

    return code;
}


// =====================================
// PRINT TAC
// =====================================

void TACGenerator::print(
    const vector<TAC>& code)
{
    cout << "\n===== THREE ADDRESS CODE =====\n";

    for (const TAC& t : code)
    {
        if (t.op == "label")
        {
            cout << t.result << ":";
        }

        else if (t.op == "goto")
        {
            cout << "goto "
                 << t.result;
        }

        else if (t.op == "if_false")
        {
            cout << "ifFalse "
                 << t.arg1
                 << " goto "
                 << t.result;
        }

        else if (t.op == "print")
        {
            cout << "print "
                 << t.arg1;
        }

        else if (t.op == "=")
        {
            cout << t.result
                 << " = "
                 << t.arg1;
        }

        else
        {
            cout << t.result
                 << " = "
                 << t.arg1
                 << " "
                 << t.op
                 << " "
                 << t.arg2;
        }

        cout << endl;
    }
} 