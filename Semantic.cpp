#include "Semantic.h"
#include <iostream>

using namespace std;


// =====================================
// SYMBOL TABLE
// =====================================

bool SymbolTable::define(
    string name,
    string type,
    bool initialized,
    string& error)
{
    if (variables.count(name))
    {
        error =
            "Variable '" +
            name +
            "' is already declared.";

        return false;
    }

    variables[name] =
    {
        name,
        type,
        initialized
    };

    return true;
}


Symbol* SymbolTable::lookup(string name)
{
    auto it = variables.find(name);

    if (it == variables.end())
        return nullptr;

    return &it->second;
}


void SymbolTable::print()
{
    cout << "\n===== SYMBOL TABLE =====\n";

    cout << "Name\tType\tStatus\n";

    for (auto& item : variables)
    {
        cout
            << item.second.name
            << "\t"
            << item.second.type
            << "\t"
            << (item.second.initialized
                ? "initialized"
                : "uninitialized")
            << endl;
    }
}


// =====================================
// EXPRESSION TYPE
// =====================================

string SemanticAnalyzer::expressionType(Expr* node)
{
    // Number
    if (NumberExpr* n =
        dynamic_cast<NumberExpr*>(node))
    {
        if (n->real)
            return "বাস্তব";

        return "সংখ্যা";
    }


    // Identifier
    if (IdExpr* id =
        dynamic_cast<IdExpr*>(node))
    {
        Symbol* symbol =
            table.lookup(id->name);

        if (symbol == nullptr)
        {
            errors.push_back(
                "Variable '" +
                id->name +
                "' is not declared."
            );

            return "";
        }

        if (!symbol->initialized)
        {
            errors.push_back(
                "Variable '" +
                id->name +
                "' is not initialized."
            );
        }

        return symbol->type;
    }


    // Binary expression
    if (BinaryExpr* b =
        dynamic_cast<BinaryExpr*>(node))
    {
        string left =
            expressionType(b->left);

        string right =
            expressionType(b->right);

        if (left == "" || right == "")
            return "";

        // If either side is real,
        // result is real.
        if (left == "বাস্তব" ||
            right == "বাস্তব")
        {
            return "বাস্তব";
        }

        return "সংখ্যা";
    }

    return "";
}


// =====================================
// STATEMENT ANALYSIS
// =====================================

void SemanticAnalyzer::analyzeStatement(Stmt* node)
{
    // =================================
    // DECLARATION
    // =================================

    if (DeclarationStmt* d =
        dynamic_cast<DeclarationStmt*>(node))
    {
        cout
            << "\nChecking declaration: "
            << d->name
            << endl;

        cout
            << "Declared type: "
            << d->type
            << endl;

        string error;

        bool ok =
            table.define(
                d->name,
                d->type,
                d->value != nullptr,
                error
            );

        if (!ok)
        {
            errors.push_back(error);

            cout
                << "Result: ERROR"
                << endl;

            return;
        }

        // Declaration without value
        if (d->value == nullptr)
        {
            cout
                << "Variable declared without initial value."
                << endl;

            cout
                << "Result: OK"
                << endl;

            return;
        }


        // Check expression type
        string valueType =
            expressionType(d->value);

        cout
            << "Expression type: "
            << valueType
            << endl;


        // সংখ্যা cannot receive বাস্তব
        if (d->type == "সংখ্যা" &&
            valueType == "বাস্তব")
        {
            errors.push_back(
                "Type mismatch: '" +
                d->name +
                "' is সংখ্যা but expression is বাস্তব."
            );

            cout
                << "Result: ERROR"
                << endl;
        }
        else
        {
            cout
                << "Type check: OK"
                << endl;

            cout
                << "Result: OK"
                << endl;
        }

        return;
    }


    // =================================
    // ASSIGNMENT
    // =================================

    if (AssignmentStmt* a =
        dynamic_cast<AssignmentStmt*>(node))
    {
        cout
            << "\nChecking assignment: "
            << a->name
            << endl;

        Symbol* symbol =
            table.lookup(a->name);

        if (symbol == nullptr)
        {
            errors.push_back(
                "Variable '" +
                a->name +
                "' is not declared."
            );

            cout
                << "Result: ERROR"
                << endl;

            return;
        }

        string valueType =
            expressionType(a->value);

        cout
            << "Variable type: "
            << symbol->type
            << endl;

        cout
            << "Expression type: "
            << valueType
            << endl;


        if (symbol->type == "সংখ্যা" &&
            valueType == "বাস্তব")
        {
            errors.push_back(
                "Type mismatch: '" +
                a->name +
                "' is সংখ্যা but expression is বাস্তব."
            );

            cout
                << "Type check: ERROR"
                << endl;
        }
        else
        {
            symbol->initialized = true;

            cout
                << "Type check: OK"
                << endl;

            cout
                << "Result: OK"
                << endl;
        }

        return;
    }


    // =================================
    // PRINT
    // =================================

    if (PrintStmt* p =
        dynamic_cast<PrintStmt*>(node))
    {
        cout
            << "\nChecking print expression"
            << endl;

        string type =
            expressionType(p->value);

        if (type != "")
        {
            cout
                << "Expression type: "
                << type
                << endl;

            cout
                << "Result: OK"
                << endl;
        }
        else
        {
            cout
                << "Result: ERROR"
                << endl;
        }

        return;
    }


    // =================================
    // IF
    // =================================

    if (IfStmt* i =
        dynamic_cast<IfStmt*>(node))
    {
        cout
            << "\nChecking IF condition"
            << endl;

        string leftType =
            expressionType(i->left);

        string rightType =
            expressionType(i->right);

        cout
            << "Left expression type: "
            << leftType
            << endl;

        cout
            << "Right expression type: "
            << rightType
            << endl;

        if (leftType != "" &&
            rightType != "")
        {
            cout
                << "Condition type check: OK"
                << endl;
        }
        else
        {
            cout
                << "Condition type check: ERROR"
                << endl;
        }


        // Analyze THEN block
        for (Stmt* s : i->thenPart)
        {
            analyzeStatement(s);
        }


        // Analyze ELSE block
        for (Stmt* s : i->elsePart)
        {
            analyzeStatement(s);
        }

        return;
    }


    // =================================
    // WHILE
    // =================================

    if (WhileStmt* w =
        dynamic_cast<WhileStmt*>(node))
    {
        cout
            << "\nChecking WHILE condition"
            << endl;

        string leftType =
            expressionType(w->left);

        string rightType =
            expressionType(w->right);

        cout
            << "Left expression type: "
            << leftType
            << endl;

        cout
            << "Right expression type: "
            << rightType
            << endl;

        if (leftType != "" &&
            rightType != "")
        {
            cout
                << "Condition type check: OK"
                << endl;
        }
        else
        {
            cout
                << "Condition type check: ERROR"
                << endl;
        }


        // Analyze WHILE body
        for (Stmt* s : w->body)
        {
            analyzeStatement(s);
        }

        return;
    }
}


// =====================================
// ANALYZE PROGRAM
// =====================================

bool SemanticAnalyzer::analyze(
    Program* program)
{
    cout
        << "\nStarting semantic analysis..."
        << endl;

    for (Stmt* s : program->statements)
    {
        analyzeStatement(s);
    }

    if (errors.empty())
    {
        cout
            << "\nAll semantic checks passed."
            << endl;

        return true;
    }

    cout
        << "\nSemantic analysis found "
        << errors.size()
        << " error(s)."
        << endl;

    return false;
}


// =====================================
// PRINT ERRORS
// =====================================

void SemanticAnalyzer::printErrors()
{
    cout
        << "\n===== SEMANTIC ERRORS ====="
        << endl;

    for (string error : errors)
    {
        cout
            << "Semantic Error: "
            << error
            << endl;
    }
}


void SemanticAnalyzer::printSymbols()
{
    table.print();
} 