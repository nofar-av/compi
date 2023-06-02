#include "nonterminals.hpp"

extern SymTable symtable;

Exp::Exp(Node *terminal, string type) : Node(terminal->value), type(type) {
    if (DEBUG)
        std::cout << "Exp Node+string " << type << " " << terminal->value << std::endl;
    if (type == "byte") {
        int value = stoi(terminal->value);
        if (value > 255) {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
}

Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type) {}

Exp::Exp(Node *terminal, string op) : type("bool") {
    Exp* exp = dynamic_cast<Exp*>(terminal);
    if (op != "not") {
        //TODO: some error
    }
    if (exp->is_var && !symtable.checkForSymbol(exp->value)) {
        output::errorUndef(yylineno, exp->value);
        exit(0);
    }
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    // this->value = !exp->value;
}
Exp::Exp(Node *terminal1, Node *terminal2, string type, string op) {
    Exp* exp1 = dynamic_cast<Exp*>(terminal1);
    Exp* exp2 = dynamic_cast<Exp*>(terminal2);
    if (exp1->is_var && !symtable.checkForSymbol(exp1->value)) {
        output::errorUndef(yylineno, exp1->value);
        exit(0);
    }
    if (exp2->is_var && !symtable.checkForSymbol(exp2->value)) {
        output::errorUndef(yylineno, exp2->value);
        exit(0);
    }
    if (type == "bool") {
        if (op == "and" || op == "or") {
            if (exp1->type != "bool" || exp2->type != "bool") {
                output::errorMismatch(yylineno);
                exit(0);
            }   
        } else {
            if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
                output::errorMismatch(yylineno);
                exit(0);
            }
        }
        this->type = "bool";
    } else {
        if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = (exp1->type == "int" || exp2->type == "int") ?
            "int" : "byte";
    }
}
Exp::Exp(Node *terminal, Node *type) : Node(terminal->value) {
    Exp* exp = dynamic_cast<Exp*>(terminal);
    Type* new_type = dynamic_cast<Type*>(type);
    if ((exp->type != "int" && exp->type != "byte") || (new_type->value != "int" && new_type->value != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
    }
    this->type = new_type->value;     
}
Exp::Exp(Call *call) : Node(), type(call->type) { }

ExpList::ExpList(Exp *exp) : Node(), exps() {
    this->exps.push_back(make_shared<Exp>(*exp));
}

ExpList::ExpList(Exp *exp, ExpList* explist) : Node() {
    this->exps.push_back(make_shared<Exp>(*exp));
    if(explist != nullptr) {
        exps.insert(exps.end(), explist->exps.begin(), explist->exps.end());
    }
}

Call::Call(string name) : Node(name) {
    if(!symtable.checkForSymbol(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    Symbol& func = symtable.getFunction(name, {});
    this->type = func.type;
}

Call::Call(string name, ExpList *explist) : Node(name) {
    if(!symtable.checkForSymbol(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol& func = symtable.getFunction(name, convertExpToString(explist->exps));
    this->type = func.type;
}

//function parameter
FormalDecl::FormalDecl(Type* type, string name) : Node(name), type(type->type) {}

//function's list of parameters
FormalsList::FormalsList(FormalDecl *formaldecl) : Node(), formals_list() {
    if (formaldecl != nullptr)
        this->formals_list.push_back(make_shared<FormalDecl>(*formaldecl));
}

FormalsList::FormalsList(FormalDecl *formaldecl, FormalsList *f_list) 
        : Node(), formals_list() {
    this->formals_list.push_back(make_shared<FormalDecl>(*formaldecl));
    if(f_list != nullptr) {
        this->formals_list.insert(formals_list.end(), f_list->formals_list.begin(), f_list->formals_list.end());
    }
}
    
Formals::Formals(FormalsList* formals_list) {
    if (formals_list != nullptr) {
        this->params = formals_list->formals_list;
    }
}

vector<string> convertExpToString(vector<shared_ptr<Exp>>& explist)
{
    vector<string> types;
    for (auto it = explist.begin(); it != explist.end(); it++)
    {
        types.push_back((*it)->type);
    }
    return types;
}

vector<string> convertFormalDeclToString(vector<shared_ptr<FormalDecl>>& f_list)
{
    vector<string> types;
    for (auto it = f_list.begin(); it != f_list.end(); it++)
    {
        types.push_back((*it)->type);
    }
    return types;
}

Statement::Statement(string name, string type) : Node() {
    symtable.addSymbol(name, type);
}

Statement::Statement(string name, string ltype, string rtype) : Node() {
    if(ltype != rtype && !(ltype == "INT" && rtype == "BYTE")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    symtable.addSymbol(name, ltype);
}

Statement::Statement(string name, Exp* exp) : Node() {
    Symbol& id = symtable.getSymbol(name);
    if(id.type != exp->type && !(id.type == "INT" && exp->type == "BYTE")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement() : Node() {
    if(symtable.getCurrScopeRetType() != "void") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}
Statement::Statement(Exp *exp) : Node() {
    string ltype = symtable.getCurrScopeRetType();
    string rtype = exp->type;
    if(ltype != rtype && !(ltype == "INT" && rtype == "BYTE")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(string type) : Node() {
    if(type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

FuncDecl::FuncDecl(bool is_override, string type, string name, Formals* formals) : Node() {
    symtable.addFunction(name, type, true, convertFormalDeclToString(formals->params), is_override);
    symtable.addScope(type);
    symtable.addFuncParams(formals->params);
}

Program::Program() : Node() {
    symtable.CheckMain(); // check if program has main, main returns void, main gets 0 args and is not overrided.
}