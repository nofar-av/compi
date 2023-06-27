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

Label::Label() {
    this->value = buffer.genLabel();
    buffer.emit("br label %" + this->value);
    buffer.emit(this->value + ":");
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
        generator.genConstVar(*this, terminal->value);
    } else if (type == "bool") {
        generator.createSimpleBoolBranch(*this);
    } else { //type is string
        generator.genStringVar(*this);
    }
}

Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type), reg(exp->reg), false_list(exp->false_list),
true_list(exp->true_list) , next_list(exp->next_list) {
} 

//EXP -> ID
Exp::Exp(string value) : Node(value) , type() {
    Symbol &sym = symtable.getSymbol(value);
    if (sym.is_function) {
        output::errorUndef(yylineno, value);
        exit(0);
    }
    this->is_var = true;
    this->type = sym.type;

    if (sym.offset < 0) {
        this->reg = "%" + to_string(((-1) * sym.offset - 1));
    } else {
        this->reg = generator.genLoadVar(symtable.getCurrScopeRbp(), sym.offset, sym.type);
    }

    if (sym.type == "bool") {
        string cmp_reg = generator.freshVar();
        buffer.emit(cmp_reg + " = icmp ne i1 0, " + this->reg);
        int address = buffer.emit("br i1 " + cmp_reg + ", label @, label @");
        this->true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
        this->false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
    }
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
    //generator.binopCode(*this, exp->reg, "xor", "1");
    this->true_list = bp_list(exp->false_list);
    this->false_list = bp_list(exp->true_list);
    this->next_list = bp_list(exp->next_list);
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
            generator.bpBoolOp(*this, *exp1, op, *exp2, label);   
            //generator.binopCode(*this, exp1->reg, op, exp2->reg);
        } else {
            if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
                output::errorMismatch(yylineno);
                exit(0);
            }
            generator.relopCode(*this, *exp1, op, *exp2);
        }
        this->type = "bool";
    } else {
        if ((exp1->type != "int" && exp1->type != "byte") || (exp2->type != "int" && exp2->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = (exp1->type == "int" || exp2->type == "int") ?
            "int" : "byte";
        generator.binopCode(*this, *exp1, op, *exp2);
    }
    // this->value = allocator.freshVar();
}

// EXP -> LPAREN Type RPAREN Exp
Exp::Exp(Node *terminal, Node *type) : Node(terminal->value) {
    Exp* exp = dynamic_cast<Exp*>(terminal);
    Type* new_type = dynamic_cast<Type*>(type);
    if ((exp->type != "int" && exp->type != "byte") || (new_type->type != "int" && new_type->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
    }
    this->type = new_type->type;
    if (exp->type == new_type->type) {
        this->reg = exp->reg;        
    }
    else {
        generator.genConversion(*this, *exp);
    }
    this->false_list = bp_list(exp->false_list);
    this->true_list = bp_list(exp->true_list);     
    this->next_list = bp_list(exp->next_list);
}

Exp::Exp(Call *call) : Node(), type(call->type), reg(call->reg), false_list(call->false_list),
true_list(call->true_list) , next_list() { 

}

ExpList::ExpList(Exp *exp) : Node(), exps() {
    shared_ptr<Exp> new_exp = generator.genBoolExp(*exp);
    this->exps.push_back(new_exp);
}

ExpList::ExpList(Exp *exp, ExpList* explist) : Node() {
    this->exps.push_back(make_shared<Exp>(*exp));
    if(explist != nullptr) {
        this->exps.insert(this->exps.end(), explist->exps.begin(), explist->exps.end());
    }
}

Call::Call(string name) : Node(name) {
    if(!symtable.checkForSymbol(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    Symbol& func = symtable.getFunction(name, {});
    this->type = func.type;
    
    generator.genCall(name, *this, func);
}

Call::Call(string name, ExpList *explist) : Node(name) {
    if(!symtable.checkForSymbol(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol& func = symtable.getFunction(name, convertExpToString(explist->exps));
    this->type = func.type;

    generator.genCall(name, *this, func, explist->exps);
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

//Statement -> Type ID SC
Statement::Statement(string name, string type) : Node() {
    int offset = symtable.addSymbol(name, type);
    Exp exp = Exp();
    // if (type == "bool") { 
    //     exp.value = "false";
    //     exp.type = "bool";
    //     generator.createSimpleBoolBranch(exp);
    // }
    
    generator.genStoreVar(symtable.getCurrScopeRbp(), offset, "0", type);
}

//Statement -> Type ID = EXP SC
Statement::Statement(string name, string ltype, Exp* rexp) : Node() {
    if(ltype != rexp->type && !(ltype == "int" && rexp->type == "byte")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    int offset = symtable.addSymbol(name, ltype); // check about int byte extension
    string reg = rexp->reg;
    if (ltype == "bool") {
        shared_ptr<Exp> temp = generator.genBoolExp(*rexp);
        reg = temp->reg;
    }

    generator.genStoreVar(symtable.getCurrScopeRbp(), offset, reg, ltype, rexp->type);
}

//Statement -> ID = EXP SC
Statement::Statement(string name, Exp* exp) : Node() {
    Symbol& id = symtable.getSymbol(name);
    if (id.is_function) {
        output::errorUndef(yylineno, name);
        exit(0);
    }
    if(id.type != exp->type && !(id.type == "int" && exp->type == "byte")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    string reg = exp->reg;
    if (id.type == "bool") {
        shared_ptr<Exp> temp = generator.genBoolExp(*exp);
        reg = temp->reg;
    }

    generator.genStoreVar(symtable.getCurrScopeRbp(), id.offset, reg, id.type, exp->type);
}

// Statement -> { Statements }
Statement::Statement(Statements* statements) : Node() {
    this->break_list = bp_list(statements->break_list);
    this->cont_list = bp_list(statements->cont_list);
}

Statement::Statement(Call* func) : Node(), break_list(), cont_list() {
    string end_label = buffer.genLabel();
    buffer.emit("br label %" + end_label);
    buffer.emit(end_label + ":");
    buffer.bpatch(func->false_list, end_label);
    buffer.bpatch(func->true_list, end_label);
}

// Statement ->  RETURN SC
Statement::Statement() : Node() {
    if(symtable.getCurrScopeRetType() != "void") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    generator.genRet();
}

// Statement -> RETURN EXP SC
Statement::Statement(Exp *exp) : Node() {
    string ltype = symtable.getCurrScopeRetType();
    string rtype = exp->type;
    if(ltype != rtype && !(ltype == "int" && rtype == "byte")) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if(exp->type == "bool") { 
        shared_ptr<Exp> new_exp = generator.genBoolExp(*exp);
        generator.genRet(new_exp);
    } else {
        generator.genRet(make_shared<Exp>(*exp));
        // if(!exp->value.empty()){
        //     Symbol* symbol = tables.get_symbol(exp->value);
        //     if(symbol){
        //         // Function call
        //         code_gen.return_value(exp->type, exp->reg);
        //     } else {
        //         code_gen.return_value(exp->type, exp->value);
        //     }
        // } else {
        //     code_gen.return_value(exp->type, exp->reg);
        // }
    }
}

//Statement-> BREAK, Statement-> CONTINUE 
Statement::Statement(bool is_break) : Node() {
    int address = buffer.emit("br label @");
    if (is_break) {
        this->break_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
    } else {
        this->cont_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
    }
}

// Statement -> WHILE
Statement::Statement(Label* cond_label, Label* code_label, Exp* exp, Statement* code) : Node() {
    buffer.emit("br label %" + cond_label->value);
    string end_label = buffer.genLabel();
    buffer.emit(end_label + ":");
    buffer.bpatch(code->cont_list, cond_label->value);
    buffer.bpatch(exp->true_list, code_label->value);
    buffer.bpatch(exp->false_list, end_label);
    buffer.bpatch(exp->next_list, end_label);
    buffer.bpatch(code->break_list, end_label);    
    
}

// Statement -> IF EXP statement
Statement::Statement(Exp* exp, Label* true_label, Statement* true_code) : Node() {
    string end_label = buffer.genLabel();
    buffer.bpatch(exp->true_list, true_label->value);
    buffer.bpatch(exp->false_list, end_label);
    buffer.bpatch(exp->next_list, end_label);    
    
    this->break_list = bp_list(true_code->break_list);
    this->cont_list = bp_list(true_code->cont_list);

    buffer.emit("br label %" + end_label);
    buffer.emit(end_label + ":");
}

// Statement -> IF EXP statement ELSE statement 
Statement::Statement(Statement* true_code, Statement* false_code, Exp* exp, Label* true_label, Label* false_label) : Node() {
    string end_label = buffer.genLabel();
    buffer.bpatch(exp->true_list, true_label->value);
    buffer.bpatch(exp->false_list, false_label->value);
    buffer.bpatch(exp->next_list, end_label);

    this->break_list = buffer.merge(true_code->break_list, false_code->break_list);
    this->cont_list = buffer.merge(true_code->cont_list, false_code->cont_list);

    buffer.emit("br label %" + end_label);
    buffer.emit(end_label + ":");
}

Statements::Statements(Statement* statement) : Node() {
    this->break_list = bp_list(statement->break_list);
    this->cont_list = bp_list(statement->cont_list);
}
Statements::Statements(Statements* stmnts, Statement* statement) : Node() {
    this->break_list = buffer.merge(statement->break_list, stmnts->break_list);
    this->cont_list = buffer.merge(statement->cont_list, stmnts->cont_list);
}

FuncDecl::FuncDecl(bool is_override, string type, string name, Formals* formals) : Node() {
    vector<string> params = convertFormalDeclToString(formals->params);
    symtable.addFunction(name, type, params, is_override);
    symtable.addScope(type);
    symtable.addFuncParams(formals->params);

    generator.genFuncDecl(name, params, type); // TODO:: Handle OverRide !!
    buffer.emit(symtable.getCurrScopeRbp() + " = alloca i32, i32 50");
}

Program::Program() : Node() {
    symtable.checkMain(); // check if program has main, main returns void, main gets 0 args and is not overrided.
    symtable.removeScope();
    buffer.printGlobalBuffer();
    buffer.printCodeBuffer();
}