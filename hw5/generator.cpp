#include "generator.hpp"

using namespace std;
extern CodeBuffer buffer;

string fanCTypeToIRString(string type) {
    if (type == "bool") {
        return "i1";
    }
    else if (type == "byte") {
        return "i8";
    }
    else if (type == "int") {
        return "i32";
    }
    else if (type == "void") {
        return "void";
    }
    else if (type == "string") {
        return "i8*";
    }
    return ":(";
}

string fanCOPToIROp (string op) {
    if (op == "+") {
        return "add";
    }
    else if (op == "-") {
        return "sub";
    }
    else if (op == "\\") {
        return "div";
    }
    else if (op == "*") {
        return "mul";
    }
    return op;

}

string fanCRelopToIR (string op) {
    if (op == "==") {
        return "eq";
    }
    else if (op == "!=") {
        return "ne";
    }
    else if (op == "<") {
        return "slt";
    }
    else if (op == ">") {
        return "sgt";
    }
    else if (op == "<=") {
        return "sle";
    }
    else if (op == ">=") {
        return "sge";
    }
    return ":(";
}

void Generator::emitDeclarations() {
    buffer.emitGlobal("@.intFormat = internal constant [4 x i8] c\"%d\\0A\\00\"");
    buffer.emitGlobal("@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c\"Error division by zero\\00\"");

    buffer.emitGlobal("declare i32 @printf(i8*, ...)");
    buffer.emitGlobal("declare void @exit(i32)");

    buffer.emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    buffer.emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
}
void Generator::emitPrintFuncs() {
    buffer.emitGlobal("define void @print(i8*){");
    buffer.emitGlobal("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    buffer.emitGlobal("ret void");
    buffer.emitGlobal("}");

    buffer.emitGlobal("define void @printi(i32){");
    buffer.emitGlobal("%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0");
    buffer.emitGlobal("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)");
    buffer.emitGlobal("ret void");
    buffer.emitGlobal("}");
}


Generator::Generator() : reg_num(0) {
}

void Generator::generateInitCode() {
    this->emitDeclarations();
    this->emitPrintFuncs();
}

string Generator::freshVar()
{
    string var = "%var_" + to_string(this->reg_num);
    this->reg_num += 1;
    return var;
}

string Generator::freshGlobalVar()
{
    return "@var_" + to_string(this->reg_num++);
}

void Generator::binopCode(Exp& result, const string& reg1, const string& op, const string& reg2) {
    result.reg = this->freshVar();
    string llvm_op = fanCOPToIROp(op);
    string op_type = fanCTypeToIRString(result.type);
    if (op_type == "div") {
        op_type = (result.type == "int") ? "sdiv" : "udiv";
    }
    buffer.emit(result.reg + " = " + llvm_op + " " + op_type + " "+ reg1 + ", " + reg2);
    if (result.type == "bool") {
        this->createSimpleBoolBranch(result);
    }
}

void Generator::relopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2) {
    result.reg = this->freshVar();
    string llvm_op = fanCRelopToIR(op);
    buffer.emit(result.reg + " = " + "icmp " + llvm_op + " i32 " + exp1.reg + ", " + exp2.reg);
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

void Generator::genBoolVar(Exp& exp) {

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
        result.false_list = buffer.merge(exp1.false_list, exp2.false_list);
    } else if (op == "or") { // op == "or"
        buffer.bpatch(exp1.false_list, label);
        result.true_list = buffer.merge(exp1.true_list, exp2.true_list);
        result.false_list = exp2.false_list;
    }
}

/*void Generator::genBoolVar(Statement& stmnt) {
    stmnt.reg = this->freshVar();
    value = (stmnt.value == "true") ?
    buffer.emit( stmnt.reg + "= i1 add 0, " + );
}*/

// void Generator::genNumVar(Exp& exp) {
//     exp.reg = this->freshVar();
//     string op_type = fanCTypeToIRString(exp.type);
//     buffer.emit( exp.reg + "= " + op_type + " add 0, " + exp.value);
// }

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
    buffer.emit(reg_ptr + " = alloca i32");
    return reg_ptr;
}