#include "symtable.hpp"

void to_upper(char &ch) {
    ch = toupper(static_cast<unsigned char>(ch));
}

bool equalsParams(vector<string> func_args, vector<string> params) {
    if (func_args == params) {
        return true;
    } else if (func_args.size() == params.size()) {
        for(int i = 0; i < func_args.size(); i++)
        {
            if(!(params[i] == func_args[i] || (params[i] == "byte" && func_args[i] == "int"))) {
                return false;
            }
        }
        return true;
    }
    return false;
}
string printArgs(vector<string> params)
{
    if (params.empty()) {
        return "";
    }
    string res = "";
    for(auto arg = params.begin(); arg != params.end(); arg++)
    {
        if(arg != params.begin()) {
            res += ",";
        }
        res += *arg;
    } 
    std::for_each(res.begin(), res.end(), to_upper);
    return res;
}

void Scope::addSymbol(string name, string type, int offset, bool is_func, vector<string> params, bool is_override)
{
    //shared_ptr<Symbol> Symbol(name, type, offset, is_func, params, is_override})
    shared_ptr<Symbol> sym = make_shared<Symbol>(name, type, offset, is_func, params, is_override);
    this->symbols.push_back(sym);
}

SymTable::SymTable()
{
    shared_ptr<Scope> scope = make_shared<Scope>();
    scope->addSymbol("print", "void", 0, true, {"string"}, false);
    scope->addSymbol("printi", "void", 0, true, {"int"}, false);
    this->offsets.push_back(0);
    //this->offsets.push_back(0);
    this->tables.push_back(scope); 
}

void SymTable::addSymbol(string name, string type, bool is_func, vector<string> params, bool is_override) {
    if (checkForSymbol(name)) {
        output::errorDef(yylineno, name);
        exit(0);
    }
    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back();
    this->offsets.pop_back();
    scope.addSymbol(name, type, offset, is_func, params, is_override);
    this->offsets.push_back(offset + 1);
}

void SymTable::addFuncParams(vector<shared_ptr<FormalDecl>>& params) {
    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back() - 1;
    for (auto param = params.begin(); param != params.end(); param++, offset--)
    {
        if (checkForSymbol((*param)->value)) {
            output::errorDef(yylineno, (*param)->value);
            exit(0);
        }
        scope.addSymbol((*param)->value, (*param)->type, offset, false, {}, false);
    }
}

void SymTable::addFunction(string name, string type, bool is_func, vector<string> params, bool is_override) {
    if (checkForSymbol(name)) {
        Symbol& orig_func = this->getSymbol(name);
        if(!is_override && !orig_func.is_override) {
            output::errorDef(yylineno, name);
            exit(0);
        }
        if (!orig_func.is_override && is_override) {
            output::errorFuncNoOverride(yylineno, name);
            exit(0);
        }
        if(!is_override && orig_func.is_override) {
            output::errorOverrideWithoutDeclaration(yylineno, name);
            exit(0);
        }
        if(is_override && orig_func.is_override && type == orig_func.type && params == orig_func.params) {
            output::errorDef(yylineno, name);
            exit(0);
        } 
    }

    Scope& scope = *(this->tables.back());
    int offset = this->offsets.back();
    scope.addSymbol(name, type, offset, is_func, params, is_override);
    if (name == "main" && params.empty() && type == "void") {
        this->has_main = true;
        if (is_override == true) {
            output::errorMainOverride(yylineno);
            exit(0);
        }
    }
}

void SymTable::addScope(string return_type) {
    int offset = this->offsets.back();
    this->offsets.push_back(offset);
    shared_ptr<Scope> scope = make_shared<Scope>(return_type);
    this->tables.push_back(scope);
}

void SymTable::addScope(bool is_loop, string return_type) {
    int offset = this->offsets.back();
    this->offsets.push_back(offset);
    is_loop = this->tables.back()->is_loop || is_loop; 
    shared_ptr<Scope> scope = make_shared<Scope>(is_loop, return_type);
    this->tables.push_back(scope);
}

void SymTable::removeScope() {
    this->printScope();
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

bool SymTable::checkForFunction (string name, vector<string> params, string type) {
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = (*table)->symbols.begin(); symbol != (*table)->symbols.end(); symbol++){
            if((*symbol)->name == name && (*symbol)->params == params && (*symbol)->type == type)
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
    shared_ptr<Symbol> func;
    bool func_exist = false;
    for(auto table = this->tables.begin(); table != this->tables.end(); table++) {
        for(auto symbol = (*table)->symbols.begin(); symbol != (*table)->symbols.end(); symbol++){
            if((*symbol)->name == name && !(*symbol)->is_function) {
                output::errorUndefFunc(yylineno, name);
                exit(0);
            }
            if((*symbol)->name == name) {
                func_exist = true;
                if (equalsParams((*symbol)->params, params)) {
                    if (func == nullptr) {
                        func = *symbol;
                    } else {
                        output::errorAmbiguousCall(yylineno, name);
                        exit(0);
                    }
                }
                else if (!(*symbol)->is_override) {
                    output::errorPrototypeMismatch(yylineno, name);
                    exit(0);
                }
                //else - meaning it's overriden so we can keep looking :)
            }
        }
    }
    if (func != nullptr) 
        return *func;
    if(func_exist) {
        output::errorPrototypeMismatch(yylineno, name);
        exit(0);
    }
    output::errorUndefFunc(yylineno, name);
    exit(0);
}

string SymTable::getCurrScopeRetType() {
    return this->tables.back()->getReturnType();
}

void SymTable::verifyInLoop(bool is_break)
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

void SymTable::printScope()
{
    output::endScope();
    shared_ptr<Scope> scope = this->tables.back();
    for(auto symbol = scope->symbols.begin(); symbol != scope->symbols.end(); symbol++)
    {
        std::cout << (*symbol)->name << " ";
        if ((*symbol)->is_function) {
            std::cout << "(" << printArgs((*symbol)->params) << ")" << "->";
            
        }
        string uppercase = (*symbol)->type; 
        std::for_each(uppercase.begin(), uppercase.end(), to_upper);
        std::cout << uppercase << " " << (*symbol)->offset << std::endl;
    }
}

void SymTable::checkMain() {
    if (!this->has_main) {
        output::errorMainMissing();
        exit(0);
    }
}
