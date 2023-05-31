#include "decl.hpp"

void Scope::addSymbol(string name, string type, int offset, bool is_func = false, vector<string> params = {}, bool is_override = false)
{
    this->symbols.push_back({name, type, offset, is_func, params, is_override});

}

SymTable::SymTable()
{
    Scope scope;
    //scope.addSymbol("print", "void", 0, true, );
    this->tables.push_back(scope); // TODO: should be refernce?
}

void SymTable::addSymbol(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false) {
    if (checkForSymbol(name)) {
        output::errorDef(yylineno, name);
        exit(0);
    }
    this->vars.insert(name);
    Scope& scope = this->tables.back();
    int offset = this->offsets.back();
    this->offsets.pop_back();
    scope.addSymbol(name, type, offset + 1, is_func, params, is_override);
    this->offsets.push_back(offset + 1);
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

Symbol& getFunction (string name, vector<string> params) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = table->symbols.begin(); symbol != table->symbols.end(); symbol++){
            if(symbol->name == name) {
                if (symbol->is_function && params == symbol->params)
                    return *symbol;
                else if (!symbol->is_function) {
                    output::errorUndef(yylineno, name);
                    exit(0);
                } else if (!symbol->is_override) {
                    output::errorPrototypeMismatch(yylineno, name);
                    exit(0);
                }
                //else - meaning it's overriden so we can keep looking :)
            }
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
void SymTable::printTable()
{
    for(auto scope = this->tables.begin(); scope != this->tables.end(); scope++)
    {
        for(auto symbol = scope->symbols.begin(); symbol != scope->symbols.end(); symbol++)
        {
            std::cout << symbol->name << " ";
            if (symbol->is_function) {
                std::cout << "(" << printArgs(symbol->params) << ")" << "->";
                
            }
            std::cout << symbol->type << " " << symbol->offset << std::endl;
        }
        std::cout << "---end scope---" << std::endl;
    }
}
string printArgs(vector<string> params)
{
    if (params.empty()) {
        return "VOID";
    }
    string res = "";
    for(auto arg = params.begin(); arg != params.end(); arg++)
    {
        if(arg != params.begin()) {
            res += ", ";
        }
        res += *arg;
    }
    return res;
}