#include <string>
#include "core.h"
#include "bimap.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctype.h>


enum TokenType {
    _err = -1,
    _eof = 0,


    //Keywords
    _if = 1, _else = 2,
    _true = 3, _false = 4,
    _null = 5,
    _and = 6, _or = 7,
    _function = 8,
    _return = 9,
    _var = 10,
    _for = 11, _while = 12,

    //Literals
    _identifier = 13,
    _string = 14,
    _number = 15,

    //One or two character tokens
    _bang = 16, _bang_eq = 17,
    _eq = 18, _eq_eq = 19,
    _greater = 20, _greater_eq = 21,
    _less = 21, less_eq = 22,

    //One character tokens
    _l_paren = 23, _r_paren = 24,
    _l_brace = 25, _r_brace = 25,
    _comma = 26, _dot = 27,
    _minus = 28, _plus = 29, _slash = 30, _asterisk = 31,
    _semicolon = 32
};

typedef Bimap<string, TokenType> LexemesMap;
typedef struct { string content; TokenType id; } ContentfulLexeme;

namespace tk {
    LexemesMap lexemesDef;

    const int populateLexemes = [&] {
        lexemesDef.set("true", _true);
        lexemesDef.set("false", _false);
        lexemesDef.set("null", _null);
        lexemesDef.set("function", _function);
        lexemesDef.set("return", _return);
        lexemesDef.set("var", _var);

        lexemesDef.set("identifier", _identifier);
        lexemesDef.set("string", _string);
        lexemesDef.set("number", _number);

        lexemesDef.set("=", _eq);
        lexemesDef.set("==", _eq_eq);
        lexemesDef.set("!", _bang);
        lexemesDef.set("!=", _bang_eq);

        lexemesDef.set("{", _l_brace);
        lexemesDef.set("}", _r_brace);
        lexemesDef.set("(", _l_paren);
        lexemesDef.set(")", _r_paren);
        lexemesDef.set("-", _minus);
        lexemesDef.set("+", _plus);
        lexemesDef.set(",", _comma);

        lexemesDef.set(";", _semicolon);

        return 0;
    }();


    ContentfulLexeme getContentfulLexeme(string input) {
        ContentfulLexeme error = { "", _err };

        if (lexemesDef.hasKey(input)) {

            return ContentfulLexeme{ input, lexemesDef.valueForKey(input) };
        }
        else {
            TokenType lexeme = _err;

            if (isalpha(input[0])) lexeme = lexemesDef.valueForKey("identifier");
            if (isdigit(input[0])) lexeme = lexemesDef.valueForKey("number");
            if (input[0] == '"') lexeme = lexemesDef.valueForKey("string");

            return ContentfulLexeme{ input, lexeme };
        }

        return error;
    }
}


bool canBeFollowed(char c) {
    return (c == '=' || c == '!');
}

using namespace tk;

std::vector<ContentfulLexeme> tokenizer(string input) {
    int i = input.length();
    char lastChar = '\0';
    char firstChar = '\0';
    bool shouldAppend = false;
    string word;

    std::vector<ContentfulLexeme> tokenVec;

    for (char& c : input) {
        if (word.length() == 0) firstChar = c;

        shouldAppend = false;
        if (isalnum(firstChar) && isalnum(c)) shouldAppend = true;
        if (ispunct(firstChar) && ispunct(c) && (canBeFollowed(firstChar) == word.length() > 0)) shouldAppend = true;
        if (ispunct(firstChar) && ispunct(c) && word.length() == 0) shouldAppend = true;

        if (shouldAppend) {
            word.append(string(1, c));
        }
        else if (word.length()) {
            ContentfulLexeme lex = getContentfulLexeme(word);
            string lexType = lexemesDef.keyForValue(lex.id);
            tokenVec.push_back(lex);

            word = "";
            if (isalnum(c) || ispunct(c)) {
                firstChar = c;
                word.append(string(1, c));
            }
        }

        lastChar = c;
    }

    tokenVec.push_back(ContentfulLexeme{ "", TokenType::_eof });

    return tokenVec;
}




string fileReader(string fileName) {
    ifstream inputFile;
    string temp;
    string text;

    inputFile.open(fileName);
    if (!inputFile.is_open()) return "";

    for (;;) {
        inputFile >> temp;
        if (inputFile.fail()) break;
        text.append(temp + " ");
    }

    inputFile.close();

    return text;
}


std::vector<ContentfulLexeme>::iterator tkit; // short for tokenIterator
static std::map<TokenType, int> binopPrecedence;


class Expression {
public:
    virtual ~Expression() {}
};

class NumberExpression : public Expression {
    double val;

public:
    NumberExpression(double val): val(val) {}
};

class VariableExpression : public Expression {
    std::string name;

public:
    VariableExpression(const std::string &name): name(name) {}
};

class BinaryExpression : public Expression {
    char operation;
    std::unique_ptr<Expression> LHS, RHS;


public:
    BinaryExpression(char op, std::unique_ptr<Expression> LHS, std::unique_ptr<Expression> RHS):
        operation(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExpression : public Expression {
    std::string callee;
    std::vector<std::unique_ptr<Expression>> args;

public:
    CallExpression(const std::string &callee, std::vector<std::unique_ptr<Expression>> args):
        callee(callee), args(std::move(args)) {}
};

class Prototype {
    std::string name;
    std::vector<std::string> args;

public:
    Prototype(const std::string& name, std::vector<std::string> args)
        : name(name), args(std::move(args)) {}

    const std::string& getName() const { return name; }
};

class Function {
    std::unique_ptr<Prototype> proto;
    std::unique_ptr<Expression> body;

public:
    Function(std::unique_ptr<Prototype> Proto,
        std::unique_ptr<Expression> Body)
        : proto(std::move(proto)), body(std::move(body)) {}
};

std::unique_ptr<Expression> LogError(const char* str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

std::unique_ptr<Prototype> LogErrorP(const char* str) {
    LogError(str);
    return nullptr;
}

std::unique_ptr<Expression> parseExpression();

std::unique_ptr<Expression> parseNumberExpression() {
    double value = stod((*tkit).content, nullptr);
    auto result = std::make_unique<NumberExpression>(value);
    ++tkit;
    return std::move(result);
}

std::unique_ptr<Expression> parseParenExpression() {
    ++tkit;
    auto temp = parseExpression();
    if (!temp) return nullptr;

    if ((*tkit).id != TokenType::_r_paren)
        return LogError("expected ')'");
    ++tkit;
    return temp;
}

std::unique_ptr<Expression> parseIdentifierExpression() {
    std::string idName = (*tkit).content;
    ++tkit;

    if ((*tkit).id != TokenType::_r_paren) return std::make_unique<VariableExpression>(idName);

    // if no parenthesis, this is a call
    ++tkit;
    std::vector<std::unique_ptr<Expression>> args;
    if ((*tkit).id != TokenType::_r_paren) {
        while (1) {
            if (auto arg = parseExpression()) args.push_back(std::move(arg));
            else return nullptr;

            if ((*tkit).id == TokenType::_r_paren) break;

            if ((*tkit).id != TokenType::_comma) return LogError("Expected ')' or ',' in argument list");
            
            ++tkit;
        }
    }

    ++tkit;

    return std::make_unique<CallExpression>(idName, std::move(args));
}

std::unique_ptr<Expression> parsePrimary() {
    switch ((*tkit).id) {
    case TokenType::_identifier:
        return parseIdentifierExpression();
    case TokenType::_number:
        return parseNumberExpression();
    case TokenType::_l_paren:
        return parseParenExpression();
    default:
        return LogError("unknown token when expecting an expression");
    }
}

int getTokenPrecedence() {
    int tokPrec = binopPrecedence[(*tkit).id];
    if (tokPrec <= 0) return -1;
    return tokPrec;
}

std::unique_ptr<Expression> parseBinOpRHS(int expressionPrec, std::unique_ptr<Expression> LHS) {
    while (1) {
        int tokenPrec = getTokenPrecedence();

        if (tokenPrec < expressionPrec) return LHS;
        int binOp = (*tkit).id;
        ++tkit;

        auto RHS = parsePrimary();
        if (!RHS) return nullptr;

        int nextPrec = getTokenPrecedence();
        if (tokenPrec < nextPrec) {
            RHS = parseBinOpRHS(tokenPrec + 1, std::move(RHS));
            if (!RHS) return nullptr;
        }

        LHS = std::make_unique<BinaryExpression>(binOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<Expression> parseExpression() {
    auto LHS = parsePrimary();
    if (!LHS) return nullptr;

    return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<Prototype> parsePrototype() {
    if ((*tkit).id != TokenType::_identifier) return LogErrorP("Expected function name in prototype");

    std::string fName = (*tkit).content;
    ++tkit;

    if ((*tkit).id != TokenType::_l_paren) return LogErrorP("Expected '(' in prototype");

    std::vector<std::string> argNames;
    while ((*++tkit).id == TokenType::_identifier) argNames.push_back((*tkit).content);
    if ((*tkit).id != TokenType::_r_paren) return LogErrorP("Expected ')' in prototype");
    ++tkit;

    return std::make_unique<Prototype>(fName, std::move(argNames));
}

std::unique_ptr<Function> parseDefinition() {
    ++tkit;
    auto proto = parsePrototype();
    if (!proto) return nullptr;

    if (auto E = parseExpression()) return std::make_unique<Function>(std::move(proto), std::move(E));
    return nullptr;
}

std::unique_ptr<Prototype> parseExtern() {
    ++tkit;
    return parsePrototype();
}

std::unique_ptr<Function> parseTopLevelExpression() {
    if (auto E = parseExpression()) {
        auto proto = std::make_unique<Prototype>("", std::vector<std::string>());
        return std::make_unique<Function>(std::move(proto), std::move(E));
    }
    return nullptr;
}

static void HandleDefinition() {
    if (parseDefinition()) fprintf(stderr, "Parsed a function definition.\n");
    else ++tkit;
}

static void HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (parseTopLevelExpression()) fprintf(stderr, "Parsed a top-level expr\n");
    else ++tkit;
}

void mainLoop() {
    while (1) {
        switch ((*tkit).id) {
        case TokenType::_eof:
            return;
        case TokenType::_semicolon:
            ++tkit;
            break;
        case TokenType::_function:
            HandleDefinition();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

int main() {

    string sourceCode = fileReader("test.txt");
    std::vector<ContentfulLexeme> tokens = tokenizer(sourceCode);
    tkit = tokens.begin();

    binopPrecedence[TokenType::_less] = 10;
    binopPrecedence[TokenType::_plus] = 20;
    binopPrecedence[TokenType::_minus] = 20;
    binopPrecedence[TokenType::_asterisk] = 40;

    mainLoop();
}
