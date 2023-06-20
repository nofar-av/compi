#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>

class Generator
{
public:
    Generator();
    std::string fresh_var();

    void binopCode(Exp& result, string& reg2, string op, string& reg2);
    void relopCode(Exp& result, Exp& exp1, string op, Exp& exp2);
    void createSimpleBoolBranch (Exp& exp);
    void bpBoolOp(Exp& result, Exp& exp1, string op, Exp& exp2, string& label);
    void genBoolVar(Statement& stmnt);
    void genNumVar(Statement& stmnt);
    string Generator::allocateVar();

private:
    int reg_num;
};

#endif