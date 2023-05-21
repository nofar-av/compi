#ifndef decl
#define decl

#include "hw3_output.hpp"
#include <stack>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

extern int yylineno;

using namespace std;

enum Type { intType, boolType, byteType, stringType };
struct Symbol {
        string name;
        Type type;
        int offset;
        bool is_function;
        vector<string> params;
};
class Scope {
    public:
        vector<Symbol> symbols;
        bool is_loop;
        Scope(bool is_loop = false) : symbols(), is_loop(is_loop) {};
        ~Scope() = default;
        void addSymbol(string name, Type type, int offset, bool is_func = false, vector<string> params = {});
};

class SymTable {
public:
    set<string> vars;
    vector<Scope> tables;
    vector<int> offsets;
    
    SymTable() = default;
    ~SymTable() = default;
    void addSymbol(string name, Type type, bool is_func = false, vector<string> params = {});
    void addScope(bool is_loop);
    void removeScope();
    bool checkForSymbol (string name);
    Symbol& getSymbol (string name);
    bool verifyInLoop(bool is_break);
};

#endif