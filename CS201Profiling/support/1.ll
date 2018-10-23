; ModuleID = 'support/1.bc'
target datalayout = "e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@BasicBlockPrintfFormatStr = private constant [8 x i8] c"%s: %d\0A\00"
@eachBlockCounter = internal global i32 0
@eachBlockCounter1 = internal global i32 0
@eachBlockCounter2 = internal global i32 0
@eachBlockCounter3 = internal global i32 0
@eachBlockCounter4 = internal global i32 0
@eachBlockCounter5 = internal global i32 0
@eachBlockCounter6 = internal global i32 0
@eachBlockCounter7 = internal global i32 0
@edgeCount = internal global i32 0
@edgeCount8 = internal global i32 0
@edgeCount9 = internal global i32 0
@edgeCount10 = internal global i32 0
@edgeCount11 = internal global i32 0
@edgeCount12 = internal global i32 0
@edgeCount13 = internal global i32 0
@edgeCount14 = internal global i32 0
@edgeCount15 = internal global i32 0
@0 = private unnamed_addr constant [3 x i8] c"b0\00"
@1 = private unnamed_addr constant [3 x i8] c"b6\00"
@2 = private unnamed_addr constant [3 x i8] c"b7\00"
@3 = private unnamed_addr constant [3 x i8] c"b5\00"
@4 = private unnamed_addr constant [3 x i8] c"b4\00"
@5 = private unnamed_addr constant [3 x i8] c"b2\00"
@6 = private unnamed_addr constant [3 x i8] c"b3\00"
@7 = private unnamed_addr constant [3 x i8] c"b1\00"
@8 = private unnamed_addr constant [16 x i8] c"\0AEDGE PROFILING\00"
@9 = private unnamed_addr constant [9 x i8] c"b5 -> b6\00"
@10 = private unnamed_addr constant [9 x i8] c"b1 -> b2\00"
@11 = private unnamed_addr constant [9 x i8] c"b1 -> b3\00"
@12 = private unnamed_addr constant [9 x i8] c"b3 -> b4\00"
@13 = private unnamed_addr constant [9 x i8] c"b6 -> b1\00"
@14 = private unnamed_addr constant [9 x i8] c"b0 -> b1\00"
@15 = private unnamed_addr constant [9 x i8] c"b4 -> b6\00"
@16 = private unnamed_addr constant [9 x i8] c"b3 -> b5\00"
@17 = private unnamed_addr constant [9 x i8] c"b2 -> b7\00"
@18 = private unnamed_addr constant [16 x i8] c"\0ALOOP PROFILING\00"
@19 = private unnamed_addr constant [16 x i8] c" b1 b6 b4 b3 b5\00"
@eachBlockCounter16 = internal global i32 0

; Function Attrs: nounwind uwtable
define void @function_1(i32 %x) #0 {
b0:
  %0 = load i32* @eachBlockCounter
  %1 = add i32 1, %0
  store i32 %1, i32* @eachBlockCounter
  %2 = alloca i32, align 4
  store i32 %x, i32* %2, align 4
  br label %" "

b1:                                               ; preds = %" 8", %" "
  %3 = load i32* @eachBlockCounter1
  %4 = add i32 1, %3
  store i32 %4, i32* @eachBlockCounter1
  %5 = load i32* %2, align 4
  %6 = icmp ugt i32 %5, 0
  br i1 %6, label %" 1", label %" 2"

b2:                                               ; preds = %" 2"
  %7 = load i32* @eachBlockCounter2
  %8 = add i32 1, %7
  store i32 %8, i32* @eachBlockCounter2
  br label %" 3"

b3:                                               ; preds = %" 1"
  %9 = load i32* @eachBlockCounter3
  %10 = add i32 1, %9
  store i32 %10, i32* @eachBlockCounter3
  %11 = load i32* %2, align 4
  %12 = urem i32 %11, 4
  %13 = icmp eq i32 %12, 0
  br i1 %13, label %" 4", label %" 5"

b4:                                               ; preds = %" 4"
  %14 = load i32* @eachBlockCounter4
  %15 = add i32 1, %14
  store i32 %15, i32* @eachBlockCounter4
  %16 = load i32* %2, align 4
  %17 = add i32 %16, -1
  store i32 %17, i32* %2, align 4
  br label %" 6"

b5:                                               ; preds = %" 5"
  %18 = load i32* @eachBlockCounter5
  %19 = add i32 1, %18
  store i32 %19, i32* @eachBlockCounter5
  %20 = load i32* %2, align 4
  %21 = add i32 %20, -1
  store i32 %21, i32* %2, align 4
  br label %" 7"

b6:                                               ; preds = %" 7", %" 6"
  %22 = load i32* @eachBlockCounter6
  %23 = add i32 1, %22
  store i32 %23, i32* @eachBlockCounter6
  br label %" 8"

b7:                                               ; preds = %" 3"
  %24 = load i32* @eachBlockCounter7
  %25 = add i32 1, %24
  store i32 %25, i32* @eachBlockCounter7
  ret void

" ":                                              ; preds = %b0
  %26 = load i32* @edgeCount
  %27 = add i32 1, %26
  store i32 %27, i32* @edgeCount
  br label %b1

" 1":                                             ; preds = %b1
  %28 = load i32* @edgeCount8
  %29 = add i32 1, %28
  store i32 %29, i32* @edgeCount8
  br label %b3

" 2":                                             ; preds = %b1
  %30 = load i32* @edgeCount9
  %31 = add i32 1, %30
  store i32 %31, i32* @edgeCount9
  br label %b2

" 3":                                             ; preds = %b2
  %32 = load i32* @edgeCount10
  %33 = add i32 1, %32
  store i32 %33, i32* @edgeCount10
  br label %b7

" 4":                                             ; preds = %b3
  %34 = load i32* @edgeCount11
  %35 = add i32 1, %34
  store i32 %35, i32* @edgeCount11
  br label %b4

" 5":                                             ; preds = %b3
  %36 = load i32* @edgeCount12
  %37 = add i32 1, %36
  store i32 %37, i32* @edgeCount12
  br label %b5

" 6":                                             ; preds = %b4
  %38 = load i32* @edgeCount13
  %39 = add i32 1, %38
  store i32 %39, i32* @edgeCount13
  br label %b6

" 7":                                             ; preds = %b5
  %40 = load i32* @edgeCount14
  %41 = add i32 1, %40
  store i32 %41, i32* @edgeCount14
  br label %b6

" 8":                                             ; preds = %b6
  %42 = load i32* @edgeCount15
  %43 = add i32 1, %42
  store i32 %43, i32* @edgeCount15
  br label %b1
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
b0:
  %0 = load i32* @eachBlockCounter16
  %1 = add i32 1, %0
  store i32 %1, i32* @eachBlockCounter16
  %2 = alloca i32, align 4
  store i32 0, i32* %2
  call void @function_1(i32 100)
  %3 = load i32* @eachBlockCounter
  %4 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @0, i32 0, i32 0), i32 %3)
  %5 = load i32* @eachBlockCounter6
  %6 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @1, i32 0, i32 0), i32 %5)
  %7 = load i32* @eachBlockCounter7
  %8 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @2, i32 0, i32 0), i32 %7)
  %9 = load i32* @eachBlockCounter5
  %10 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @3, i32 0, i32 0), i32 %9)
  %11 = load i32* @eachBlockCounter4
  %12 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @4, i32 0, i32 0), i32 %11)
  %13 = load i32* @eachBlockCounter2
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @5, i32 0, i32 0), i32 %13)
  %15 = load i32* @eachBlockCounter3
  %16 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @6, i32 0, i32 0), i32 %15)
  %17 = load i32* @eachBlockCounter1
  %18 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @7, i32 0, i32 0), i32 %17)
  %19 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8]* @8, i32 0, i32 0))
  %20 = load i32* @edgeCount14
  %21 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @9, i32 0, i32 0), i32 %20)
  %22 = load i32* @edgeCount9
  %23 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @10, i32 0, i32 0), i32 %22)
  %24 = load i32* @edgeCount8
  %25 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @11, i32 0, i32 0), i32 %24)
  %26 = load i32* @edgeCount11
  %27 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @12, i32 0, i32 0), i32 %26)
  %28 = load i32* @edgeCount15
  %29 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @13, i32 0, i32 0), i32 %28)
  %30 = load i32* @edgeCount
  %31 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @14, i32 0, i32 0), i32 %30)
  %32 = load i32* @edgeCount13
  %33 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @15, i32 0, i32 0), i32 %32)
  %34 = load i32* @edgeCount12
  %35 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @16, i32 0, i32 0), i32 %34)
  %36 = load i32* @edgeCount10
  %37 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @17, i32 0, i32 0), i32 %36)
  %38 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8]* @18, i32 0, i32 0))
  %39 = load i32* @edgeCount15
  %40 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8]* @19, i32 0, i32 0), i32 %39)
  ret i32 0
}

declare i32 @printf(i8*, ...)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
