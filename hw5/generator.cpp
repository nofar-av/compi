#include "generator.hpp"

using namespace std;
extern CodeBuffer buffer;
extern SymTable symtable;

string fanCTypeToIRString(string type) { // TODO:: maybe we need everything as i32 and i8*? (Section 3 in pdf)
    if (type == "bool") {
        return "i1";
    }
    else if (type == "byte") {
        return "i8";
    }
    else if (type == "int") {
        return "i32";
    }
    else if (type == "string") {
        return "i8*";
    }
    return type;
}

string fanCOPToIROp (string op) {
    if (op == "+") {
        return "add";
    }
    else if (op == "-") {
        return "sub";
    }
    else if (op == "/") {
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
    buffer.emitGlobal("define void @print_string(i8*) {");
    buffer.emitGlobal("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    buffer.emitGlobal("ret void");
    buffer.emitGlobal("}");

    buffer.emitGlobal("define void @printi_int(i32) {");
    buffer.emitGlobal("%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0");
    buffer.emitGlobal("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)");
    buffer.emitGlobal("ret void");
    buffer.emitGlobal("}");

    buffer.emitGlobal("define void @check_div_error(i32) {");
    buffer.emitGlobal("%valid = icmp eq i32 %0, 0");
    buffer.emitGlobal("br i1 %valid, label %ILLEGAL, label %LEGAL");
    buffer.emitGlobal("ILLEGAL:");
    buffer.emitGlobal("call void @print_string(i8* getelementptr([23 x i8], [23 x i8]* @.DIVIDE_BY_ZERO.str, i32 0, i32 0))");
    buffer.emitGlobal("call void @exit(i32 0)");
    buffer.emitGlobal("ret void");
    buffer.emitGlobal("LEGAL:");
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

void Generator::genConstVar(Exp& result, string value) {
    result.reg = this->freshVar();
    buffer.emit(result.reg + " = add " + fanCTypeToIRString(result.type) + " 0, " + value);
}

void Generator::binopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2) {
    result.reg = this->freshVar();
    string llvm_op = fanCOPToIROp(op);
    string op_type = fanCTypeToIRString(result.type);
    if (llvm_op == "div") {
        llvm_op = (result.type == "int") ? "sdiv" : "udiv";
        string temp = exp2.reg;
        if (exp2.type == "byte") {
            temp = this->freshVar();
            buffer.emit(temp + " = zext i8 " + exp2.reg + " to i32");
        }
        buffer.emit("call void @check_div_error(i32 " + temp + ")");
    }

    string reg1 = exp1.reg;
    string reg2 = exp2.reg;
    if (exp1.type == "byte" && exp2.type == "byte") {
        op_type = "i8";
    } else {
        op_type = "i32";
        if (exp1.type == "byte") {
            reg1 = this->freshVar();
            buffer.emit(reg1 + " = zext i8 " + exp1.reg + " to i32");
        }
        if (exp2.type == "byte") {
            reg2 = this->freshVar();
            buffer.emit(reg2 + " = zext i8 " + exp2.reg + " to i32");
        }
    }
    
    buffer.emit(result.reg + " = " + llvm_op + " " + op_type + " " + reg1 + ", " + reg2);
    if (result.type == "bool") {
        this->createSimpleBoolBranch(result);
    }
}

void Generator::relopCode(Exp& result, Exp& exp1, const string& op, Exp& exp2) {
    result.reg = this->freshVar();
    string llvm_op = fanCRelopToIR(op);
    string llvm_type = "i32";
    string reg1 = exp1.reg;
    string reg2 = exp2.reg;
    if (exp1.type == "byte" && exp2.type == "byte") {
        llvm_type = "i8";
    } else {
        llvm_type = "i32";
        if (exp1.type == "byte") {
            reg1 = this->freshVar();
            buffer.emit(reg1 + " = zext i8 " + exp1.reg + " to i32");
        }
        if (exp2.type == "byte") {
            reg2 = this->freshVar();
            buffer.emit(reg2 + " = zext i8 " + exp2.reg + " to i32");
        }
    }
    buffer.emit(result.reg + " = icmp " + llvm_op + " " + llvm_type + " " + reg1 + ", " + reg2);
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

// void Generator::genBoolVar(Exp& exp) {

// }

// void Generator::createBoolBranch (Exp& exp) {
//     int address = buffer.emit("br label @");
//     exp.true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
//     buffer.emit(", label @");
//     exp.false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
// }

void Generator::bpBoolOp(Exp& result, Exp& exp1, const string& op, Exp& exp2, string& label) {
    
    if (op == "and") {
        buffer.bpatch(exp1.true_list, label);
        result.true_list = bp_list(exp2.true_list);
        result.false_list = buffer.merge(exp1.false_list, exp2.false_list);
    } else if (op == "or") { // op == "or"
        buffer.bpatch(exp1.false_list, label);
        result.true_list = buffer.merge(exp1.true_list, exp2.true_list);
        result.false_list = bp_list(exp2.false_list);
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
//     buffer.emit(exp.reg + "= " + op_type + " add 0, " + exp.value);
//     this->genStoreVar();
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

// string Generator::allocateVar() {
//     string reg_ptr = this->freshVar();
//     buffer.emit(reg_ptr + " = alloca i32");
//     return reg_ptr;
// }

shared_ptr<Exp> Generator::genBoolExp(Exp &exp) {
    if (exp.type != "bool") {
        return make_shared<Exp>(exp);
    }
    
    shared_ptr<Exp> bool_exp = make_shared<Exp>();
    // bool_exp->reg = this->allocateVar();
    bool_exp->reg = this->freshVar();
    bool_exp->type = "bool";
    string true_list_label = buffer.genLabel("TRUE_LIST_");
    string false_list_label = buffer.genLabel("FALSE_LIST_");
    string next_list_label = buffer.genLabel("NEXT_LIST_");

    buffer.emit("br label %" + true_list_label);
    buffer.emit(true_list_label + ":");
    int true_address = buffer.emit("br label @");
    buffer.emit(false_list_label + ":");
    int false_address = buffer.emit("br label @");
    buffer.emit(next_list_label + ":");

    bp_list next_list = buffer.merge(buffer.makelist(pair<int, BranchLabelIndex>(true_address, FIRST)),
                               buffer.makelist(pair<int, BranchLabelIndex>(false_address, FIRST)));
    buffer.bpatch(exp.true_list, true_list_label);
    buffer.bpatch(exp.false_list, false_list_label);
    buffer.bpatch(next_list, next_list_label);
    buffer.emit(bool_exp->reg + " = phi i1 [ 1, %" + true_list_label +"], [0, %" + false_list_label + "]");
    return bool_exp;
}

void Generator::genStoreVar(string rbp, int offset, string value, const string& ltype, const string& rtype) {
    string reg_value = this->freshVar();
    string reg_ptr = this->freshVar(); // should add reg to symbol????
    if ((ltype != "int" && value != "0") || (rtype != "int" && rtype != "")) {
        string type = (ltype == "int") ? rtype : ltype; 
        buffer.emit(reg_value + " = zext " + fanCTypeToIRString(type) + " " + value + " to i32");
    }
    else {
        reg_value = value;
    }
    buffer.emit(reg_ptr + " = getelementptr i32, i32* " + rbp + ", i32 " + to_string(offset));
    buffer.emit("store i32 " + reg_value + ", i32* " + reg_ptr);
}

string Generator::genLoadVar(string rbp, int offset, const string& type) {
    string var_ptr = this->freshVar();
    string extend_reg = this->freshVar();
    string reg = this->freshVar();
    buffer.emit(var_ptr + " = getelementptr i32, i32* " + rbp + ", i32 " + to_string(offset));
    buffer.emit(extend_reg + " = load i32,  i32* " + var_ptr);
    if (type != "int") {
        buffer.emit(reg + " = trunc i32 " + extend_reg + " to " + fanCTypeToIRString(type));
        return reg;
    }
    return extend_reg;
}

void Generator::genRet(shared_ptr<Exp> exp) {
    if (exp == nullptr) {
        buffer.emit("ret void");
    }
    else {
        string reg = exp->reg;
        string ret_type = symtable.getCurrScopeRetType();
        if (exp->type != ret_type) {
            reg = this->freshVar();
            buffer.emit(reg + " = zext " + fanCTypeToIRString(exp->type) + " " + exp->reg + " to i32");
        }
        buffer.emit("ret " + fanCTypeToIRString(ret_type) + " " + reg);
    }
}

void Generator::genLabelAfterIf(Exp* exp) {
    int address = buffer.emit("br label @");
    exp->next_list = buffer.merge(buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST)), exp->next_list);
}

void Generator::genEndOfFunc(string type) { 
    type == "void" ? buffer.emit("ret void") : buffer.emit("ret " + fanCTypeToIRString(type) + " 0");
    buffer.emit("}");
}

void Generator::genFuncDecl(string name, vector<string> params, string ret_type) {
    string args_line = "";
    for(auto it = params.begin(); it != params.end(); it++)
    {
        args_line += fanCTypeToIRString(*it) + ", ";
        name += "_" + *it;
    }

    if (args_line != "") { // Remove the comma and space from last arg
        args_line = args_line.substr(0, args_line.size() - 2); 
    }

    buffer.emit("define " + fanCTypeToIRString(ret_type) + " @" + name + "(" + args_line + ") {");
}

void Generator::genCall(string name, Call& call, const Symbol& func, vector<shared_ptr<Exp>> params) {
    string args_line = "";
    // if (name == "printi") {
    //     string reg = params[0]->reg;
    //     if (params[0]->type == "byte") {
    //         reg = this->freshVar();
    //         buffer.emit(reg + " = zext i8 " + params[0]->reg + " to i32");
    //     }
    //     buffer.emit("call void @printi_int(i32 " + reg + ")");
    //     return;
    // }
    
    for(int i = 0; i < params.size(); i++)
    {
        string reg = params[i]->reg;
        if (params[i]->type != func.params[i]) {
            reg = this->freshVar();
            buffer.emit(reg + " = zext i8 " + params[i]->reg + " to i32");
        }
        args_line += fanCTypeToIRString(func.params[i]) + " " + reg + ", ";
        name += "_" + func.params[i];
    }

    if (args_line != "") { // Remove the comma and space from last arg
        args_line = args_line.substr(0, args_line.size() - 2); 
    }

    string reg_str = "";
    call.reg = "";
    if (call.type != "void") {
        call.reg = this->freshVar();
        reg_str = call.reg + " = ";
    }
    buffer.emit(reg_str + "call " + fanCTypeToIRString(call.type) + " @" + name + "(" + args_line + ")");
    
    if (call.type == "bool") {
        string reg_bool = this->freshVar();
        //buffer.emit(reg_bool + " = icmp ne i32 0, " + func.reg);
        buffer.emit(reg_bool + " = icmp ne i1 0, " + call.reg);
        call.reg = reg_bool;
        int address = buffer.emit("br i1 " + call.reg + " , label @, label @");
        call.true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
        call.false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
    }
}

Exp* Generator::genBoolIfNeeded(Exp* exp) {
    shared_ptr<Exp> new_exp = this->genBoolExp(*exp); 
    exp = new Exp(new_exp.get());
    return exp;
}

void Generator::genConversion(Exp& result, const Exp& exp) {
    string reg = this->freshVar();
    if(result.type == "int") {
        
        buffer.emit(reg + " = zext i8 " + exp.reg + " to i32");
    } else {
        buffer.emit(reg + " = trunc i32 " + exp.reg + " to i8");
    }
    result.reg = reg;
}