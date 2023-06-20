#include "generator.hpp"

using namespace std;

string fanCTypeToIRString(string type) {
    switch (type) {
    case "bool":
        return "i1";
    case "byte":
        return "i8";
    case "int":
        return "i32";
    case "void":
        return "void";
    case "string":
        return "i8*";
    default:
        return ":(";
    };
}

string fanCOPToIROp (string op) {
    switch (op) {
    case "+":
        return "add";
    case "-":
        return "sub";
    case "*":
        return "mul";
    case "\\":
        return "div";
    default:
        return op;
    };
}

string fanCRelopToIR (string op) {
    switch(op) {
        case "==":
            return "eq";
        case "!=":
            return "ne";
        case "<":
            return "slt";
        case ">":
            return "sgt";
        case "<=":
            return "sle";
        case ">=":
            return "sge";
        default:
            return ":(";
    };
}

Generator::Generator() : reg_num(0) {}

std::string Generator::fresh_var()
{
    return "%var_" + to_string(reg_num++);
}

void Generator::binopCode(Exp& result, string& reg1, string op, string& reg2) {
    result.reg = allocator.freshVar();
    op = fanCOPToIROp(op);
    string op_type = fanCTypeToIRString(result.type);
    if (op_type == "div") {
        op_type = (result.type == "int") ? "sdiv" : "udiv";
    }
    buffer.emit(result.reg + "= " + op + op_type + " "+ reg1 + "," + reg2);
    if (result.type == "bool") {
        this->createSimpleBoolBranch(result);
    }
}

void Generator::relopCode(Exp& result, Exp& exp1, string op, Exp& exp2) {
    result.reg = allocator.freshVar();
    string llvm_op = fanCRelopToIR(op);
    buffer.emit(result.reg + "= " + "icmp" + llvm_op + "i32"  +" "+ exp1.reg + "," + exp2.reg);
    //is this needed? can do without the branch?
    int address = buffer.emit("br i1 " + res->reg + ", label @, label @");
    res->true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
    res->false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
}

void Generator::createSimpleBoolBranch (Exp& exp) {
    int address = buffer.emit("br label @");
        if (exp.value == "true") {
            exp.true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
        } else {
            exp.false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
        }
}

// void Generator::createBoolBranch (Exp& exp) {
//     int address = buffer.emit("br label @");
//     exp.true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
//     buffer.emit(", label @");
//     exp.false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
// }

void Generator::bpBoolOp(Exp& result, Exp& exp1, string op, Exp& exp2, string& label) {
    
    if (op == "and") {
        buffer.bp(exp1.true_list, label);
        result.true_list = exp2.true_list;
        result.false_list = buffer.merge(exp1.false_list, exp2.false_list)
    } else if (op == "or") { // op == "or"
        buffer.bp(exp1.false_list, label);
        result.true_list = buffer.merge(exp1.true_list, exp2.true_list);
        result.false_list = exp2.false_list;
    }
}

void Generator::genBoolVar(Statement& stmnt) {
    stmnt.reg = allocator.freshVar();
    value = (stmnt.value == "true") ?
    buffre.emit( stmnt.reg + "= i1 add 0, " + );
}

void Generator::genNumVar(Exp& exp) {
    exp.reg = allocator.freshVar();
    string op_type = fanCTypeToIRString(result.type);
    buffre.emit( exp.reg + "= " + op_type + " add 0, " + exp.value);
}

string Generator::allocateVar() {
    string reg_ptr = allocator.freshVar();
    buffer.emit(reg_ptr + "= alloca i32");
    return reg_ptr;
}