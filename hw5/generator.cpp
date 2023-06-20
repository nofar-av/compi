#include "generator.hpp"

using namespace std;
extern CodeBuffer buffer;

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

std::string Generator::freshVar()
{
    return "%var_" + to_string(reg_num++);
}

std::string Generator::freshGlobalVar()
{
    return "@var_" + to_string(reg_num++);
}

void Generator::binopCode(Exp& result, const string& reg1, const string& op, const string& reg2) {
    result.reg = this->freshVar();
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

void Generator::relopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2) {
    result.reg = this->freshVar();
    string llvm_op = fanCRelopToIR(op);
    buffer.emit(result.reg + "= " + "icmp" + llvm_op + "i32"  +" "+ exp1.reg + "," + exp2.reg);
    //is this needed? can do without the branch?
    int address = buffer.emit("br i1 " + result.reg + ", label @, label @");

    result.true_list = buffer.makelist({ address, FIRST });
    result.false_list = buffer.makelist({ address, SECOND });
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

void Generator::bpBoolOp(Exp& result, Exp& exp1, const string& op, Exp& exp2, string& label) {
    
    if (op == "and") {
        buffer.bpatch(exp1.true_list, label);
        result.true_list = exp2.true_list;
        result.false_list = buffer.merge(exp1.false_list, exp2.false_list)
    } else if (op == "or") { // op == "or"
        buffer.bpatch(exp1.false_list, label);
        result.true_list = buffer.merge(exp1.true_list, exp2.true_list);
        result.false_list = exp2.false_list;
    }
}

void Generator::genBoolVar(Statement& stmnt) {
    stmnt.reg = this->freshVar();
    value = (stmnt.value == "true") ?
    buffer.emit( stmnt.reg + "= i1 add 0, " + );
}

void Generator::genNumVar(Exp& exp) {
    exp.reg = this->freshVar();
    string op_type = fanCTypeToIRString(exp.type);
    buffer.emit( exp.reg + "= " + op_type + " add 0, " + exp.value);
}

void Generator::genStringVar(Exp& exp) {
    string str = exp.value;
    str.pop_back();
    string reg = this->freshGlobalVar();
    string size_str = "[" + to_string(str.length()) + " x i8]";
    string get_ptr = "getelementptr" + size_str + ", " + size_str + "* " + reg + ", i32 0, i32 0";

    buffer.emitGlobal(reg + " = constant " + size_str + " c" + str + "\\00\"");
    reg.replace(0, 1, "%");
    buffer.emit(reg + ".ptr = " + get_ptr);
    exp.reg = reg + ".ptr";
}

string Generator::allocateVar() {
    string reg_ptr = this->freshVar();
    buffer.emit(reg_ptr + "= alloca i32");
    return reg_ptr;
}