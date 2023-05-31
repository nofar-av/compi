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
        if(exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
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
Exp::Exp(Call *call) : Node(), type(call->type) {

}

ExpList::ExpList(Exp *exp) : Node(), exps() {
    this->exps.push_back(make_shared<Exp>(*exp));
}

ExpList::ExpList(Exp *exp, ExpList* explist) : Node() {
    this->exps.push_back(make_shared<Exp>(*exp));
    if(explist != nullptr) {
        exps.insert(exps.end(), explist->exps.begin(), explist->exps.end());
    }
}

Call::Call(Node *id) : Node(id->value) {
    if(!symtable.checkForSymbol(id->value)) {
        output::errorUndefFunc(yylineno, id->value);
        exit(0);
    }
    Symbol func = symtable.getSymbol(id->value);
    if(!func.is_function) {
        output::errorUndefFunc(yylineno, id->value);
        exit(0);       
    }
    //TODO: handle override
    if(!func.params.empty()) {
        output::errorPrototypeMismatch(yylineno, id->value);
        exit(0);       
    }
    this->type = func.type;
}
Call::Call(Node *id, ExpList *explist) : Node(id->value) {
    if(!symtable.checkForSymbol(id->value)) {
        output::errorUndefFunc(yylineno, id->value);
        exit(0);
    }
    Symbol func = symtable.getSymbol(id->value);
    if(!func.is_function) {
        output::errorUndefFunc(yylineno, id->value);
        exit(0);       
    }
    //TODO: handle override
}