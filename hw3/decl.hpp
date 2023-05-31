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


struct Symbol {
        string name;
        string type;
        int offset;
        bool is_function;
        vector<string> params;
        bool is_override;
};
class Scope {
    public:
        vector<Symbol> symbols;
        bool is_loop;
        Scope(bool is_loop = false) : symbols(), is_loop(is_loop) {};
        ~Scope() = default;
        void addSymbol(string name, string type, int offset, bool is_func = false, vector<string> params = {}, bool is_override = false);
};

class SymTable {
public:
    set<string> vars;
    vector<Scope> tables;
    vector<int> offsets;
    
    SymTable();
    ~SymTable() = default;
    void addSymbol(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false);
    void addScope(bool is_loop);
    void removeScope();
    bool checkForSymbol (string name);
    Symbol& getSymbol (string name);
    bool verifyInLoop(bool is_break);
    void printTable();
};

#endif