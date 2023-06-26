@.intFormat = internal constant [4 x i8] c"%d\0A\00"
@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c"Error division by zero\00"
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @print_string(i8*) {
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
ret void
}
define void @printi_int(i32) {
%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)
ret void
}
@var_2 = constant [12 x i8] c"val is true\00"
@var_3 = constant [13 x i8] c"val is false\00"
@var_7 = constant [5 x i8] c"true\00"
@var_10 = constant [5 x i8] c"true\00"
define void @printByValue_bool(i1) {
%var_0 = alloca i32, i32 50
%var_1 = icmp ne i1 0, %0
br i1 %var_1, label %label_4, label %label_8
label_4:
%var_2.ptr = getelementptr[12 x i8], [12 x i8]* @var_2, i32 0, i32 0
call void @print_string(i8* %var_2.ptr)
br label %label_11
label_8:
%var_3.ptr = getelementptr[13 x i8], [13 x i8]* @var_3, i32 0, i32 0
call void @print_string(i8* %var_3.ptr)
label_11:
ret void
}
define void @main() {
%var_4 = alloca i32, i32 50
br label %TRUE_LIST_label_17
TRUE_LIST_label_17:
FALSE_LIST_label_18:
NEXT_LIST_label_19:
br label %TRUE_LIST_label_17
TRUE_LIST_label_17:
br label %NEXT_LIST_label_19
FALSE_LIST_label_18:
br label %NEXT_LIST_label_19
NEXT_LIST_label_19:
%var_5 = phi i32 [ 1, %TRUE_LIST_label_17], [0, %FALSE_LIST_label_18]
call void @printByValue_bool(i1 %var_5)
br label %FALSE_LIST_label_30
TRUE_LIST_label_29:
FALSE_LIST_label_30:
NEXT_LIST_label_31:
br label %TRUE_LIST_label_29
TRUE_LIST_label_29:
br label %NEXT_LIST_label_31
FALSE_LIST_label_30:
br label %NEXT_LIST_label_31
NEXT_LIST_label_31:
%var_6 = phi i32 [ 1, %TRUE_LIST_label_29], [0, %FALSE_LIST_label_30]
call void @printByValue_bool(i1 %var_6)
br label %label_41
label_41:
%var_7.ptr = getelementptr[5 x i8], [5 x i8]* @var_7, i32 0, i32 0
call void @print_string(i8* %var_7.ptr)
label_44:
br label %label_46
label_46:
br label %label_55
label_48:
br label %label_52
%var_8 = and :( , 
%var_9 = or :( , %var_8
label_52:
%var_10.ptr = getelementptr[5 x i8], [5 x i8]* @var_10, i32 0, i32 0
call void @print_string(i8* %var_10.ptr)
label_55:
ret void
}
