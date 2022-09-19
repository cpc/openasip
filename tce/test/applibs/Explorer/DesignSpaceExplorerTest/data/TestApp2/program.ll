; ModuleID = 'program.bc'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:32-i16:16:32-i32:32:32-i64:32:32-f32:32:32-f64:32:32-v64:32:32-v128:32:32-v256:32:32-v512:32:32-v1024:32:32-a0:0:32-n32"
target triple = "tce-tut-llvm"

%0 = type { %1 }
%1 = type { [30 x ptr], [30 x i32] }
%2 = type { i32, %struct.__FILE*, %struct.__FILE*, %struct.__FILE*, i32, [25 x i8], i32, ptr, i32, void (%struct._reent*)*, %struct._Bigint*, i32, %struct._Bigint*, %struct._Bigint**, i32, ptr, %3, %struct._atexit*, %struct._atexit, void (i32)**, %struct._glue, [3 x %struct.__FILE] }
%3 = type { %4, [44 x i8] }
%4 = type { i32, ptr, [26 x i8], %struct.__tm, i32, i32, %struct._rand48, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, [8 x i8], [24 x i8], i32, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t }
%struct._reent = type { i32, %struct.__FILE*, %struct.__FILE*, %struct.__FILE*, i32, [25 x i8], i32, ptr, i32, void (%struct._reent*)*, %struct._Bigint*, i32, %struct._Bigint*, %struct._Bigint**, i32, ptr, %0, %struct._atexit*, %struct._atexit, void (i32)**, %struct._glue, [3 x %struct.__FILE] }
%struct.__FILE = type { ptr, i32, i32, i16, i16, %struct.__sbuf, i32, ptr, i32 (ptr, ptr, i32)*, i32 (ptr, ptr, i32)*, i32 (ptr, i32, i32)*, i32 (ptr)*, %struct.__sbuf, ptr, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i32, %struct._reent*, i32 }
%struct.__sbuf = type { ptr, i32 }
%struct._Bigint = type { %struct._Bigint*, i32, i32, i32, i32, [1 x i32] }
%struct._atexit = type { %struct._atexit*, i32, [32 x void ()*], %struct._on_exit_args }
%struct._on_exit_args = type { [32 x ptr], [32 x ptr], i32, i32 }
%struct._glue = type { %struct._glue*, i32, %struct.__FILE* }
%struct.__tm = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%struct._rand48 = type { [3 x i16], [3 x i16], i16 }
%struct._mbstate_t = type { i32, %struct.anon }
%struct.anon = type { i32 }

@ar = internal global i32 2
@ai = internal global i32 1
@br = internal global i32 2
@bi = internal global i32 5
@cr = internal global i32 0
@ci = internal global i32 0
@_Output = internal global [2 x i32] zeroinitializer
@_profiling = internal global i32 0
@_global_impure_ptr = internal constant %struct._reent* bitcast (%2* @impure_data to %struct._reent*)
@.str2 = internal constant [2 x i8] c"C\00"
@impure_data = internal global %2 { i32 0, %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 0), %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 1), %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 2), i32 0, [25 x i8] zeroinitializer, i32 0, ptr getelementptr inbounds ([2 x i8], [2 x i8]* @.str2, i32 0, i32 0), i32 0, void (%struct._reent*)* null, %struct._Bigint* null, i32 0, %struct._Bigint* null, %struct._Bigint** null, i32 0, ptr null, %3 { %4 { i32 0, ptr null, [26 x i8] zeroinitializer, %struct.__tm zeroinitializer, i32 0, i32 1, %struct._rand48 { [3 x i16] [i16 13070, i16 -21555, i16 4660], [3 x i16] [i16 -6547, i16 -8468, i16 5], i16 11 }, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, [8 x i8] zeroinitializer, [24 x i8] zeroinitializer, i32 0, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer }, [44 x i8] zeroinitializer }, %struct._atexit* null, %struct._atexit zeroinitializer, void (i32)** null, %struct._glue zeroinitializer, [3 x %struct.__FILE] zeroinitializer }
@_impure_ptr = internal global %struct._reent* bitcast (%2* @impure_data to %struct._reent*)
@end = internal global i8 0

define void @_start() {
entry:
  store i32 -1, ptr @cr, align 4
  store i32 12, ptr @ci, align 4
  store i32 -1, ptr getelementptr inbounds ([2 x i32], [2 x i32]* @_Output, i32 0, i32 0), align 4
  store i32 12, ptr getelementptr inbounds ([2 x i32], [2 x i32]* @_Output, i32 0, i32 1), align 4
  store i32 2, ptr @ar, align 4
  store i32 1, ptr @ai, align 4
  store i32 2, ptr @br, align 4
  store i32 5, ptr @bi, align 4
  tail call void @exit(i32 undef)
  unreachable
}

define internal void @exit(i32 %code) {
entry:
  tail call void @__call_exitprocs(i32 %code, ptr null)
  %tmp3 = load void (%struct._reent*)*, void (%struct._reent*)** getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 9), align 4
  %tmp4 = icmp eq void (%struct._reent*)* %tmp3, null
  br i1 %tmp4, label %cond_next, label %cond_true

cond_true:                                        ; preds = %entry
  tail call void %tmp3(%struct._reent* bitcast (%2* @impure_data to %struct._reent*))
  tail call void @_exit(i32 %code)
  unreachable

cond_next:                                        ; preds = %entry
  tail call void @_exit(i32 %code)
  unreachable
}

define internal void @pin_down(ptr %ar, ptr %ai, ptr %br, ptr %bi, ptr %cr, ptr %ci) {
entry:
  store i32 2, ptr %ar, align 4
  store i32 1, ptr %ai, align 4
  store i32 2, ptr %br, align 4
  store i32 5, ptr %bi, align 4
  ret void
}

define internal void @main() {
entry:
  store i32 -1, ptr @cr, align 4
  store i32 12, ptr @ci, align 4
  store i32 -1, ptr getelementptr inbounds ([2 x i32], [2 x i32]* @_Output, i32 0, i32 0), align 4
  store i32 12, ptr getelementptr inbounds ([2 x i32], [2 x i32]* @_Output, i32 0, i32 1), align 4
  store i32 2, ptr @ar, align 4
  store i32 1, ptr @ai, align 4
  store i32 2, ptr @br, align 4
  store i32 5, ptr @bi, align 4
  ret void
}

define internal void @__call_exitprocs(i32 %code, ptr %d) {
entry:
  %tmp2 = load %struct._atexit*, %struct._atexit** getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 17), align 4
  %tmp120 = icmp eq %struct._atexit* %tmp2, null
  br i1 %tmp120, label %return, label %bb

bb:                                               ; preds = %entry
  %tmp8 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 1
  %tmp9 = load i32, ptr %tmp8, align 4
  %n.0138 = add i32 %tmp9, -1
  %tmp114140 = icmp sgt i32 %n.0138, -1
  br i1 %tmp114140, label %bb11.preheader, label %return

bb11.preheader:                                   ; preds = %bb
  %tmp6 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3
  %tmp15 = icmp eq ptr %d, null
  %tmp18 = icmp eq %struct._on_exit_args* %tmp6, null
  %tmp72 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3, i32 2
  %tmp83 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3, i32 3
  br label %bb11

bb11:                                             ; preds = %bb112, %cond_true90, %bb79, %bb11.preheader
  %n.0124.0 = phi i32 [ %n.0138, %bb11.preheader ], [ %n.0142, %bb79 ], [ %n.0145, %cond_true90 ], [ %n.0, %bb112 ]
  %tmp13 = shl i32 1, %n.0124.0
  br i1 %tmp15, label %cond_next33, label %cond_true

cond_true:                                        ; preds = %bb11
  br i1 %tmp18, label %bb112, label %cond_next

cond_next:                                        ; preds = %cond_true
  %tmp25 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3, i32 1, i32 %n.0124.0
  %tmp26 = load ptr, ptr %tmp25, align 4
  %tmp28 = icmp eq ptr %tmp26, %d
  br i1 %tmp28, label %cond_next33, label %bb112

cond_next33:                                      ; preds = %cond_next, %bb11
  %tmp37 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 2, i32 %n.0124.0
  %tmp38 = load void ()*, void ()** %tmp37, align 4
  %tmp41 = load i32, ptr %tmp8, align 4
  %tmp42 = add i32 %tmp41, -1
  %tmp44 = icmp eq i32 %tmp42, %n.0124.0
  %tmp60 = icmp eq void ()* %tmp38, null
  br i1 %tmp44, label %cond_true47, label %cond_false

cond_true47:                                      ; preds = %cond_next33
  store i32 %tmp42, ptr %tmp8, align 4
  br i1 %tmp60, label %bb112, label %cond_next64

cond_false:                                       ; preds = %cond_next33
  store void ()* null, void ()** %tmp37, align 4
  br i1 %tmp60, label %bb112, label %cond_next64

cond_next64:                                      ; preds = %cond_false, %cond_true47
  br i1 %tmp18, label %bb79, label %cond_next70

cond_next70:                                      ; preds = %cond_next64
  %tmp73 = load i32, ptr %tmp72, align 4
  %tmp75 = and i32 %tmp73, %tmp13
  %tmp76 = icmp eq i32 %tmp75, 0
  br i1 %tmp76, label %bb79, label %bb81

bb79:                                             ; preds = %cond_next70, %cond_next64
  tail call void %tmp38()
  %n.0142 = add i32 %n.0124.0, -1
  %tmp114144 = icmp sgt i32 %n.0142, -1
  br i1 %tmp114144, label %bb11, label %return

bb81:                                             ; preds = %cond_next70
  %tmp84 = load i32, ptr %tmp83, align 4
  %tmp86 = and i32 %tmp84, %tmp13
  %tmp87 = icmp eq i32 %tmp86, 0
  br i1 %tmp87, label %cond_true90, label %cond_false99

cond_true90:                                      ; preds = %bb81
  %tmp9192 = bitcast void ()* %tmp38 to void (i32, ptr)*
  %tmp96 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3, i32 0, i32 %n.0124.0
  %tmp97 = load ptr, ptr %tmp96, align 4
  tail call void %tmp9192(i32 %code, ptr %tmp97)
  %n.0145 = add i32 %n.0124.0, -1
  %tmp114147 = icmp sgt i32 %n.0145, -1
  br i1 %tmp114147, label %bb11, label %return

cond_false99:                                     ; preds = %bb81
  %tmp100101 = bitcast void ()* %tmp38 to void (ptr)*
  %tmp105 = getelementptr %struct._atexit, %struct._atexit* %tmp2, i32 0, i32 3, i32 0, i32 %n.0124.0
  %tmp106 = load ptr, ptr %tmp105, align 4
  tail call void %tmp100101(ptr %tmp106)
  br label %bb112

bb112:                                            ; preds = %cond_false99, %cond_false, %cond_true47, %cond_next, %cond_true
  %n.0 = add i32 %n.0124.0, -1
  %tmp114 = icmp sgt i32 %n.0, -1
  br i1 %tmp114, label %bb11, label %return

return:                                           ; preds = %bb112, %cond_true90, %bb79, %bb, %entry
  ret void
}

define internal void @_exit(i32 %rc) {
entry:
  ret void
}
