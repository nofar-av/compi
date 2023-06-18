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

void Generator::binopCode(Exp& result, string& reg2, string op, string& reg2) {
    result.reg = allocator.freshVar();
    op = fanCOPToIROp(op);
    string op_type = fanCTypeToIRString(result.type);
    if (op_type == "div") {
        op_type = (result.type == "int") ? "sdiv" : "udiv";
    }
    buffer.emit(result.reg + "= " + op + op_type + " "+ reg1 + "," + reg2);
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