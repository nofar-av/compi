#include "decl.hpp"

void SymTable::addSymbol(string name, Type type) {
    if (this->vars.find(name) != this->vars.end()) {
        output::errorDef(yylineno, name);
        exit(0);
    }
    this->vars.insert(name);
    if (this->tables.empty()) {
        vector<Entry> table;
        table.push_back({name, type, 0});
        this->tables.push(table); // TODO: should be refernce?
    } else {
        vector<Entry> table = this->tables.top();
        int offset = this->offsets.top();
        this->offsets.pop();
        table.push_back({name, type, offset + 1});
        this->offsets.push(offset + 1);
    }
}

void SymTable::addScope() {
    int offset = this->offsets.top();
    this->offsets.push(offset);
    vector<Entry> table;
    this->tables.push(table);
}

void SymTable::removeScope() {
    this->offsets.pop();
    vector<Entry> scope = this->tables.top();
    for(auto it = scope.begin(); it != scope.end(); it++) {
        this->vars.erase(it->name);
    }
    this->tables.pop();
}

bool SymTable::checkForSymbol (string name) {
    return this->vars.find(name) != this->vars.end();
}

Type SymTable::findType (string name) {
    
}