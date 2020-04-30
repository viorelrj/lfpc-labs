#pragma once
#ifndef TOKEN_H
#define TOKEN_H
#include "core.h"
#include "bimap.h"

#include <vector>

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


std::vector<ContentfulLexeme> tokenizer(string input);

#endif