#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <streambuf>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ASTPrinter.h"
#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"
#include "TAC.h"
#include "CodeGenerator.h"

using namespace std;


// =====================================================
// SEND OUTPUT TO BOTH TERMINAL AND result.txt
// =====================================================

class TeeBuf : public streambuf
{
private:
    streambuf* terminal;
    streambuf* file;

public:
    TeeBuf(streambuf* terminalBuffer, streambuf* fileBuffer)
        : terminal(terminalBuffer), file(fileBuffer)
    {
    }

protected:
    int overflow(int c) override
    {
        if (c == EOF)
            return !EOF;

        if (terminal->sputc(c) == EOF)
            return EOF;

        if (file->sputc(c) == EOF)
            return EOF;

        return c;
    }

    int sync() override
    {
        terminal->pubsync();
        file->pubsync();

        return 0;
    }
};


int main()
{
#ifdef _WIN32
    // Terminal uses UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // =====================================================
    // OPEN result.txt AS UTF-8 TEXT
    // =====================================================

#ifdef _WIN32

    // Open result.txt in binary mode.
    // We will write UTF-8 bytes directly.
    ofstream resultFile("result.txt", ios::binary);

#else

    ofstream resultFile("result.txt");

#endif

    if (!resultFile)
    {
        cout << "Error: Cannot create result.txt" << endl;
        return 1;
    }


    // Save original cout buffer
    streambuf* originalCout = cout.rdbuf();


    // Send cout to both terminal and result.txt
    TeeBuf teeBuffer(originalCout, resultFile.rdbuf());

    cout.rdbuf(&teeBuffer);


    // =====================================================
    // READ test.bangla
    // =====================================================

    ifstream file("test.bangla", ios::binary);

    if (!file)
    {
        cout << "Error: Cannot open test.bangla" << endl;

        cout.rdbuf(originalCout);
        resultFile.close();

        return 1;
    }


    stringstream buffer;
    buffer << file.rdbuf();

    string source = buffer.str();


    // =====================================================
    // BANGLA COMPILER
    // =====================================================

    cout << "===== BANGLA COMPILER =====\n";


    // =====================================================
    // LEXER
    // =====================================================

    Lexer lexer(source);

    vector<Token> tokens = lexer.tokenize();

    cout << "\n===== TOKENS =====\n";

    for (const Token& token : tokens)
    {
        cout << token.line
             << " : "
             << getTokenTypeName(token.type)
             << " -> "
             << token.value
             << endl;
    }


    // =====================================================
    // PARSER
    // =====================================================

    cout << "\n===== PARSER =====\n";

    Parser parser(tokens);

    Program* ast = parser.parse();

    if (parser.hasError())
    {
        cout << "\nCompiler stopped because of syntax errors."
             << endl;

        cout.rdbuf(originalCout);
        resultFile.close();

        return 0;
    }

    cout << "Parsing successful!" << endl;


    // =====================================================
    // AST
    // =====================================================

    cout << "\n===== PARSE TREE (AST) =====\n";

    printAST(ast);


    // =====================================================
    // SEMANTIC ANALYSIS
    // =====================================================

    cout << "\n===== SEMANTIC ANALYSIS =====\n";

    SemanticAnalyzer semantic;

    if (!semantic.analyze(ast))
    {
        semantic.printErrors();
        semantic.printSymbols();

        cout.rdbuf(originalCout);
        resultFile.close();

        return 0;
    }

    cout << "Semantic analysis successful!" << endl;

    semantic.printSymbols();


    // =====================================================
    // THREE ADDRESS CODE
    // =====================================================

    cout << "\n===== THREE ADDRESS CODE =====\n";

    TACGenerator tacGenerator;

    vector<TAC> tac = tacGenerator.generate(ast);

    TACGenerator::print(tac);


    // =====================================================
    // PYTHON CODE GENERATION
    // =====================================================

    cout << "\n===== CODE GENERATION =====\n";

    CodeGenerator generator;

    if (generator.save(ast, "output.py"))
    {
        cout << "Python code generated successfully!"
             << endl;

        cout << "Output file: output.py" << endl;
    }
    else
    {
        cout << "Error creating output.py" << endl;

        cout.rdbuf(originalCout);
        resultFile.close();

        return 1;
    }


    // =====================================================
    // FINISHED
    // =====================================================

    cout << "\n===== COMPILATION FINISHED =====\n";

    cout << "Complete compiler output saved to result.txt"
         << endl;


    // Restore cout
    cout.rdbuf(originalCout);

    resultFile.close();

    return 0;
} 