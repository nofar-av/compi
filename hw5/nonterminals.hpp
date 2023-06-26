#ifndef NONTERMINALS_HPP
#define NONTERMINALS_HPP
#define DEBUG 1

#include "hw3_output.hpp"
#include <stack>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include "symtable.hpp"
#include "generator.hpp"
#include "bp.hpp"

using namespace std;

// enum BranchLabelIndex {FIRST, SECOND};
typedef vector<std::pair<int, BranchLabelIndex>> bp_list;

class Formals;
class FormalsList;
class FormalDecl;
class ExpList;
class Exp;
class Type;
class Statement;
class Call;

class Node {
public:
    string value;
    Node(const string value = "") : value(value) {};
    Node(const Node &node): value(node.value) {};
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Label : public Node {
public:
    Label();
    virtual ~Label() = default;
};

class Program : public Node {
public:
    Program();
    virtual ~Program() = default;
};

class Funcs : public Node {
public:
    Funcs() = default;
    virtual ~Funcs() = default;
};

class FuncDecl : public Node {
public:
    FuncDecl(bool is_override, string type, string name, Formals* formals);
    virtual ~FuncDecl() = default;
};

class OverRide : public Node {
public:    
    bool is_override;
    OverRide(bool is_override = false) : is_override(is_override) {};
    virtual ~OverRide() = default;
};

class RetType : public Node {
public:
    string type;
    RetType(string type) : Node(), type(type) {};
    virtual ~RetType() = default;
};

class Formals : public Node {
public:
    vector<shared_ptr<FormalDecl>> params;
    Formals() = default;
    Formals(FormalsList* formals_list);
    virtual ~Formals() = default;
};

class FormalsList : public Node {
public:
    vector<shared_ptr<FormalDecl>> formals_list;
    FormalsList() = default;
    FormalsList(FormalDecl *formaldecl);
    FormalsList(FormalDecl *formaldecl, FormalsList *formals_list);
    //FormalsList(FormalsList other); //TODO:: needed for formals
    virtual ~FormalsList() = default;
};

class FormalDecl : public Node {
public:
    string type;
    FormalDecl(Type* type, string name);
    virtual ~FormalDecl() = default;
};

class Statements : public Node {
public:
    bp_list cont_list;
    bp_list break_list;
    Statements(Statement* statement);
    Statements(Statements* stmnts, Statement* statement);
    virtual ~Statements() = default;
};

class Statement : public Node {
public:
    bp_list cont_list;
    bp_list break_list;
    Statement(string name, string type);
    Statement(string name, string ltype, Exp* rexp);
    Statement(string name, Exp* exp);
    Statement(Statements* statements);
    Statement(Call* func);
    Statement();
    Statement(Exp* exp);
    Statement(bool is_break);
    Statement(Label* cond_label, Label* code_label, Exp* exp, Statement* code);
    Statement(Exp* exp, Label* true_label, Statement* true_code);
    Statement(Statement* true_code, Statement* false_code, Exp* exp, Label* true_label, Label* false_label);
    virtual ~Statement() = default;
};

class Call : public Node {
public:
    string reg;
    string type;
    bp_list true_list;
    bp_list false_list;
    //bp_list next_list;
    Call(string name);
    Call(string name, ExpList *explist);
    virtual ~Call() = default;
};

class ExpList : public Node {
public:
    vector<shared_ptr<Exp>> exps;
    ExpList() = default;
    ExpList(Exp *exp);
    ExpList(Exp *exp, ExpList *explist);
    virtual ~ExpList() = default;
};

class Type : public Node {
public:
    string type;
    Type(string type) : Node(), type(type) {};
    virtual ~Type() = default;
};

class Exp : public Node {
public:    
    string type;
    bool is_var = false;
    string reg;
    bp_list true_list;
    bp_list false_list;
    bp_list next_list;
    Exp() = default;
    Exp(Node *node, string type);
    Exp(Exp *exp);
    Exp(Node *terminal, string type, string op);
    Exp(Node *terminal1, Node *terminal2, string type, string op, string label = "");
    Exp(Node *terminal, Node *type);
    Exp(Call *call);
    Exp(string value);
    virtual ~Exp() = default;

};

#endif // NONTERMINALS_HPP