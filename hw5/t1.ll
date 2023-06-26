@.intFormat = internal constant [4 x i8] c"%d\0A\00"
@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c"Error division by zero\00"
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @print(i8*) {
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
ret void
}
define void @printi(i32) {
%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)
ret void
}
define void @main() {
%var_0 = alloca i32, i32 50
%var_1 = add i32 1, 0
%var_3 = zext i32 %var_1to i32
%var_2 = getelementptr i32, i32* %var_0, i32 0
store i32 %var_3, i32* %var_2
%var_4 = add i32 1, 0
%var_5 = add i32 2, 0
%var_6 = icmp sgt i32 %var_4, %var_5
br i1 %var_6, label %label_10, label %label_15
label_10:
%var_7 = add i32 2, 0
%var_9 = zext i32%var_7to i32
%var_8 = getelementptr i32, i32* %var_0, i32 0
store i32 %var_9, i32* %var_8
label_15:
ret void
}
