#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser(const vector<Token>& tokens)
{
    this->tokens = tokens;
    position = 0;
    error = false;
}

Token Parser::current()
{
    if (position < tokens.size())
        return tokens[position];

    return {
        TokKind::END_OF_FILE,
        "EOF",
        0
    };
}

void Parser::advance()
{
    if (position < tokens.size() - 1)
        position++;
}

bool Parser::check(TokKind type)
{
    return current().type == type;
}

bool Parser::match(TokKind type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

bool Parser::expect(
    TokKind type,
    string message)
{
    if (check(type))
    {
        advance();
        return true;
    }

    cout << "Syntax Error at line "
         << current().line
         << ": "
         << message
         << endl;

    error = true;

    return false;
}


// =====================================
// ERROR RECOVERY
// =====================================

void Parser::recover()
{
    while (!check(TokKind::SEMICOLON) &&
           !check(TokKind::RIGHT_BRACE) &&
           !check(TokKind::END) &&
           !check(TokKind::END_OF_FILE))
    {
        advance();
    }

    if (check(TokKind::SEMICOLON))
        advance();
}


// =====================================
// PARSE
// =====================================

Program* Parser::parse()
{
    Program* result = program();

    if (!error)
        cout << "Parsing successful!" << endl;
    else
        cout << "Parsing failed!" << endl;

    return result;
}

bool Parser::hasError()
{
    return error;
}


// =====================================
// PROGRAM
// =====================================

Program* Parser::program()
{
    Program* p = new Program();

    expect(
        TokKind::START,
        "Program must start with 'শুরু'."
    );

    while (!check(TokKind::END) &&
           !check(TokKind::END_OF_FILE))
    {
        p->statements.push_back(
            statement()
        );
    }

    expect(
        TokKind::END,
        "Program must end with 'শেষ'."
    );

    return p;
}


// =====================================
// STATEMENT
// =====================================

Stmt* Parser::statement()
{
    if (check(TokKind::DECLARE))
        return declaration();

    if (check(TokKind::IDENTIFIER))
        return assignment();

    if (check(TokKind::PRINT))
        return printStatement();

    if (check(TokKind::IF))
        return ifStatement();

    if (check(TokKind::WHILE))
        return whileStatement();

    cout << "Syntax Error at line "
         << current().line
         << ": Unexpected token '"
         << current().value
         << "'"
         << endl;

    error = true;

    recover();

    // Safe node so compiler does not crash
    return new PrintStmt(
        new NumberExpr("0", false)
    );
}


// =====================================
// DECLARATION
// =====================================

Stmt* Parser::declaration()
{
    advance(); // ধরো

    string type;

    if (match(TokKind::INT_TYPE))
        type = "সংখ্যা";

    else if (match(TokKind::REAL_TYPE))
        type = "বাস্তব";

    else
    {
        cout << "Syntax Error at line "
             << current().line
             << ": Expected 'সংখ্যা' or 'বাস্তব'."
             << endl;

        error = true;
        recover();

        return new PrintStmt(
            new NumberExpr("0", false)
        );
    }

    if (!expect(
        TokKind::IDENTIFIER,
        "Expected variable name."))
    {
        recover();

        return new PrintStmt(
            new NumberExpr("0", false)
        );
    }

    string name =
        tokens[position - 1].value;

    Expr* value = nullptr;

    if (match(TokKind::ASSIGN))
    {
        value = expression();
    }

    expect(
        TokKind::SEMICOLON,
        "Expected ';' after declaration."
    );

    return new DeclarationStmt(
        type,
        name,
        value
    );
}


// =====================================
// ASSIGNMENT
// =====================================

Stmt* Parser::assignment()
{
    string name = current().value;

    advance();

    expect(
        TokKind::ASSIGN,
        "Expected '=' in assignment."
    );

    Expr* value = expression();

    expect(
        TokKind::SEMICOLON,
        "Expected ';' after assignment."
    );

    return new AssignmentStmt(
        name,
        value
    );
}


// =====================================
// PRINT
// =====================================

Stmt* Parser::printStatement()
{
    advance(); // দেখাও

    expect(
        TokKind::LEFT_PAREN,
        "Expected '('."
    );

    Expr* value = expression();

    expect(
        TokKind::RIGHT_PAREN,
        "Expected ')'."
    );

    expect(
        TokKind::SEMICOLON,
        "Expected ';'."
    );

    return new PrintStmt(value);
}


// =====================================
// CONDITION
// =====================================

Parser::Condition Parser::condition()
{
    Expr* left = expression();

    string op;

    if (match(TokKind::EQUAL))
        op = "==";

    else if (match(TokKind::NOT_EQUAL))
        op = "!=";

    else if (match(TokKind::LESS))
        op = "<";

    else if (match(TokKind::GREATER))
        op = ">";

    else if (match(TokKind::LESS_EQUAL))
        op = "<=";

    else if (match(TokKind::GREATER_EQUAL))
        op = ">=";

    else
    {
        cout << "Syntax Error at line "
             << current().line
             << ": Expected comparison operator."
             << endl;

        error = true;
        op = "==";
    }

    Expr* right = expression();

    return {left, op, right};
}


// =====================================
// IF
// =====================================

Stmt* Parser::ifStatement()
{
    advance(); // যদি

    expect(
        TokKind::LEFT_PAREN,
        "Expected '(' after 'যদি'."
    );

    Condition c = condition();

    expect(
        TokKind::RIGHT_PAREN,
        "Expected ')'."
    );

    IfStmt* node =
        new IfStmt(
            c.left,
            c.op,
            c.right
        );

    block(node->thenPart);

    if (match(TokKind::ELSE))
    {
        block(node->elsePart);
    }

    return node;
}


// =====================================
// WHILE
// =====================================

Stmt* Parser::whileStatement()
{
    advance(); // যতক্ষণ

    expect(
        TokKind::LEFT_PAREN,
        "Expected '(' after 'যতক্ষণ'."
    );

    Condition c = condition();

    expect(
        TokKind::RIGHT_PAREN,
        "Expected ')'."
    );

    WhileStmt* node =
        new WhileStmt(
            c.left,
            c.op,
            c.right
        );

    block(node->body);

    return node;
}


// =====================================
// BLOCK
// =====================================

void Parser::block(vector<Stmt*>& statements)
{
    expect(
        TokKind::LEFT_BRACE,
        "Expected '{'."
    );

    while (!check(TokKind::RIGHT_BRACE) &&
           !check(TokKind::END) &&
           !check(TokKind::END_OF_FILE))
    {
        statements.push_back(
            statement()
        );
    }

    expect(
        TokKind::RIGHT_BRACE,
        "Expected '}'."
    );
}


// =====================================
// EXPRESSION
//
// expression → term { (+ | -) term }
// =====================================

Expr* Parser::expression()
{
    Expr* left = term();

    while (check(TokKind::PLUS) ||
           check(TokKind::MINUS))
    {
        string op = current().value;

        advance();

        Expr* right = term();

        left =
            new BinaryExpr(
                left,
                op,
                right
            );
    }

    return left;
}


// =====================================
// TERM
//
// term → factor { (* | /) factor }
// =====================================

Expr* Parser::term()
{
    Expr* left = factor();

    while (check(TokKind::MULTIPLY) ||
           check(TokKind::DIVIDE))
    {
        string op = current().value;

        advance();

        Expr* right = factor();

        left =
            new BinaryExpr(
                left,
                op,
                right
            );
    }

    return left;
}


// =====================================
// FACTOR
//
// factor → number
//         | identifier
//         | ( expression )
// =====================================

Expr* Parser::factor()
{
    if (check(TokKind::INTEGER))
    {
        string value = current().value;

        advance();

        return new NumberExpr(
            value,
            false
        );
    }

    if (check(TokKind::DECIMAL))
    {
        string value = current().value;

        advance();

        return new NumberExpr(
            value,
            true
        );
    }

    if (check(TokKind::IDENTIFIER))
    {
        string name = current().value;

        advance();

        return new IdExpr(name);
    }

    if (match(TokKind::LEFT_PAREN))
    {
        Expr* e = expression();

        expect(
            TokKind::RIGHT_PAREN,
            "Expected ')'."
        );

        return e;
    }

    cout << "Syntax Error at line "
         << current().line
         << ": Expected number, variable, or expression."
         << endl;

    error = true;

    advance();

    return new NumberExpr("0", false);
} 