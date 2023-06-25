@.intFormat = internal constant [4 x i8] c"%d\0A\00"
@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c"Error division by zero\00"
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @print(i8*){
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
ret void
}
define void @printi(i32){
%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)
ret void
}
define void @main() {
%var_0 = alloca i32, i32 50
%var_1 = add i32 1, 0
%var_2 = getelementptr i32, i32* %var_0, i32 0
store i32 %var_1, i32* %var_2
%var_3 = add i32 1, 0
%var_4 = add i32 2, 0
%var_5 = icmp sgt i32 %var_3, %var_4
br i1 %var_5, label %label_9, label %label_14
label_9:
%var_6 = add i32 1, 0
%var_7 = getelementptr i32, i32* %var_0, i32 0
store i32 %var_6, i32* %var_7
ret void
label_14:
%var_8 = add i32 2, 0
%var_9 = getelementptr i32, i32* %var_0, i32 0
store i32 %var_8, i32* %var_9
ret void
}
