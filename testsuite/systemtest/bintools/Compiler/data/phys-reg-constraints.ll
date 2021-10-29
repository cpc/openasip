; ModuleID = 'main-example.bc'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:32-i16:16:32-i32:32:32-i64:32:32-f32:32:32-f64:32:32-v64:32:32-v128:32:32-v256:32:32-v512:32:32-v1024:32:32-a0:0:32-n32"
target triple = "tce-tut-llvm"

@__dummy__ = internal global i32 0, align 4
@result = internal global i32 0, align 4

; Function Attrs: noinline noreturn nounwind
define void @_start() #0 {
entry:
  tail call void asm sideeffect ".call_global_ctors", ""() #2, !srcloc !1

  %a = call i32 asm sideeffect "TEST", "=r,{RF.5},{RF.6},{RF.7},{RF.8}"(i32 49152, i32 14, i32 2560, i32 240)
  store volatile i32 %a, i32* @result, align 4, !tbaa !2

  %res = load volatile i32, i32* @result, align 4
  %cond = icmp eq i32 %res, 51966
  %char = select i1 %cond, i32 84, i32 70
  tail call void asm sideeffect "STDOUT", "ir"(i32 %char)

  tail call void asm sideeffect ".call_global_dtors", ""() #2, !srcloc !6
  tail call fastcc void @_exit() #3
  unreachable
}

; Function Attrs: noinline norecurse noreturn nounwind
define internal fastcc void @_exit() unnamed_addr #1 {
entry:
  br label %while.body

while.body:                                       ; preds = %entry, %while.body
  store volatile i32 0, i32* @__dummy__, align 4, !tbaa !2
  br label %while.body
}

attributes #0 = { noinline noreturn nounwind }
attributes #1 = { noinline noreturn nounwind }
attributes #2 = { nounwind }
attributes #3 = { nobuiltin noreturn }

!llvm.ident = !{!0, !0, !0}

!0 = !{!"clang version 3.8.1 (branches/release_38 270414)"}
!1 = !{i32 666}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{i32 1035}
