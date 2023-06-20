#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include "nonterminals.hpp"
#include "bp.hpp"

class Generator
{
public:
    Generator();
    std::string freshVar();
    std::string freshGlobalVar();
    void binopCode(Exp& result, const string& reg1, const string& op, const string& reg2);
    void relopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2);
    void createSimpleBoolBranch (Exp& exp);
    void bpBoolOp(Exp& result, Exp& exp1, const string& op, Exp& exp2, string& label);
    void genBoolVar(Statement& stmnt);
    void genNumVar(Statement& stmnt);
    void genStringVar(Exp& exp);
    string Generator::allocateVar();

private:
    int reg_num;
};

#endif