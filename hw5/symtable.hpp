#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

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

class FormalDecl;

class Symbol {
    public:
        string name;
        string type;
        int offset;
        bool is_function;
        vector<string> params;
        bool is_override;
        Symbol(string name, string type, int offset, bool is_function, vector<string> params, bool is_override) : name(name), type(type), offset(offset), 
                                                                                                                  is_function(is_function), params(params), is_override(is_override) {};
        string getPtr();
        ~Symbol() = default;
};

class Scope {
public:
    string rbp;
    vector<shared_ptr<Symbol>> symbols;
    bool is_loop;
    string return_type;
    Scope(string return_type = "void");
    Scope(bool is_loop, string return_type);
    string getReturnType() { return this->return_type; };
    ~Scope() = default;
    void addSymbol(string name, string type, int offset, bool is_func = false, vector<string> params = {}, bool is_override = false);
};

class SymTable {
public:
    bool has_main = false;
    vector<shared_ptr<Scope>> tables;
    vector<int> offsets;
    
    SymTable();
    ~SymTable() = default;
    void addSymbol(string name, string type, string value = "0");
    void addFunction(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false);
    void addScope(string return_type); // only for functions
    void addScope(bool is_loop, string return_type);
    void addFuncParams(vector<shared_ptr<FormalDecl>>& params);
    void removeScope();
    bool checkForSymbol (string name);
    bool checkForFunction (string name, vector<string> params, string type);
    Symbol& getSymbol (string name);
    Symbol& getFunction (string name, vector<string> params);
    string getCurrScopeRetType();
    string getCurrScopeRbp();
    void verifyInLoop(bool is_break);
    void printScope();
    void checkMain();
};

#endif // SYMBOLTABLE_HPP