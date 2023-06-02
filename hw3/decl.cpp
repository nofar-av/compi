#include "decl.hpp"

void Scope::addSymbol(string name, string type, int offset, bool is_func = false, vector<string> params = {}, bool is_override = false)
{
    //shared_ptr<Symbol> Symbol(name, type, offset, is_func, params, is_override})
    shared_ptr<Symbol> sym = make_shared<Symbol>(name, type, offset, is_func, params, is_override);
    this->symbols.push_back(sym);
}

SymTable::SymTable()
{
    shared_ptr<Scope> scope = make_shared<Scope>();
    scope->addSymbol("print", "void", 0, true, {"STRING"}, false);
    scope->addSymbol("printi", "void", 0, true, {"INT"}, false);
    this->tables.push_back(scope); 
}

void SymTable::addSymbol(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false) {
    if (checkForSymbol(name)) {
        output::errorDef(yylineno, name);
        exit(0);
    }
    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back();
    this->offsets.pop_back();
    scope.addSymbol(name, type, offset + 1, is_func, params, is_override);
    this->offsets.push_back(offset + 1);
}

void SymTable::addFuncParams(vector<shared_ptr<FormalDecl>>& params) {
    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back() - 1;
    for (auto param = params.begin(); param != params.end(); param++, offset--)
    {
        scope.addSymbol((*param)->value, (*param)->type, offset, false, {}, false);
    }
}

void SymTable::addFunction(string name, string type, bool is_func = false, vector<string> params = {}, bool is_override = false) {
    if (checkForSymbol(name)) {
        if(!is_override) {
            output::errorDef(yylineno, name);
            exit(0);
        } else {
            Symbol& orig_func = getSymbol(name);
            if (!orig_func.is_override) {
                output::errorFuncNoOverride(yylineno, name);
                exit(0);
            }
        }
    }
    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back();
    scope.addSymbol(name, type, offset, is_func, params, is_override);

}

void SymTable::addScope(string return_type) {
    int offset = this->offsets.back();
    this->offsets.push_back(offset);
    shared_ptr<Scope> scope = make_shared<Scope>(return_type);
    this->tables.push_back(scope);
}

void SymTable::addScope(bool is_loop = false) {
    int offset = this->offsets.back();
    this->offsets.push_back(offset);
    shared_ptr<Scope> scope = make_shared<Scope>(is_loop);
    this->tables.push_back(scope);
}

void SymTable::removeScope() {
    this->offsets.pop_back();
    // Scope& scope = *(this->tables.back());
    this->tables.pop_back();
}

bool SymTable::checkForSymbol (string name) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = (*table)->symbols.begin(); symbol != (*table)->symbols.end(); symbol++){
            if((*symbol)->name == name)
                return true;
        }
    }
    return false;
}

Symbol& SymTable::getSymbol (string name) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = (*table)->symbols.begin(); symbol != (*table)->symbols.end(); symbol++){
            if((*symbol)->name == name)
                return **symbol;
        }
    }
    output::errorUndef(yylineno, name);
    exit(0);
}

Symbol& SymTable::getFunction (string name, vector<string> params) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = (*table)->symbols.begin(); symbol != (*table)->symbols.end(); symbol++){
            if((*symbol)->name == name) {
                if ((*symbol)->is_function && params == (*symbol)->params)
                    return **symbol;
                else if (!(*symbol)->is_function) {
                    output::errorUndef(yylineno, name);
                    exit(0);
                } else if (!(*symbol)->is_override) {
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

string SymTable::getCurrScopeRetType() {
    return this->tables.back()->getReturnType();
}

bool SymTable::verifyInLoop(bool is_break)
{
    if (!this->tables.back()->is_loop) {
        if (is_break) {
            output::errorUnexpectedBreak(yylineno);
        } else {
            output::errorUnexpectedContinue(yylineno);
        }
        exit(0);
    }
}
void SymTable::printTable()
{
    for(auto scope = this->tables.begin(); scope != this->tables.end(); scope++)
    {
        for(auto symbol = (*scope)->symbols.begin(); symbol != (*scope)->symbols.end(); symbol++)
        {
            std::cout << (*symbol)->name << " ";
            if ((*symbol)->is_function) {
                std::cout << "(" << printArgs((*symbol)->params) << ")" << "->";
                
            }
            std::cout << (*symbol)->type << " " << (*symbol)->offset << std::endl;
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