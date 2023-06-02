#ifndef decl
#define decl

#include "hw3_output.hpp"
#include "nonterminals.hpp"
#include <stack>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <memory>
extern int yylineno;

using namespace std;


class Symbol {
    public:
        string name;
        string type;
        int offset;
        bool is_function;
        vector<string> params;
        bool is_override;
};

class Scope {
    public:
        vector<shared_ptr<Symbol>> symbols;
        bool is_loop;
        string return_type = "void";
        Scope(string return_type) : symbols() , is_loop(false), return_type(return_type) {};
        Scope(bool is_loop = false) : symbols(), is_loop(is_loop) {};
        string getReturnType() { return this->return_type; };
        ~Scope() = default;
        void addSymbol(string name, string type, int offset, bool is_func = false, vector<string> params = {}, bool is_override = false);
};

class SymTable {
public:
    vector<shared_ptr<Scope>> tables;
    vector<int> offsets;
    
    SymTable();
    ~SymTable() = default;
    void addSymbol(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false);
    void addFunction(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false);
    void addScope(string return_type);
    void addScope(bool is_loop = false);
    void addFuncParams(vector<shared_ptr<FormalDecl>>& params);
    void removeScope();
    bool checkForSymbol (string name);
    Symbol& getSymbol (string name);
    Symbol& getFunction (string name, vector<string> params);
    string getCurrScopeRetType();
    bool verifyInLoop(bool is_break);
    void printTable();
};

#endif