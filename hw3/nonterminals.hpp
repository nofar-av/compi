#ifndef nonterminals
#define nonterminals
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
#include "decl.hpp"

using namespace std;

class Node {
public:
    string value;
    Node(const string value = "") : value(value) {};
    Node(const Node &node): value(node.value) {};
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Program : public Node {
    Program();
    virtual ~Program() = default;
};

class Funcs : public Node {
    virtual ~Funcs() = default;
};

class FuncDecl : public Node {
    FuncDecl(bool is_override, string type, string name, Formals* formals);
    virtual ~FuncDecl() = default;
};

class OverRide : public Node {
    bool is_override;
    OverRide(bool is_override = false) : is_override(is_override) {};
    virtual ~OverRide() = default;
};

class RetType : public Node {
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
    Statements() = default;
    virtual ~Statements() = default;
};

class Statement : public Node {
    Statement(string name, string type);
    Statement(string name, string ltype, string rtype);
    Statement(string name, Exp* exp);
    Statement(); // RETURN SC
    Statement(Exp* exp); // RETURN Exp SC
    Statement(string type);
    virtual ~Statement() = default;
};

class Call : public Node {
public:
    string type;
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
    Exp(Node *node, string type);
    Exp(Exp *exp);
    Exp(Node *terminal, string op);
    Exp(Node *terminal1, Node *terminal2, string type, string op);
    Exp(Node *terminal, Node *type);
    Exp(Call *call);
    virtual ~Exp() = default;

};

#endif