#include "decl.hpp"

void Scope::addSymbol(string name, Type type, int offset, bool is_func = false, vector<string> params = {})
{
    this->symbols.push_back({name, type, offset, is_func, params});

}


void SymTable::addSymbol(string name, Type type, bool is_func = false, vector<string> params = {}) {
    if (checkForSymbol(name)) {
        output::errorDef(yylineno, name);
        exit(0);
    }
    this->vars.insert(name);
    if (this->tables.empty()) {
        Scope scope;
        scope.addSymbol(name, type, 0, is_func, params);
        this->tables.push_back(scope); // TODO: should be refernce?
    } else {
        Scope& scope = this->tables.back();
        int offset = this->offsets.back();
        this->offsets.pop_back();
        scope.addSymbol(name, type, offset + 1, is_func, params);
        this->offsets.push_back(offset + 1);
    }
}

void SymTable::addScope(bool is_loop) {
    int offset = this->offsets.back();
    this->offsets.push_back(offset);
    Scope scope(is_loop);
    this->tables.push_back(scope);
}

void SymTable::removeScope() {
    this->offsets.pop_back();
    Scope& scope = this->tables.back();
    for(auto it = scope.symbols.begin(); it != scope.symbols.end(); it++) {
        this->vars.erase(it->name);
    }
    this->tables.pop_back();
}

bool SymTable::checkForSymbol (string name) {
    return this->vars.find(name) != this->vars.end();
}

Symbol& SymTable::getSymbol (string name) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = table->symbols.begin(); symbol != table->symbols.end(); symbol++){
            if(symbol->name == name)
                return *symbol;
        }
    }
    output::errorUndef(yylineno, name);
    exit(0);
}
bool SymTable::verifyInLoop(bool is_break)
{
    if (!this->tables.back().is_loop) {
        if (is_break) {
            output::errorUnexpectedBreak(yylineno);
        } else {
            output::errorUnexpectedBreak(yylineno);
        }
        exit(0);
    }
}