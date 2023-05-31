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

    virtual ~Program() = default;
};

class Funcs : public Node {
    virtual ~Funcs() = default;
};

class FuncDecl : public Node {
    virtual ~FuncDecl() = default;
};

class OverRide : public Node {
    virtual ~OverRide() = default;
};

class RetType : public Node {
    string type;
    RetType(string type) : Node(), type(type) {};
    virtual ~RetType() = default;
};

class Formals : public Node {
    virtual ~Formals() = default;
};

class FormalsList : public Node {
    virtual ~FormalsList() = default;
};

class FormalDecl : public Node {
    virtual ~FormalDecl() = default;
};

class Statements : public Node {
    virtual ~Statements() = default;
};

class Statement : public Node {
    virtual ~Statement() = default;
};

class Call : public Node {
public:
    string type;
    Call(Node *id);
    Call(Node *id, ExpList *explist);
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