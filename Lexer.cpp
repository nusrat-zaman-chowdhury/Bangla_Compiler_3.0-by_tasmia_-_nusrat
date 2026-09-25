#include "Lexer.h"

#include <cctype>

using namespace std;

Lexer::Lexer(const string& source)
{
    this->source = source;
    position = 0;
    line = 1;
}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    while (position < source.length())
    {
        char c = source[position];

        if (c == ' ' || c == '\t' || c == '\r')
        {
            position++;
            continue;
        }

        if (c == '\n')
        {
            line++;
            position++;
            continue;
        }

        // Bangla words
        if ((unsigned char)c >= 128)
        {
            size_t start = position;

            while (position < source.length())
            {
                unsigned char x = source[position];

                if (x < 128)
                    break;

                position++;
            }

            string word = source.substr(start, position - start);

            if (word == "শুরু")
                tokens.push_back({TokKind::START, word, line});
            else if (word == "শেষ")
                tokens.push_back({TokKind::END, word, line});
            else if (word == "ধরো")
                tokens.push_back({TokKind::DECLARE, word, line});
            else if (word == "সংখ্যা")
                tokens.push_back({TokKind::INT_TYPE, word, line});
            else if (word == "বাস্তব")
                tokens.push_back({TokKind::REAL_TYPE, word, line});
            else if (word == "যদি")
                tokens.push_back({TokKind::IF, word, line});
            else if (word == "নাহলে")
                tokens.push_back({TokKind::ELSE, word, line});
            else if (word == "যতক্ষণ")
                tokens.push_back({TokKind::WHILE, word, line});
            else if (word == "দেখাও")
                tokens.push_back({TokKind::PRINT, word, line});
            else
                tokens.push_back({TokKind::IDENTIFIER, word, line});

            continue;
        }

        // Numbers
        if (isdigit((unsigned char)c))
        {
            size_t start = position;
            bool decimal = false;

            while (position < source.length() &&
                   isdigit((unsigned char)source[position]))
            {
                position++;
            }

            if (position < source.length() &&
                source[position] == '.')
            {
                decimal = true;
                position++;

                while (position < source.length() &&
                       isdigit((unsigned char)source[position]))
                {
                    position++;
                }
            }

            string number =
                source.substr(start, position - start);

            if (decimal)
                tokens.push_back(
                    {TokKind::DECIMAL, number, line});
            else
                tokens.push_back(
                    {TokKind::INTEGER, number, line});

            continue;
        }

        // Operators
        if (c == '+')
        {
            tokens.push_back({TokKind::PLUS, "+", line});
            position++;
        }
        else if (c == '-')
        {
            tokens.push_back({TokKind::MINUS, "-", line});
            position++;
        }
        else if (c == '*')
        {
            tokens.push_back({TokKind::MULTIPLY, "*", line});
            position++;
        }
        else if (c == '/')
        {
            tokens.push_back({TokKind::DIVIDE, "/", line});
            position++;
        }
        else if (c == '=')
        {
            if (position + 1 < source.length() &&
                source[position + 1] == '=')
            {
                tokens.push_back(
                    {TokKind::EQUAL, "==", line});
                position += 2;
            }
            else
            {
                tokens.push_back(
                    {TokKind::ASSIGN, "=", line});
                position++;
            }
        }
        else if (c == '!')
        {
            if (position + 1 < source.length() &&
                source[position + 1] == '=')
            {
                tokens.push_back(
                    {TokKind::NOT_EQUAL, "!=", line});
                position += 2;
            }
            else
            {
                tokens.push_back(
                    {TokKind::UNKNOWN, "!", line});
                position++;
            }
        }
        else if (c == '<')
        {
            if (position + 1 < source.length() &&
                source[position + 1] == '=')
            {
                tokens.push_back(
                    {TokKind::LESS_EQUAL, "<=", line});
                position += 2;
            }
            else
            {
                tokens.push_back(
                    {TokKind::LESS, "<", line});
                position++;
            }
        }
        else if (c == '>')
        {
            if (position + 1 < source.length() &&
                source[position + 1] == '=')
            {
                tokens.push_back(
                    {TokKind::GREATER_EQUAL, ">=", line});
                position += 2;
            }
            else
            {
                tokens.push_back(
                    {TokKind::GREATER, ">", line});
                position++;
            }
        }

        // Symbols
        else if (c == '(')
        {
            tokens.push_back(
                {TokKind::LEFT_PAREN, "(", line});
            position++;
        }
        else if (c == ')')
        {
            tokens.push_back(
                {TokKind::RIGHT_PAREN, ")", line});
            position++;
        }
        else if (c == '{')
        {
            tokens.push_back(
                {TokKind::LEFT_BRACE, "{", line});
            position++;
        }
        else if (c == '}')
        {
            tokens.push_back(
                {TokKind::RIGHT_BRACE, "}", line});
            position++;
        }
        else if (c == ';')
        {
            tokens.push_back(
                {TokKind::SEMICOLON, ";", line});
            position++;
        }
        else
        {
            tokens.push_back(
                {TokKind::UNKNOWN, string(1, c), line});
            position++;
        }
    }

    tokens.push_back(
        {TokKind::END_OF_FILE, "", line});

    return tokens;
}

string getTokenTypeName(TokKind type)
{
    switch (type)
    {
        case TokKind::START: return "START";
        case TokKind::END: return "END";
        case TokKind::DECLARE: return "DECLARE";
        case TokKind::INT_TYPE: return "INT_TYPE";
        case TokKind::REAL_TYPE: return "REAL_TYPE";
        case TokKind::IF: return "IF";
        case TokKind::ELSE: return "ELSE";
        case TokKind::WHILE: return "WHILE";
        case TokKind::PRINT: return "PRINT";

        case TokKind::IDENTIFIER: return "IDENTIFIER";
        case TokKind::INTEGER: return "INTEGER";
        case TokKind::DECIMAL: return "DECIMAL";

        case TokKind::PLUS: return "PLUS";
        case TokKind::MINUS: return "MINUS";
        case TokKind::MULTIPLY: return "MULTIPLY";
        case TokKind::DIVIDE: return "DIVIDE";

        case TokKind::ASSIGN: return "ASSIGN";
        case TokKind::EQUAL: return "EQUAL";
        case TokKind::NOT_EQUAL: return "NOT_EQUAL";
        case TokKind::LESS: return "LESS";
        case TokKind::GREATER: return "GREATER";
        case TokKind::LESS_EQUAL: return "LESS_EQUAL";
        case TokKind::GREATER_EQUAL: return "GREATER_EQUAL";

        case TokKind::LEFT_PAREN: return "LEFT_PAREN";
        case TokKind::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokKind::LEFT_BRACE: return "LEFT_BRACE";
        case TokKind::RIGHT_BRACE: return "RIGHT_BRACE";

        case TokKind::SEMICOLON: return "SEMICOLON";
        case TokKind::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
} 