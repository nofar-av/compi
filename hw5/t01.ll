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
@var_8 = constant [5 x i8] c"true\00"
define void @printByValue_bool(i1) {
%var_0 = alloca i32, i32 50
%var_1 = icmp ne i1 0, %0
br i1 %var_1, label %label_4, label %label_9
br label %label_4
label_4:
%var_2.ptr = getelementptr[12 x i8], [12 x i8]* @var_2, i32 0, i32 0
call void @print_string(i8* %var_2.ptr)
br label %label_13
br label %label_9
label_9:
%var_3.ptr = getelementptr[13 x i8], [13 x i8]* @var_3, i32 0, i32 0
call void @print_string(i8* %var_3.ptr)
br label %label_13
label_13:
ret void
}
define void @main() {
%var_4 = alloca i32, i32 50
br label %TRUE_LIST_label_20
br label %TRUE_LIST_label_20
TRUE_LIST_label_20:
br label %NEXT_LIST_label_20
FALSE_LIST_label_20:
br label %NEXT_LIST_label_20
NEXT_LIST_label_20:
%var_5 = phi i1 [ 1, %TRUE_LIST_label_20], [0, %FALSE_LIST_label_20]
call void @printByValue_bool(i1 %var_5)
br label %FALSE_LIST_label_29
br label %TRUE_LIST_label_29
TRUE_LIST_label_29:
br label %NEXT_LIST_label_29
FALSE_LIST_label_29:
br label %NEXT_LIST_label_29
NEXT_LIST_label_29:
%var_6 = phi i1 [ 1, %TRUE_LIST_label_29], [0, %FALSE_LIST_label_29]
call void @printByValue_bool(i1 %var_6)
br label %label_38
br label %label_38
label_38:
%var_7.ptr = getelementptr[5 x i8], [5 x i8]* @var_7, i32 0, i32 0
call void @print_string(i8* %var_7.ptr)
br label %label_42
label_42:
br label %label_45
br label %label_45
label_45:
br label %label_55
br label %label_48
label_48:
br label %label_51
br label %label_51
label_51:
%var_8.ptr = getelementptr[5 x i8], [5 x i8]* @var_8, i32 0, i32 0
call void @print_string(i8* %var_8.ptr)
br label %label_55
label_55:
ret void
}
