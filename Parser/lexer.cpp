#include <iostream>
#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include<fstream>
#include <assert.h>
#include <memory>

// Lexer

using namespace std;
template<typename Base, typename T>
inline bool instanceof(const T*) {
    return is_base_of<Base, T>::value;
} int x=-1;

ifstream fin("code.txt");
struct struckTok
{
    int id;
    string name;
    string value;
};
vector<struckTok> arrayOfStruct;
struct struckTok tmpStructTok;
enum Token {
    tok_eof        = -1,
    tok_function   = -2,
    tok_extern     = -3,
    tok_identifier = -4,
    tok_number     = -5,
    tok_if         = -6,
    tok_else       = -7,
    tok_for        = -8,
    tok_from       = -9,
    tok_to         = -10,
    tok_binary     = -11,
    tok_unary      = -12,
    tok_variable   = -13,
    tok_execute =-14
};

static string IdentifierStr;
static double NumVal;
string anotherOne;
static int gettok() {
    static int LastChar = ' ';

    while (isspace(LastChar))
        LastChar = fin.get();
    if (isalpha(LastChar) || LastChar=='#') {
        IdentifierStr = LastChar;
        while (isalnum((LastChar = fin.get()))|| LastChar=='_')
            IdentifierStr += LastChar;
        tmpStructTok.value=IdentifierStr;
        if (IdentifierStr == "function")
            return tok_function;
        if (IdentifierStr == "extern")
            return tok_extern;
        if (IdentifierStr == "if")
            return tok_if;
        if (IdentifierStr == "else")
            return tok_else;
        if (IdentifierStr == "for")
            return tok_for;
        if (IdentifierStr == "variable")
            return tok_variable;
        if (IdentifierStr == "from")
            return tok_from;
        if (IdentifierStr == "to")
            return tok_to;
        if (IdentifierStr == "execute")
            return tok_execute;
        return tok_identifier;
    }
    if (isdigit(LastChar)) {
        string NumStr;
        do {
            NumStr += LastChar;
            LastChar = fin.get();
        } while (isdigit(LastChar) || LastChar == '.');
        tmpStructTok.value=NumStr;
        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }
    anotherOne=LastChar;
    tmpStructTok.value=anotherOne;

    if (LastChar == '%') {
        do
            LastChar = fin.get();
        while (LastChar != '\0' && LastChar != '\n' && LastChar != '\r');

        if (LastChar != '\0')
            return gettok();
    }

    if (LastChar == EOF)
        return tok_eof;

    int ThisChar = LastChar;
    LastChar = fin.get();
    return ThisChar;
}