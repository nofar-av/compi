#ifndef nonterminals
#define nonterminals


#include "hw3_output.hpp"
#include <stack>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
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
    virtual ~Call() = default;
};

class ExpList : public Node {
    virtual ~ExpList() = default;
};

class Type : public Node {
    Type(string type) : Node(type) {};
    virtual ~Type() = default;
};

class Exp : public Node {
    Exp(Node *node, string type);
    virtual ~Exp() = default;

};

#endif