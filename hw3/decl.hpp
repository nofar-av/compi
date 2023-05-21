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


using namespace std;

enum Type { intType, boolType, byteType, stringType };

class SymTable {
private:
    struct Entry {
        string name;
        Type type;
        int offset;
    };
    set<string, Type> vars;
    stack<vector<Entry>> tables;
    stack<int> offsets;
    
public:
    SymTable() = default;
    ~SymTable() = default;
    void addSymbol(string name, Type type);
    void addScope();
    void removeScope();
    bool checkForSymbol (string name);
    Type SymTable::findType (string name);
};

#endif