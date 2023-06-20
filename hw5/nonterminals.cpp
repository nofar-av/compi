#include "nonterminals.hpp"

extern SymTable symtable;
extern Generator generator;
extern CodeBuffer buffer;

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

Exp::Exp(Node *terminal, string type) : Node(terminal->value), type(type) {
    if (type == "byte") {
        int value = stoi(terminal->value);
        if (value > 255) {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
    
    if (type == "byte" || type == "int") { //if number add 0 to num
        generator.binopCode(*this, terminal->value, "+", "0");
    } else if (type == "bool") {
        generator.createSimpleBoolBranch(*this);
    } else { //type is string
        generator.genStringVar(*this);
    }
}

Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type) {}

Exp::Exp(string value) : Node(value) , type() {
    Symbol &sym = symtable.getSymbol(value);
    if (sym.is_function) {
        output::errorUndef(yylineno, value);
        exit(0);
    }
    this->is_var = true;
    this->type = sym.type;
}

Exp::Exp(Node *terminal, string type, string op) : type(type) {
    Exp* exp = dynamic_cast<Exp*>(terminal);
    // expecting type = "bool" , op = "not"
    if (exp->is_var && !symtable.checkForSymbol(exp->value)) {
        output::errorUndef(yylineno, exp->value);
        exit(0);
    }
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    //  this->value = ()!exp->value;
    generator.binopCode(*this, exp->reg, "xor", "1");
    this->true_list = exp->false_list;
    this->false_list = exp->true_list;
    this->next_list = exp->next_list;
}


Exp::Exp(Node *terminal1, Node *terminal2, string type, string op, string label) {
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
            generator.bpBoolOp(*this,exp1, op, exp2, label);   
            generator.binopCode(*this, exp1->reg, op, exp2->reg);
        } else {
            if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
                output::errorMismatch(yylineno);
                exit(0);
            }
            generator.relopCode(*this, exp1, op, exp2);
        }
        this->type = "bool";
    } else {
        if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = (exp1->type == "int" || exp2->type == "int") ?
            "int" : "byte";
        generator.binopCode(*this, exp1->reg, op, exp2->reg);
    }
    // this->value = allocator.freshVar();
}


Exp::Exp(Node *terminal, Node *type) : Node(terminal->value) {
    Exp* exp = dynamic_cast<Exp*>(terminal);
    Type* new_type = dynamic_cast<Type*>(type);
    if ((exp->type != "int" && exp->type != "byte") || (new_type->type != "int" && new_type->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
    }
    this->type = new_type->type;     
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
FormalDecl::FormalDecl(Type* type, string name) : Node(name), type(type->type) {
    if(symtable.checkForSymbol(name)) {
        output::errorDef(yylineno, name);
        exit(0);
    }
}

//function's list of parameters
FormalsList::FormalsList(FormalDecl *formaldecl) : Node(), formals_list() {
    if (formaldecl != nullptr)
        this->formals_list.push_back(make_shared<FormalDecl>(*formaldecl));
}

FormalsList::FormalsList(FormalDecl *formaldecl, FormalsList *f_list) 
        : Node(), formals_list() {
    for(auto formal : f_list->formals_list) {
        if (formal->value == formaldecl->value) {
            output::errorDef(yylineno, formaldecl->value);
            exit(0);
        }
    }

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

Statement::Statement(string name, string type) : Node() {
    symtable.addSymbol(name, type);
    /*shared_ptr<Exp> exp = make_shared<Exp>(new Exp());
    if (type == "bool") {
        exp->value = "false";
        exp->type = "bool";
        generator.createSimpleBoolBranch(*exp);
    } else if (type == "int" || type == "byte") {
        exp->value = "0";
        exp->type = type;
        buffer.genNumVar(*exp);
    } else { //string
        exp->value = "";
        exp->type = "string";
        buffer.genString(*exp);
    }*/
}

Statement::Statement(string name, string ltype, Exp* rexp) : Node() {
    if(ltype != rexp->type && !(ltype == "int" && rexp->type == "byte")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    symtable.addSymbol(name, ltype, rexp->reg); // check about int byte extension
    /*if (ltype == "bool") {
        // genBoolExp()
        exp->value = "false";
        exp->type = "bool";
        generator.createSimpleBoolBranch(*exp);
    } else if (type == "int" || type == "byte") {
       buffer.genNumVar(*rexp);
    } else {
        buffer.genString(*rexp);
    }*/
}

Statement::Statement(string name, Exp* exp) : Node() {
    Symbol& id = symtable.getSymbol(name);
    if (id.is_function) {
        output::errorUnDef(yylineno, name);
        exit(0);
    }
    if(id.type != exp->type && !(id.type == "int" && exp->type == "byte")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    string reg_ptr = buffer.freshVar();
    buffer.emit(reg_ptr + " = add i32 " + symtable.getCurrScopeRbp() + ", " + id.offset);
    buffer.emit("store i32 " + exp->reg + ", i32* " + reg_ptr);
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
    if(ltype != rtype && !(ltype == "int" && rtype == "byte")) {
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
    symtable.checkMain(); // check if program has main, main returns void, main gets 0 args and is not overrided.
    symtable.removeScope();
}