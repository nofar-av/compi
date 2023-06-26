#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include "nonterminals.hpp"
#include "bp.hpp"

class Exp;
class Statement;
class Call;

class Generator
{
public:
    Generator();
    void emitDeclarations();
    void emitPrintFuncs();
    void generateInitCode();
    string freshVar();
    string freshGlobalVar();
    void binopCode(Exp& result, const string& reg1, const string& op, const string& reg2);
    void relopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2);
    void createSimpleBoolBranch (Exp& exp);
    void bpBoolOp(Exp& result, Exp& exp1, const string& op, Exp& exp2, string& label);
    //void genBoolVar(Statement& stmnt);
    //void genNumVar(Statement& stmnt);
    void genStringVar(Exp& exp);
    // string allocateVar();
    shared_ptr<Exp> genBoolExp(Exp &exp);
    void genStoreVar(string rbp, int offset, string value, const string& type);
    string genLoadVar(string rbp, int offset);
    void genRet(shared_ptr<Exp> exp = nullptr);
    void genLabelAfterIf(Exp* exp);
    void genEndOfFunc(string type);
    void genFuncDecl(string name, vector<string> params, string type);
    void genCall(string name, Call& func, vector<shared_ptr<Exp>> params = {});

private:
    int reg_num;
};

#endif