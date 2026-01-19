; ModuleID = 'program.bc'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:32-i16:16:32-i32:32:32-i64:32:32-f16:16:16-f32:32:32-f64:32:32-v64:64:64-i128:128-v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-v2048:2048:2048-v4096:4096:4096-a0:0:32-n32"
target triple = "tce-tut-llvm"

%0 = type { %1 }
%1 = type { [30 x ptr], [30 x i32] }
%2 = type { i32, %struct.__FILE*, %struct.__FILE*, %struct.__FILE*, i32, [25 x i8], i32, ptr, i32, void (%struct._reent*)*, %struct._Bigint*, i32, %struct._Bigint*, %struct._Bigint**, i32, ptr, %3, %struct._atexit*, %struct._atexit, void (i32)**, %struct._glue, [3 x %struct.__FILE] }
%3 = type { %4, [44 x i8] }
%4 = type { i32, ptr, [26 x i8], %struct.__tm, i32, i32, %struct._rand48, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, [8 x i8], [24 x i8], i32, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t, %struct._mbstate_t }
%struct.STATES = type { [8 x i16], [8 x i16], i16, i16, i16, i16, i16, i16, i16, i16, i32 }
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

@Input = internal global [32 x i16] [i16 255, i16 167, i16 25, i16 146, i16 15, i16 145, i16 24, i16 163, i16 77, i16 43, i16 154, i16 19, i16 143, i16 17, i16 150, i16 32, i16 190, i16 175, i16 28, i16 148, i16 16, i16 144, i16 21, i16 158, i16 54, i16 54, i16 158, i16 21, i16 144, i16 16, i16 148, i16 28]
@A_LAW_table = internal constant [256 x i16] [i16 4784, i16 4752, i16 4848, i16 4816, i16 4656, i16 4624, i16 4720, i16 4688, i16 5040, i16 5008, i16 5104, i16 5072, i16 4912, i16 4880, i16 4976, i16 4944, i16 4440, i16 4424, i16 4472, i16 4456, i16 4376, i16 4360, i16 4408, i16 4392, i16 4568, i16 4552, i16 4600, i16 4584, i16 4504, i16 4488, i16 4536, i16 4520, i16 6848, i16 6720, i16 7104, i16 6976, i16 6336, i16 6208, i16 6592, i16 6464, i16 7872, i16 7744, i16 8128, i16 8000, i16 7360, i16 7232, i16 7616, i16 7488, i16 5472, i16 5408, i16 5600, i16 5536, i16 5216, i16 5152, i16 5344, i16 5280, i16 5984, i16 5920, i16 6112, i16 6048, i16 5728, i16 5664, i16 5856, i16 5792, i16 4139, i16 4137, i16 4143, i16 4141, i16 4131, i16 4129, i16 4135, i16 4133, i16 4155, i16 4153, i16 4159, i16 4157, i16 4147, i16 4145, i16 4151, i16 4149, i16 4107, i16 4105, i16 4111, i16 4109, i16 4099, i16 4097, i16 4103, i16 4101, i16 4123, i16 4121, i16 4127, i16 4125, i16 4115, i16 4113, i16 4119, i16 4117, i16 4268, i16 4260, i16 4284, i16 4276, i16 4236, i16 4228, i16 4252, i16 4244, i16 4332, i16 4324, i16 4348, i16 4340, i16 4300, i16 4292, i16 4316, i16 4308, i16 4182, i16 4178, i16 4190, i16 4186, i16 4166, i16 4162, i16 4174, i16 4170, i16 4214, i16 4210, i16 4222, i16 4218, i16 4198, i16 4194, i16 4206, i16 4202, i16 688, i16 656, i16 752, i16 720, i16 560, i16 528, i16 624, i16 592, i16 944, i16 912, i16 1008, i16 976, i16 816, i16 784, i16 880, i16 848, i16 344, i16 328, i16 376, i16 360, i16 280, i16 264, i16 312, i16 296, i16 472, i16 456, i16 504, i16 488, i16 408, i16 392, i16 440, i16 424, i16 2752, i16 2624, i16 3008, i16 2880, i16 2240, i16 2112, i16 2496, i16 2368, i16 3776, i16 3648, i16 4032, i16 3904, i16 3264, i16 3136, i16 3520, i16 3392, i16 1376, i16 1312, i16 1504, i16 1440, i16 1120, i16 1056, i16 1248, i16 1184, i16 1888, i16 1824, i16 2016, i16 1952, i16 1632, i16 1568, i16 1760, i16 1696, i16 43, i16 41, i16 47, i16 45, i16 35, i16 33, i16 39, i16 37, i16 59, i16 57, i16 63, i16 61, i16 51, i16 49, i16 55, i16 53, i16 11, i16 9, i16 15, i16 13, i16 3, i16 1, i16 7, i16 5, i16 27, i16 25, i16 31, i16 29, i16 19, i16 17, i16 23, i16 21, i16 172, i16 164, i16 188, i16 180, i16 140, i16 132, i16 156, i16 148, i16 236, i16 228, i16 252, i16 244, i16 204, i16 196, i16 220, i16 212, i16 86, i16 82, i16 94, i16 90, i16 70, i16 66, i16 78, i16 74, i16 118, i16 114, i16 126, i16 122, i16 102, i16 98, i16 110, i16 106]
@u_LAW_table = internal constant [256 x i16] [i16 16223, i16 15967, i16 15711, i16 15455, i16 15199, i16 14943, i16 14687, i16 14431, i16 14175, i16 13919, i16 13663, i16 13407, i16 13151, i16 12895, i16 12639, i16 12383, i16 12191, i16 12063, i16 11935, i16 11807, i16 11679, i16 11551, i16 11423, i16 11295, i16 11167, i16 11039, i16 10911, i16 10783, i16 10655, i16 10527, i16 10399, i16 10271, i16 10175, i16 10111, i16 10047, i16 9983, i16 9919, i16 9855, i16 9791, i16 9727, i16 9663, i16 9599, i16 9535, i16 9471, i16 9407, i16 9343, i16 9279, i16 9215, i16 9167, i16 9135, i16 9103, i16 9071, i16 9039, i16 9007, i16 8975, i16 8943, i16 8911, i16 8879, i16 8847, i16 8815, i16 8783, i16 8751, i16 8719, i16 8687, i16 8663, i16 8647, i16 8631, i16 8615, i16 8599, i16 8583, i16 8567, i16 8551, i16 8535, i16 8519, i16 8503, i16 8487, i16 8471, i16 8455, i16 8439, i16 8423, i16 8411, i16 8403, i16 8395, i16 8387, i16 8379, i16 8371, i16 8363, i16 8355, i16 8347, i16 8339, i16 8331, i16 8323, i16 8315, i16 8307, i16 8299, i16 8291, i16 8285, i16 8281, i16 8277, i16 8273, i16 8269, i16 8265, i16 8261, i16 8257, i16 8253, i16 8249, i16 8245, i16 8241, i16 8237, i16 8233, i16 8229, i16 8225, i16 8222, i16 8220, i16 8218, i16 8216, i16 8214, i16 8212, i16 8210, i16 8208, i16 8206, i16 8204, i16 8202, i16 8200, i16 8198, i16 8196, i16 8194, i16 0, i16 8031, i16 7775, i16 7519, i16 7263, i16 7007, i16 6751, i16 6495, i16 6239, i16 5983, i16 5727, i16 5471, i16 5215, i16 4959, i16 4703, i16 4447, i16 4191, i16 3999, i16 3871, i16 3743, i16 3615, i16 3487, i16 3359, i16 3231, i16 3103, i16 2975, i16 2847, i16 2719, i16 2591, i16 2463, i16 2335, i16 2207, i16 2079, i16 1983, i16 1919, i16 1855, i16 1791, i16 1727, i16 1663, i16 1599, i16 1535, i16 1471, i16 1407, i16 1343, i16 1279, i16 1215, i16 1151, i16 1087, i16 1023, i16 975, i16 943, i16 911, i16 879, i16 847, i16 815, i16 783, i16 751, i16 719, i16 687, i16 655, i16 623, i16 591, i16 559, i16 527, i16 495, i16 471, i16 455, i16 439, i16 423, i16 407, i16 391, i16 375, i16 359, i16 343, i16 327, i16 311, i16 295, i16 279, i16 263, i16 247, i16 231, i16 219, i16 211, i16 203, i16 195, i16 187, i16 179, i16 171, i16 163, i16 155, i16 147, i16 139, i16 131, i16 123, i16 115, i16 107, i16 99, i16 93, i16 89, i16 85, i16 81, i16 77, i16 73, i16 69, i16 65, i16 61, i16 57, i16 53, i16 49, i16 45, i16 41, i16 37, i16 33, i16 30, i16 28, i16 26, i16 24, i16 22, i16 20, i16 18, i16 16, i16 14, i16 12, i16 10, i16 8, i16 6, i16 4, i16 2, i16 0]
@D = internal global i16 0
@Y = internal global i16 0
@I = internal global i16 0
@X = internal global %struct.STATES* null
@SEZ = internal global i16 0
@SE = internal global i16 0
@DQ = internal global i16 0
@SR = internal global i16 0
@TR.b = internal global i1 false
@A2P = internal global i16 0
@LAW = internal global i32 0
@S = internal global i16 0
@SD = internal global i16 0
@SL = internal global i16 0
@qtab.1098 = internal constant [16 x i16] [i16 2048, i16 4, i16 135, i16 213, i16 273, i16 323, i16 373, i16 425, i16 425, i16 373, i16 323, i16 273, i16 213, i16 135, i16 4, i16 2048]
@AL = internal global i16 0
@W.1269 = internal constant [8 x i16] [i16 4084, i16 18, i16 41, i16 64, i16 112, i16 198, i16 355, i16 1122]
@F.1344 = internal constant [8 x i16] [i16 0, i16 0, i16 0, i16 512, i16 512, i16 512, i16 1536, i16 3584]
@TDP = internal global i16 0
@E_STATES = internal global %struct.STATES zeroinitializer
@D_STATES = internal global %struct.STATES zeroinitializer
@_global_impure_ptr = internal constant %struct._reent* bitcast (%2* @impure_data to %struct._reent*)
@.str2 = internal constant [2 x i8] c"C\00"
@impure_data = internal global %2 { i32 0, %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 0), %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 1), %struct.__FILE* getelementptr (%struct._reent, %struct._reent* bitcast (%2* @impure_data to %struct._reent*), i32 0, i32 21, i32 2), i32 0, [25 x i8] zeroinitializer, i32 0, ptr getelementptr inbounds ([2 x i8], [2 x i8]* @.str2, i32 0, i32 0), i32 0, void (%struct._reent*)* null, %struct._Bigint* null, i32 0, %struct._Bigint* null, %struct._Bigint** null, i32 0, ptr null, %3 { %4 { i32 0, ptr null, [26 x i8] zeroinitializer, %struct.__tm zeroinitializer, i32 0, i32 1, %struct._rand48 { [3 x i16] [i16 13070, i16 -21555, i16 4660], [3 x i16] [i16 -6547, i16 -8468, i16 5], i16 11 }, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, [8 x i8] zeroinitializer, [24 x i8] zeroinitializer, i32 0, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer, %struct._mbstate_t zeroinitializer }, [44 x i8] zeroinitializer }, %struct._atexit* null, %struct._atexit zeroinitializer, void (i32)** null, %struct._glue zeroinitializer, [3 x %struct.__FILE] zeroinitializer }
@_impure_ptr = internal global %struct._reent* bitcast (%2* @impure_data to %struct._reent*)
@end = internal global i8 0

define void @_start() {
entry:
  %tmp1 = tail call i32 @main(i32 0, ptr null)
  tail call void @exit(i32 %tmp1)
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

define internal fastcc void @adpt_predict_1() {
entry:
  %tmp = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp5 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 0
  br label %bb

bb:                                               ; preds = %bb151, %entry
  %tmp2.0162.0.rec = phi i32 [ 0, %entry ], [ %indvar.next1, %bb151 ]
  %tmp1.1166.0 = phi ptr [ %tmp5, %entry ], [ %tmp1.0179.1, %bb151 ]
  %SEZI.0169.0 = phi i16 [ 0, %entry ], [ %tmp139, %bb151 ]
  %tmp2.0162.0 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 %tmp2.0162.0.rec
  %tmp10 = load i16, ptr %tmp1.1166.0, align 2
  %tmp11 = icmp slt i16 %tmp10, 0
  %tmp18 = lshr i16 %tmp10, 2
  br i1 %tmp11, label %cond_true, label %cond_next

cond_true:                                        ; preds = %bb
  %tmp19 = sub i16 0, %tmp18
  %tmp20 = and i16 %tmp19, 8191
  %tmp1.0181 = getelementptr i16, ptr %tmp1.1166.0, i32 1
  %tmp40190 = icmp eq i16 %tmp20, 0
  br i1 %tmp40190, label %bb43, label %bb38

cond_next:                                        ; preds = %bb
  %tmp1.0 = getelementptr i16, ptr %tmp1.1166.0, i32 1
  %tmp40192 = icmp ult i16 %tmp10, 4
  br i1 %tmp40192, label %bb43, label %bb38

bb38:                                             ; preds = %bb38, %cond_next, %cond_true
  %iftmp.14.0176.0.ph.ph = phi i16 [ %tmp20, %cond_true ], [ %tmp18, %cond_next ], [ %iftmp.14.0176.0.ph.ph, %bb38 ]
  %tmp1.0179.0.ph.ph = phi ptr [ %tmp1.0181, %cond_true ], [ %tmp1.0, %cond_next ], [ %tmp1.0179.0.ph.ph, %bb38 ]
  %AnEXP.0183.0 = phi i16 [ %tmp37, %bb38 ], [ 0, %cond_next ], [ 0, %cond_true ]
  %tmp30.0187.0 = phi i16 [ %tmp35, %bb38 ], [ %tmp20, %cond_true ], [ %tmp18, %cond_next ]
  %tmp35 = lshr i16 %tmp30.0187.0, 1
  %tmp37 = add i16 %AnEXP.0183.0, 1
  %tmp40 = icmp ult i16 %tmp30.0187.0, 2
  br i1 %tmp40, label %bb43, label %bb38

bb43:                                             ; preds = %bb38, %cond_next, %cond_true
  %iftmp.14.0176.1 = phi i16 [ %tmp20, %cond_true ], [ %tmp18, %cond_next ], [ %iftmp.14.0176.0.ph.ph, %bb38 ]
  %tmp1.0179.1 = phi ptr [ %tmp1.0181, %cond_true ], [ %tmp1.0, %cond_next ], [ %tmp1.0179.0.ph.ph, %bb38 ]
  %AnEXP.0183.1 = phi i16 [ 0, %cond_true ], [ 0, %cond_next ], [ %tmp37, %bb38 ]
  %tmp45 = icmp eq i16 %iftmp.14.0176.1, 0
  br i1 %tmp45, label %cond_next74, label %cond_true48

cond_true48:                                      ; preds = %bb43
  %tmp4950 = sext i16 %AnEXP.0183.1 to i32
  %tmp51 = add i32 %tmp4950, -6
  %tmp52 = icmp slt i32 %tmp51, 0
  %tmp5657 = zext i16 %iftmp.14.0176.1 to i32
  br i1 %tmp52, label %cond_true55, label %cond_next71

cond_true55:                                      ; preds = %cond_true48
  %tmp60 = sub i32 6, %tmp4950
  %tmp61 = shl i32 %tmp5657, %tmp60
  %phitmp193 = and i32 %tmp61, 65535
  br label %cond_next74

cond_next71:                                      ; preds = %cond_true48
  %tmp69 = lshr i32 %tmp5657, %tmp51
  %phitmp = and i32 %tmp69, 65535
  br label %cond_next74

cond_next74:                                      ; preds = %cond_next71, %cond_true55, %bb43
  %iftmp.15.0 = phi i32 [ %phitmp, %cond_next71 ], [ %phitmp193, %cond_true55 ], [ 32, %bb43 ]
  %tmp77 = load i16, ptr %tmp2.0162.0, align 2
  %tmp7778 = zext i16 %tmp77 to i32
  %tmp79 = and i32 %tmp7778, 63
  %tmp82 = mul i32 %tmp79, %iftmp.15.0
  %tmp83 = add i32 %tmp82, 48
  %tmp84161 = lshr i32 %tmp83, 4
  %tmp89 = lshr i16 %tmp77, 6
  %tmp90 = and i16 %tmp89, 15
  %tmp92 = add i16 %tmp90, %AnEXP.0183.1
  %tmp9394 = sext i16 %tmp92 to i32
  %tmp95 = sub i32 19, %tmp9394
  %tmp96 = icmp slt i32 %tmp95, 0
  %tmp100101 = and i32 %tmp84161, 65535
  br i1 %tmp96, label %cond_true99, label %cond_false108

cond_true99:                                      ; preds = %cond_next74
  %tmp104 = add i32 %tmp9394, -19
  %tmp105 = shl i32 %tmp100101, %tmp104
  br label %cond_next117

cond_false108:                                    ; preds = %cond_next74
  %tmp114 = lshr i32 %tmp100101, %tmp95
  br label %cond_next117

cond_next117:                                     ; preds = %cond_false108, %cond_true99
  %iftmp.17.0.in.in = phi i32 [ %tmp114, %cond_false108 ], [ %tmp105, %cond_true99 ]
  %iftmp.17.0.in = trunc i32 %iftmp.17.0.in.in to i16
  %iftmp.17.0 = and i16 %iftmp.17.0.in, 32767
  %tmp122 = lshr i32 %tmp7778, 10
  %tmp123 = and i32 %tmp122, 1
  %tmp10.lobit = lshr i16 %tmp10, 15
  %tmp160 = zext i16 %tmp10.lobit to i32
  %tmp126 = icmp eq i32 %tmp123, %tmp160
  %tmp133 = sub i16 0, %iftmp.17.0
  %iftmp.18.0 = select i1 %tmp126, i16 %iftmp.17.0, i16 %tmp133
  %tmp139 = add i16 %iftmp.18.0, %SEZI.0169.0
  %tmp141 = icmp eq i32 %tmp2.0162.0.rec, 5
  br i1 %tmp141, label %cond_true144, label %bb151

cond_true144:                                     ; preds = %cond_next117
  %tmp147 = lshr i16 %tmp139, 1
  store i16 %tmp147, ptr @SEZ, align 2
  br label %bb151

bb151:                                            ; preds = %cond_true144, %cond_next117
  %indvar.next1 = add i32 %tmp2.0162.0.rec, 1
  %exitcond2 = icmp eq i32 %indvar.next1, 8
  br i1 %exitcond2, label %bb156, label %bb

bb156:                                            ; preds = %bb151
  %tmp159 = lshr i16 %tmp139, 1
  store i16 %tmp159, ptr @SE, align 2
  ret void
}

define internal fastcc void @adpt_predict_2() {
entry:
  %tmp = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp3 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 6
  %tmp5 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 7
  %tmp6 = load i16, ptr @DQ, align 2
  %tmp67 = zext i16 %tmp6 to i32
  %tmp910557 = and i32 %tmp67, 16384
  %toBool = icmp eq i32 %tmp910557, 0
  br i1 %toBool, label %cond_next, label %cond_true

cond_true:                                        ; preds = %entry
  %tmp12 = and i16 %tmp6, 16383
  %tmp13 = sub i16 0, %tmp12
  br label %cond_next

cond_next:                                        ; preds = %cond_true, %entry
  %iftmp.21.0 = phi i16 [ %tmp13, %cond_true ], [ %tmp6, %entry ]
  %tmp15 = load i16, ptr @SEZ, align 2
  %tmp1516 = zext i16 %tmp15 to i32
  %tmp1819555 = and i32 %tmp1516, 16384
  %toBool20 = icmp eq i32 %tmp1819555, 0
  %tmp23 = select i1 %toBool20, i16 0, i16 -32768
  %iftmp.26.0 = xor i16 %tmp23, %tmp15
  %tmp29 = add i16 %iftmp.26.0, %iftmp.21.0
  %tmp29.lobit = lshr i16 %tmp29, 15
  br i1 %toBool, label %cond_next46, label %cond_true40

cond_true40:                                      ; preds = %cond_next
  %tmp42 = and i16 %tmp6, 16383
  %tmp43 = sub i16 0, %tmp42
  br label %cond_next46

cond_next46:                                      ; preds = %cond_true40, %cond_next
  %iftmp.32.0 = phi i16 [ %tmp43, %cond_true40 ], [ %tmp6, %cond_next ]
  %tmp47 = load i16, ptr @SE, align 2
  %tmp4748 = zext i16 %tmp47 to i32
  %tmp5051551 = and i32 %tmp4748, 16384
  %toBool52 = icmp eq i32 %tmp5051551, 0
  %tmp55 = select i1 %toBool52, i16 0, i16 -32768
  %iftmp.37.0 = xor i16 %tmp55, %tmp47
  %tmp61 = add i16 %iftmp.37.0, %iftmp.32.0
  store i16 %tmp61, ptr @SR, align 2
  %tmp66 = icmp slt i16 %tmp61, 0
  br i1 %tmp66, label %cond_true69, label %cond_next75

cond_true69:                                      ; preds = %cond_next46
  %tmp71 = sub i16 0, %tmp61
  %tmp72 = and i16 %tmp71, 32767
  %tmp85569 = icmp eq i16 %tmp72, 0
  br i1 %tmp85569, label %bb88, label %bb

cond_next75:                                      ; preds = %cond_next46
  %tmp85571 = icmp eq i16 %tmp61, 0
  br i1 %tmp85571, label %bb88, label %bb

bb:                                               ; preds = %bb, %cond_next75, %cond_true69
  %iftmp.43.0558.0.ph.ph = phi i16 [ %tmp72, %cond_true69 ], [ %tmp61, %cond_next75 ], [ %iftmp.43.0558.0.ph.ph, %bb ]
  %EXP.0564.0 = phi i16 [ %tmp82, %bb ], [ 0, %cond_next75 ], [ 0, %cond_true69 ]
  %tmp77.0562.0 = phi i16 [ %tmp80, %bb ], [ %tmp72, %cond_true69 ], [ %tmp61, %cond_next75 ]
  %tmp80 = lshr i16 %tmp77.0562.0, 1
  %tmp82 = add i16 %EXP.0564.0, 1
  %tmp85 = icmp ult i16 %tmp77.0562.0, 2
  br i1 %tmp85, label %bb88, label %bb

bb88:                                             ; preds = %bb, %cond_next75, %cond_true69
  %iftmp.43.0558.1 = phi i16 [ %tmp72, %cond_true69 ], [ %tmp61, %cond_next75 ], [ %iftmp.43.0558.0.ph.ph, %bb ]
  %EXP.0564.1 = phi i16 [ 0, %cond_true69 ], [ 0, %cond_next75 ], [ %tmp82, %bb ]
  %iftmp.46.0 = select i1 %tmp66, i16 1024, i16 0
  %tmp98549 = shl i16 %EXP.0564.1, 6
  %tmp103 = icmp eq i16 %iftmp.43.0558.1, 0
  br i1 %tmp103, label %cond_next132, label %cond_true106

cond_true106:                                     ; preds = %bb88
  %tmp107108 = sext i16 %EXP.0564.1 to i32
  %tmp109 = add i32 %tmp107108, -6
  %tmp110 = icmp slt i32 %tmp109, 0
  %tmp114115 = zext i16 %iftmp.43.0558.1 to i32
  br i1 %tmp110, label %cond_true113, label %cond_next129

cond_true113:                                     ; preds = %cond_true106
  %tmp118 = sub i32 6, %tmp107108
  %tmp119 = shl i32 %tmp114115, %tmp118
  %iftmp.48.0572 = trunc i32 %tmp119 to i16
  br label %cond_next132

cond_next129:                                     ; preds = %cond_true106
  %tmp127 = lshr i32 %tmp114115, %tmp109
  %iftmp.48.0 = trunc i32 %tmp127 to i16
  br label %cond_next132

cond_next132:                                     ; preds = %cond_next129, %cond_true113, %bb88
  %iftmp.47.0 = phi i16 [ %iftmp.48.0, %cond_next129 ], [ %iftmp.48.0572, %cond_true113 ], [ 32, %bb88 ]
  %tmp101 = add i16 %tmp98549, %iftmp.46.0
  %tmp134 = add i16 %tmp101, %iftmp.47.0
  %tmp136 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 2
  %tmp137 = load i16, ptr %tmp136, align 4
  %tmp141 = load i16, ptr %tmp3, align 2
  %tmp142 = icmp slt i16 %tmp141, 0
  br i1 %tmp142, label %cond_true145, label %cond_false160

cond_true145:                                     ; preds = %cond_next132
  %tmp148 = icmp ugt i16 %tmp141, -8192
  br i1 %tmp148, label %cond_true151, label %cond_next174

cond_true151:                                     ; preds = %cond_true145
  %tmp153154 = zext i16 %tmp141 to i32
  %tmp155 = shl i32 %tmp153154, 2
  %tmp156 = and i32 %tmp155, 131068
  %tmp176576 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 3
  %tmp177577 = load i16, ptr %tmp176576, align 2
  %tmp179578 = icmp eq i16 %tmp177577, %tmp29.lobit
  %iftmp.51.0579 = select i1 %tmp179578, i32 16384, i32 114688
  %tmp186580 = icmp eq i16 %tmp137, %tmp29.lobit
  br i1 %tmp186580, label %cond_true189, label %cond_next195

cond_false160:                                    ; preds = %cond_next132
  %tmp163 = icmp ult i16 %tmp141, 8192
  br i1 %tmp163, label %cond_true166, label %cond_next174

cond_true166:                                     ; preds = %cond_false160
  %tmp168169 = zext i16 %tmp141 to i32
  %tmp170 = shl i32 %tmp168169, 2
  br label %cond_next174

cond_next174:                                     ; preds = %cond_true166, %cond_false160, %cond_true145
  %FA1.0 = phi i32 [ %tmp170, %cond_true166 ], [ 98308, %cond_true145 ], [ 32764, %cond_false160 ]
  %tmp176 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 3
  %tmp177 = load i16, ptr %tmp176, align 2
  %tmp179 = icmp eq i16 %tmp177, %tmp29.lobit
  %iftmp.51.0 = select i1 %tmp179, i32 16384, i32 114688
  %tmp186 = icmp eq i16 %tmp137, %tmp29.lobit
  br i1 %tmp186, label %cond_true189, label %cond_next195

cond_true189:                                     ; preds = %cond_next174, %cond_true151
  %FA1.0573.0 = phi i32 [ %tmp156, %cond_true151 ], [ %FA1.0, %cond_next174 ]
  %iftmp.51.0575.0 = phi i32 [ %iftmp.51.0579, %cond_true151 ], [ %iftmp.51.0, %cond_next174 ]
  %tmp191 = sub i32 0, %FA1.0573.0
  %tmp192 = and i32 %tmp191, 131071
  br label %cond_next195

cond_next195:                                     ; preds = %cond_true189, %cond_next174, %cond_true151
  %iftmp.51.0575.1 = phi i32 [ %iftmp.51.0575.0, %cond_true189 ], [ %iftmp.51.0579, %cond_true151 ], [ %iftmp.51.0, %cond_next174 ]
  %iftmp.52.0 = phi i32 [ %tmp192, %cond_true189 ], [ %tmp156, %cond_true151 ], [ %FA1.0, %cond_next174 ]
  %tmp198 = add i32 %iftmp.52.0, %iftmp.51.0575.1
  %tmp199 = lshr i32 %tmp198, 7
  %tmp199200 = trunc i32 %tmp199 to i16
  %tmp201 = and i16 %tmp199200, 1023
  %tmp203 = icmp eq i16 %tmp29, 0
  br i1 %tmp203, label %cond_next221, label %cond_true206

cond_true206:                                     ; preds = %cond_next195
  %tmp207208 = zext i16 %tmp201 to i32
  %tmp210211538 = and i32 %tmp207208, 512
  %toBool212 = icmp eq i32 %tmp210211538, 0
  br i1 %toBool212, label %cond_next221, label %cond_true213

cond_true213:                                     ; preds = %cond_true206
  %tmp215536 = or i16 %tmp201, -1024
  br label %cond_next221

cond_next221:                                     ; preds = %cond_true213, %cond_true206, %cond_next195
  %iftmp.53.0 = phi i16 [ %tmp215536, %cond_true213 ], [ 0, %cond_next195 ], [ %tmp201, %cond_true206 ]
  %tmp224 = load i16, ptr %tmp5, align 2
  %tmp225 = icmp slt i16 %tmp224, 0
  %tmp232 = lshr i16 %tmp224, 7
  %.pn547 = select i1 %tmp225, i16 512, i16 0
  %iftmp.56.0 = sub i16 %.pn547, %tmp232
  %tmp245 = add i16 %tmp224, %iftmp.53.0
  %tmp247 = add i16 %tmp245, %iftmp.56.0
  %tmp249.b = load i1, i1* @TR.b, align 1
  br i1 %tmp249.b, label %bb254.preheader, label %cond_false267

bb254.preheader:                                  ; preds = %cond_next221
  %tmp258 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 0
  store i16 0, ptr %tmp258, align 2
  %tmp258.1 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 1
  store i16 0, ptr %tmp258.1, align 2
  %tmp258.2 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 2
  store i16 0, ptr %tmp258.2, align 2
  %tmp258.3 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 3
  store i16 0, ptr %tmp258.3, align 2
  %tmp258.4 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 4
  store i16 0, ptr %tmp258.4, align 2
  %tmp258.5 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 5
  store i16 0, ptr %tmp258.5, align 2
  store i16 0, ptr %tmp3, align 2
  store i16 0, ptr %tmp5, align 2
  br label %cond_next427

cond_false267:                                    ; preds = %cond_next221
  %tmp269 = icmp sgt i16 %tmp247, -1
  br i1 %tmp269, label %cond_true272, label %cond_false275

cond_true272:                                     ; preds = %cond_false267
  %tmp274 = icmp ult i16 %tmp247, 12289
  %min = select i1 %tmp274, i16 %tmp247, i16 12288
  store i16 %min, ptr @A2P, align 2
  store i16 %min, ptr %tmp5, align 2
  %tmp288588 = load i16, ptr %tmp3, align 2
  br i1 %tmp203, label %cond_next303, label %cond_true293

cond_false275:                                    ; preds = %cond_false267
  %tmp277 = icmp ugt i16 %tmp247, -12289
  %max = select i1 %tmp277, i16 %tmp247, i16 -12288
  store i16 %max, ptr @A2P, align 2
  store i16 %max, ptr %tmp5, align 2
  %tmp288 = load i16, ptr %tmp3, align 2
  br i1 %tmp203, label %cond_next303, label %cond_true293

cond_true293:                                     ; preds = %cond_false275, %cond_true272
  %tmp328.rle627 = phi i16 [ %min, %cond_true272 ], [ %max, %cond_false275 ]
  %tmp307.rle626 = phi i16 [ %tmp288588, %cond_true272 ], [ %tmp288, %cond_false275 ]
  %tmp288587.0 = phi i16 [ %tmp288588, %cond_true272 ], [ %tmp288, %cond_false275 ]
  %tmp284 = load i16, ptr %tmp136, align 4
  %tmp295 = icmp eq i16 %tmp284, %tmp29.lobit
  br i1 %tmp295, label %cond_false299, label %cond_next303

cond_false299:                                    ; preds = %cond_true293
  br label %cond_next303

cond_next303:                                     ; preds = %cond_false299, %cond_true293, %cond_false275, %cond_true272
  %tmp328.rle = phi i16 [ %tmp328.rle627, %cond_false299 ], [ %min, %cond_true272 ], [ %max, %cond_false275 ], [ %tmp328.rle627, %cond_true293 ]
  %tmp307.rle = phi i16 [ %tmp307.rle626, %cond_false299 ], [ %tmp288588, %cond_true272 ], [ %tmp288, %cond_false275 ], [ %tmp307.rle626, %cond_true293 ]
  %tmp288587.1 = phi i16 [ %tmp288587.0, %cond_false299 ], [ %tmp288588, %cond_true272 ], [ %tmp288, %cond_false275 ], [ %tmp288587.0, %cond_true293 ]
  %iftmp.63.0 = phi i16 [ 192, %cond_false299 ], [ 0, %cond_true272 ], [ 0, %cond_false275 ], [ -192, %cond_true293 ]
  %tmp308 = icmp slt i16 %tmp307.rle, 0
  %tmp315 = lshr i16 %tmp307.rle, 8
  %.pn546 = select i1 %tmp308, i16 256, i16 0
  %iftmp.65.0 = sub i16 %.pn546, %tmp315
  %tmp305 = add i16 %iftmp.63.0, %tmp288587.1
  %tmp325 = add i16 %tmp305, %iftmp.65.0
  %tmp329 = sub i16 15360, %tmp328.rle
  %tmp332 = add i16 %tmp328.rle, -15360
  %tmp335 = icmp sgt i16 %tmp325, -1
  br i1 %tmp335, label %cond_true338, label %cond_false343

cond_true338:                                     ; preds = %cond_next303
  %tmp341 = icmp ule i16 %tmp329, %tmp325
  %min342 = select i1 %tmp341, i16 %tmp329, i16 %tmp325
  br label %cond_next348

cond_false343:                                    ; preds = %cond_next303
  %tmp346 = icmp uge i16 %tmp332, %tmp325
  %max347 = select i1 %tmp346, i16 %tmp332, i16 %tmp325
  br label %cond_next348

cond_next348:                                     ; preds = %cond_false343, %cond_true338
  %iftmp.66.0 = phi i16 [ %max347, %cond_false343 ], [ %min342, %cond_true338 ]
  store i16 %iftmp.66.0, ptr %tmp3, align 2
  %tmp360 = load i16, ptr @DQ, align 2
  %tmp360361 = zext i16 %tmp360 to i32
  %tmp362 = lshr i32 %tmp360361, 14
  %tmp366 = and i16 %tmp360, 16383
  %tmp371 = icmp eq i16 %tmp366, 0
  br i1 %tmp371, label %bb367.us.preheader, label %bb367

bb367.us.preheader:                               ; preds = %cond_next348
  %tmp1352.0591.0.us = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 0
  %tmp369.us = load i16, ptr %tmp1352.0591.0.us, align 2
  %tmp397.us = icmp slt i16 %tmp369.us, 0
  %tmp404.us = lshr i16 %tmp369.us, 8
  %.pn.us = select i1 %tmp397.us, i16 256, i16 0
  %iftmp.72.0.us = sub i16 %tmp369.us, %tmp404.us
  %tmp414.us = add i16 %iftmp.72.0.us, %.pn.us
  store i16 %tmp414.us, ptr %tmp1352.0591.0.us, align 2
  %tmp1352.0591.0.us.1 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 1
  %tmp369.us.1 = load i16, ptr %tmp1352.0591.0.us.1, align 2
  %tmp397.us.1 = icmp slt i16 %tmp369.us.1, 0
  %tmp404.us.1 = lshr i16 %tmp369.us.1, 8
  %.pn.us.1 = select i1 %tmp397.us.1, i16 256, i16 0
  %iftmp.72.0.us.1 = sub i16 %tmp369.us.1, %tmp404.us.1
  %tmp414.us.1 = add i16 %iftmp.72.0.us.1, %.pn.us.1
  store i16 %tmp414.us.1, ptr %tmp1352.0591.0.us.1, align 2
  %tmp1352.0591.0.us.2 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 2
  %tmp369.us.2 = load i16, ptr %tmp1352.0591.0.us.2, align 2
  %tmp397.us.2 = icmp slt i16 %tmp369.us.2, 0
  %tmp404.us.2 = lshr i16 %tmp369.us.2, 8
  %.pn.us.2 = select i1 %tmp397.us.2, i16 256, i16 0
  %iftmp.72.0.us.2 = sub i16 %tmp369.us.2, %tmp404.us.2
  %tmp414.us.2 = add i16 %iftmp.72.0.us.2, %.pn.us.2
  store i16 %tmp414.us.2, ptr %tmp1352.0591.0.us.2, align 2
  %tmp1352.0591.0.us.3 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 3
  %tmp369.us.3 = load i16, ptr %tmp1352.0591.0.us.3, align 2
  %tmp397.us.3 = icmp slt i16 %tmp369.us.3, 0
  %tmp404.us.3 = lshr i16 %tmp369.us.3, 8
  %.pn.us.3 = select i1 %tmp397.us.3, i16 256, i16 0
  %iftmp.72.0.us.3 = sub i16 %tmp369.us.3, %tmp404.us.3
  %tmp414.us.3 = add i16 %iftmp.72.0.us.3, %.pn.us.3
  store i16 %tmp414.us.3, ptr %tmp1352.0591.0.us.3, align 2
  %tmp1352.0591.0.us.4 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 4
  %tmp369.us.4 = load i16, ptr %tmp1352.0591.0.us.4, align 2
  %tmp397.us.4 = icmp slt i16 %tmp369.us.4, 0
  %tmp404.us.4 = lshr i16 %tmp369.us.4, 8
  %.pn.us.4 = select i1 %tmp397.us.4, i16 256, i16 0
  %iftmp.72.0.us.4 = sub i16 %tmp369.us.4, %tmp404.us.4
  %tmp414.us.4 = add i16 %iftmp.72.0.us.4, %.pn.us.4
  store i16 %tmp414.us.4, ptr %tmp1352.0591.0.us.4, align 2
  %tmp1352.0591.0.us.5 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 5
  %tmp369.us.5 = load i16, ptr %tmp1352.0591.0.us.5, align 2
  %tmp397.us.5 = icmp slt i16 %tmp369.us.5, 0
  %tmp404.us.5 = lshr i16 %tmp369.us.5, 8
  %.pn.us.5 = select i1 %tmp397.us.5, i16 256, i16 0
  %iftmp.72.0.us.5 = sub i16 %tmp369.us.5, %tmp404.us.5
  %tmp414.us.5 = add i16 %iftmp.72.0.us.5, %.pn.us.5
  store i16 %tmp414.us.5, ptr %tmp1352.0591.0.us.5, align 2
  br label %cond_next427

bb367:                                            ; preds = %bb367, %cond_next348
  %i351.0589.0 = phi i32 [ 0, %cond_next348 ], [ %indvar.next3, %bb367 ]
  %tmp2353.1596.0 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 %i351.0589.0
  %tmp1352.0591.0 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 0, i32 %i351.0589.0
  %tmp369 = load i16, ptr %tmp1352.0591.0, align 2
  %tmp378 = load i16, ptr %tmp2353.1596.0, align 2
  %tmp378379 = zext i16 %tmp378 to i32
  %tmp380 = lshr i32 %tmp378379, 10
  %tmp540 = xor i32 %tmp380, %tmp362
  %tmp541 = and i32 %tmp540, 1
  %tmp382 = icmp eq i32 %tmp541, 0
  %iftmp.70.0 = select i1 %tmp382, i16 128, i16 -128
  %tmp397 = icmp slt i16 %tmp369, 0
  %tmp404 = lshr i16 %tmp369, 8
  %.pn = select i1 %tmp397, i16 256, i16 0
  %tmp394 = sub i16 %tmp369, %tmp404
  %iftmp.72.0 = add i16 %tmp394, %.pn
  %tmp414 = add i16 %iftmp.72.0, %iftmp.70.0
  store i16 %tmp414, ptr %tmp1352.0591.0, align 2
  %indvar.next3 = add i32 %i351.0589.0, 1
  %exitcond4 = icmp eq i32 %indvar.next3, 6
  br i1 %exitcond4, label %cond_next427, label %bb367

cond_next427:                                     ; preds = %bb367, %bb367.us.preheader, %bb254.preheader
  %tmp434 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 5
  %tmp437 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 4
  %tmp438 = load i16, ptr %tmp437, align 2
  store i16 %tmp438, ptr %tmp434, align 2
  %tmp437.1 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 3
  %tmp438.1 = load i16, ptr %tmp437.1, align 2
  store i16 %tmp438.1, ptr %tmp437, align 2
  %tmp437.2 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 2
  %tmp438.2 = load i16, ptr %tmp437.2, align 2
  store i16 %tmp438.2, ptr %tmp437.1, align 2
  %tmp437.3 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 1
  %tmp438.3 = load i16, ptr %tmp437.3, align 2
  store i16 %tmp438.3, ptr %tmp437.2, align 2
  %tmp437.4 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 0
  %tmp438.4 = load i16, ptr %tmp437.4, align 2
  store i16 %tmp438.4, ptr %tmp437.3, align 2
  %tmp450 = load i16, ptr @DQ, align 2
  %tmp451 = and i16 %tmp450, 16383
  %tmp462615 = icmp eq i16 %tmp451, 0
  br i1 %tmp462615, label %bb465, label %bb460

bb460:                                            ; preds = %bb460, %cond_next427
  %EXP430.0608.0 = phi i16 [ 0, %cond_next427 ], [ %tmp459, %bb460 ]
  %tmp452.0612.0 = phi i16 [ %tmp451, %cond_next427 ], [ %tmp457, %bb460 ]
  %tmp457 = lshr i16 %tmp452.0612.0, 1
  %tmp459 = add i16 %EXP430.0608.0, 1
  %tmp462 = icmp ult i16 %tmp452.0612.0, 2
  br i1 %tmp462, label %bb465, label %bb460

bb465:                                            ; preds = %bb460, %cond_next427
  %EXP430.0608.1 = phi i16 [ 0, %cond_next427 ], [ %tmp459, %bb460 ]
  %tmp466467 = zext i16 %tmp450 to i32
  %tmp469470545 = and i32 %tmp466467, 16384
  %toBool471 = icmp eq i32 %tmp469470545, 0
  %iftmp.74.0 = select i1 %toBool471, i16 0, i16 1024
  %tmp477543 = shl i16 %EXP430.0608.1, 6
  br i1 %tmp462615, label %cond_next511, label %cond_true485

cond_true485:                                     ; preds = %bb465
  %tmp486487 = sext i16 %EXP430.0608.1 to i32
  %tmp488 = add i32 %tmp486487, -6
  %tmp489 = icmp slt i32 %tmp488, 0
  %tmp493494 = zext i16 %tmp451 to i32
  br i1 %tmp489, label %cond_true492, label %cond_next508

cond_true492:                                     ; preds = %cond_true485
  %tmp497 = sub i32 6, %tmp486487
  %tmp498 = shl i32 %tmp493494, %tmp497
  %iftmp.77.0616 = trunc i32 %tmp498 to i16
  br label %cond_next511

cond_next508:                                     ; preds = %cond_true485
  %tmp506 = lshr i32 %tmp493494, %tmp488
  %iftmp.77.0 = trunc i32 %tmp506 to i16
  br label %cond_next511

cond_next511:                                     ; preds = %cond_next508, %cond_true492, %bb465
  %iftmp.76.0 = phi i16 [ %iftmp.77.0, %cond_next508 ], [ %iftmp.77.0616, %cond_true492 ], [ 32, %bb465 ]
  %tmp480 = add i16 %tmp477543, %iftmp.74.0
  %tmp513 = add i16 %tmp480, %iftmp.76.0
  store i16 %tmp513, ptr %tmp437.4, align 2
  %tmp519 = load i16, ptr %tmp136, align 4
  %tmp521 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 3
  store i16 %tmp519, ptr %tmp521, align 2
  store i16 %tmp29.lobit, ptr %tmp136, align 4
  %tmp527 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 6
  %tmp528 = load i16, ptr %tmp527, align 2
  %tmp531 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 1, i32 7
  store i16 %tmp528, ptr %tmp531, align 2
  store i16 %tmp134, ptr %tmp527, align 2
  ret void
}

define internal fastcc void @iadpt_quant() {
entry:
  %tmp = load i16, ptr @I, align 2
  %tmp1 = zext i16 %tmp to i32
  %tmp2 = getelementptr [16 x i16], [16 x i16]* @qtab.1098, i32 0, i32 %tmp1
  %tmp3 = load i16, ptr %tmp2, align 2
  %tmp4 = load i16, ptr @Y, align 2
  %tmp5 = lshr i16 %tmp4, 2
  %tmp6 = add i16 %tmp5, %tmp3
  %tmp171860 = and i32 %tmp1, 8
  %toBool = icmp eq i32 %tmp171860, 0
  %iftmp.139.0 = select i1 %toBool, i16 0, i16 16384
  %tmp1920 = zext i16 %tmp6 to i32
  %tmp22 = and i32 %tmp1920, 2048
  %tmp23 = icmp eq i32 %tmp22, 0
  br i1 %tmp23, label %cond_true26, label %cond_next52

cond_true26:                                      ; preds = %entry
  %tmp9 = lshr i16 %tmp6, 7
  %tmp10 = and i16 %tmp9, 15
  %tmp12 = or i16 %tmp6, 128
  %tmp1357 = and i16 %tmp12, 255
  %tmp272858 = zext i16 %tmp10 to i32
  %tmp29 = sub i32 7, %tmp272858
  %tmp30 = icmp slt i32 %tmp29, 0
  %tmp3435 = zext i16 %tmp1357 to i32
  br i1 %tmp30, label %cond_true33, label %cond_next49

cond_true33:                                      ; preds = %cond_true26
  %tmp38 = add i32 %tmp272858, -7
  %tmp39 = shl i32 %tmp3435, %tmp38
  %iftmp.142.061 = trunc i32 %tmp39 to i16
  %tmp5562 = add i16 %iftmp.142.061, %iftmp.139.0
  store i16 %tmp5562, ptr @DQ, align 2
  ret void

cond_next49:                                      ; preds = %cond_true26
  %tmp47 = lshr i32 %tmp3435, %tmp29
  %iftmp.142.0 = trunc i32 %tmp47 to i16
  %tmp5563 = add i16 %iftmp.142.0, %iftmp.139.0
  store i16 %tmp5563, ptr @DQ, align 2
  ret void

cond_next52:                                      ; preds = %entry
  store i16 %iftmp.139.0, ptr @DQ, align 2
  ret void
}

define internal fastcc void @input_conversion() {
entry:
  %tmp = load i32, ptr @LAW, align 4
  %tmp1 = icmp eq i32 %tmp, 0
  %tmp3 = load i16, ptr @S, align 2
  %tmp34 = zext i16 %tmp3 to i32
  br i1 %tmp1, label %cond_false, label %cond_true

cond_true:                                        ; preds = %entry
  %tmp5 = getelementptr [256 x i16], [256 x i16]* @A_LAW_table, i32 0, i32 %tmp34
  %tmp6 = load i16, ptr %tmp5, align 2
  %tmp8 = and i16 %tmp6, 4096
  %tmp10 = shl i16 %tmp6, 1
  %tmp11 = and i16 %tmp10, 8190
  %tmp2235 = icmp eq i16 %tmp8, 0
  br i1 %tmp2235, label %cond_next31, label %cond_true25

cond_false:                                       ; preds = %entry
  %tmp15 = getelementptr [256 x i16], [256 x i16]* @u_LAW_table, i32 0, i32 %tmp34
  %tmp16 = load i16, ptr %tmp15, align 2
  %tmp18 = and i16 %tmp16, 8192
  %tmp20 = and i16 %tmp16, 8191
  %tmp22 = icmp eq i16 %tmp18, 0
  br i1 %tmp22, label %cond_next31, label %cond_true25

cond_true25:                                      ; preds = %cond_false, %cond_true
  %SSQ.033.0 = phi i16 [ %tmp11, %cond_true ], [ %tmp20, %cond_false ]
  %tmp27 = sub i16 0, %SSQ.033.0
  %tmp28 = and i16 %tmp27, 16383
  store i16 %tmp28, ptr @SL, align 2
  ret void

cond_next31:                                      ; preds = %cond_false, %cond_true
  %iftmp.146.0 = phi i16 [ %tmp11, %cond_true ], [ %tmp20, %cond_false ]
  store i16 %iftmp.146.0, ptr @SL, align 2
  ret void
}

define internal fastcc void @scale_factor_2() {
entry:
  %tmp = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp1 = load i16, ptr @I, align 2
  %tmp3 = icmp ult i16 %tmp1, 8
  %tmp56 = zext i16 %tmp1 to i32
  %tmp7 = sub i32 15, %tmp56
  %iftmp.168.0.in = select i1 %tmp3, i32 %tmp56, i32 %tmp7
  %iftmp.168.0 = and i32 %iftmp.168.0.in, 7
  %tmp13 = getelementptr [8 x i16], [8 x i16]* @W.1269, i32 0, i32 %iftmp.168.0
  %tmp14 = load i16, ptr %tmp13, align 2
  %tmp1415 = zext i16 %tmp14 to i32
  %tmp16 = shl i32 %tmp1415, 5
  %tmp17 = load i16, ptr @Y, align 2
  %tmp1718 = zext i16 %tmp17 to i32
  %tmp19 = sub i32 %tmp16, %tmp1718
  %tmp20 = lshr i32 %tmp19, 5
  %tmp2021 = trunc i32 %tmp20 to i16
  %tmp22 = and i16 %tmp2021, 4095
  %tmp24 = and i16 %tmp2021, 2048
  %tmp26 = icmp eq i16 %tmp24, 0
  %tmp31108 = select i1 %tmp26, i16 0, i16 4096
  %iftmp.174.0 = or i16 %tmp31108, %tmp22
  %tmp37 = add i16 %iftmp.174.0, %tmp17
  %tmp38 = and i16 %tmp37, 8191
  %tmp3940 = zext i16 %tmp38 to i32
  %tmp41 = add i32 %tmp3940, 15840
  %tmp43 = and i32 %tmp41, 8192
  %tmp44 = icmp eq i32 %tmp43, 0
  br i1 %tmp44, label %cond_true47, label %cond_next62

cond_true47:                                      ; preds = %entry
  %tmp50 = add i32 %tmp3940, 11264
  %tmp5253110 = and i32 %tmp50, 8192
  %toBool54 = icmp eq i32 %tmp5253110, 0
  br i1 %toBool54, label %cond_false58, label %cond_next62

cond_false58:                                     ; preds = %cond_true47
  br label %cond_next62

cond_next62:                                      ; preds = %cond_false58, %cond_true47, %entry
  %iftmp.178.0 = phi i16 [ 5120, %cond_false58 ], [ 544, %entry ], [ %tmp38, %cond_true47 ]
  %tmp64 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 7
  store i16 %iftmp.178.0, ptr %tmp64, align 2
  %tmp71 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 10
  %tmp72 = load i32, ptr %tmp71, align 4
  %tmp73 = sub i32 0, %tmp72
  %tmp74 = lshr i32 %tmp73, 6
  %tmp7475 = trunc i32 %tmp74 to i16
  %tmp76 = add i16 %tmp7475, %iftmp.178.0
  %tmp77 = and i16 %tmp76, 16383
  %tmp7879 = zext i16 %tmp77 to i32
  %tmp8182113 = and i32 %tmp7879, 8192
  %toBool83 = icmp eq i32 %tmp8182113, 0
  %tmp87111 = select i1 %toBool83, i32 0, i32 507904
  %iftmp.180.0 = or i32 %tmp87111, %tmp7879
  %tmp97 = add i32 %iftmp.180.0, %tmp72
  %tmp98 = and i32 %tmp97, 524287
  store i32 %tmp98, ptr %tmp71, align 4
  %tmp104 = lshr i32 %tmp98, 6
  %tmp104105 = trunc i32 %tmp104 to i16
  %tmp107 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 9
  store i16 %tmp104105, ptr %tmp107, align 2
  ret void
}

define internal fastcc void @speed_control_2() {
entry:
  %tmp = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp1 = load i16, ptr @I, align 2
  %tmp3 = icmp ult i16 %tmp1, 8
  %tmp56 = zext i16 %tmp1 to i32
  %tmp7 = sub i32 15, %tmp56
  %iftmp.184.0.in = select i1 %tmp3, i32 %tmp56, i32 %tmp7
  %iftmp.184.0 = and i32 %iftmp.184.0.in, 7
  %tmp13 = getelementptr [8 x i16], [8 x i16]* @F.1344, i32 0, i32 %iftmp.184.0
  %tmp14 = load i16, ptr %tmp13, align 2
  %tmp1516 = zext i16 %tmp14 to i32
  %tmp18 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 5
  %tmp19 = load i16, ptr %tmp18, align 2
  %tmp1920 = zext i16 %tmp19 to i32
  %tmp21 = sub i32 %tmp1516, %tmp1920
  %tmp22 = lshr i32 %tmp21, 5
  %tmp22182 = trunc i32 %tmp22 to i16
  %tmp2324 = and i16 %tmp22182, 255
  %tmp2829179 = and i32 %tmp22, 128
  %toBool30 = icmp eq i32 %tmp2829179, 0
  %tmp33168 = select i1 %toBool30, i16 0, i16 3840
  %iftmp.189.0 = or i16 %tmp33168, %tmp2324
  %tmp41 = add i16 %iftmp.189.0, %tmp19
  %tmp42 = and i16 %tmp41, 4095
  store i16 %tmp42, ptr %tmp18, align 4
  %tmp48 = shl i32 %tmp1516, 2
  %tmp50 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 6
  %tmp51 = load i16, ptr %tmp50, align 4
  %tmp5152 = zext i16 %tmp51 to i32
  %tmp53 = sub i32 %tmp48, %tmp5152
  %tmp54 = lshr i32 %tmp53, 7
  %tmp54181 = trunc i32 %tmp54 to i16
  %tmp5556 = and i16 %tmp54181, 255
  %tmp6061177 = and i32 %tmp54, 128
  %toBool62 = icmp eq i32 %tmp6061177, 0
  %tmp65169 = select i1 %toBool62, i16 0, i16 16128
  %iftmp.191.0 = or i16 %tmp65169, %tmp5556
  %tmp73 = add i16 %iftmp.191.0, %tmp51
  %tmp74 = and i16 %tmp73, 16383
  store i16 %tmp74, ptr %tmp50, align 4
  %tmp82175 = shl i16 %tmp42, 2
  %tmp87 = sub i16 %tmp82175, %tmp74
  %tmp88 = and i16 %tmp87, 32767
  %tmp8990 = zext i16 %tmp88 to i32
  %tmp9293174 = and i32 %tmp8990, 16384
  %toBool94 = icmp eq i32 %tmp9293174, 0
  br i1 %toBool94, label %cond_next101, label %cond_true95

cond_true95:                                      ; preds = %entry
  %tmp97 = sub i16 -32768, %tmp88
  %tmp98 = and i16 %tmp97, 16383
  br label %cond_next101

cond_next101:                                     ; preds = %cond_true95, %entry
  %iftmp.193.0 = phi i16 [ %tmp98, %cond_true95 ], [ %tmp88, %entry ]
  %tmp103 = load i16, ptr @Y, align 2
  %tmp104 = icmp ult i16 %tmp103, 1536
  br i1 %tmp104, label %bb, label %cond_next108

cond_next108:                                     ; preds = %cond_next101
  %tmp113 = lshr i16 %tmp74, 3
  %tmp115 = icmp ugt i16 %tmp113, %iftmp.193.0
  br i1 %tmp115, label %cond_next119, label %bb

cond_next119:                                     ; preds = %cond_next108
  %tmp120 = load i16, ptr @TDP, align 2
  %tmp121 = icmp eq i16 %tmp120, 0
  br i1 %tmp121, label %bb126, label %bb

bb:                                               ; preds = %cond_next119, %cond_next108, %cond_next101
  br label %bb126

bb126:                                            ; preds = %bb, %cond_next119
  %iftmp.195.0 = phi i32 [ 512, %bb ], [ 0, %cond_next119 ]
  %tmp132 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 4
  %tmp133 = load i16, ptr %tmp132, align 4
  %tmp133134 = zext i16 %tmp133 to i32
  %tmp135 = sub i32 %iftmp.195.0, %tmp133134
  %tmp136 = lshr i32 %tmp135, 4
  %tmp136180 = trunc i32 %tmp136 to i16
  %tmp137138 = and i16 %tmp136180, 127
  %tmp142143172 = and i32 %tmp136, 64
  %toBool144 = icmp eq i32 %tmp142143172, 0
  %tmp147170 = select i1 %toBool144, i16 0, i16 896
  %iftmp.198.0 = or i16 %tmp147170, %tmp137138
  %tmp155 = add i16 %iftmp.198.0, %tmp133
  %tmp156 = and i16 %tmp155, 1023
  %tmp157.b = load i1, i1* @TR.b, align 1
  %iftmp.200.0 = select i1 %tmp157.b, i16 256, i16 %tmp156
  store i16 %iftmp.200.0, ptr %tmp132, align 4
  ret void
}

define internal fastcc void @tone_detector_1() {
entry:
  %tmp = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp1 = load i16, ptr @DQ, align 2
  %tmp2 = and i16 %tmp1, 16383
  %tmp4 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 9
  %tmp5 = load i16, ptr %tmp4, align 4
  %tmp6 = lshr i16 %tmp5, 9
  %tmp14 = icmp ult i16 %tmp6, 9
  br i1 %tmp14, label %cond_true, label %cond_next

cond_true:                                        ; preds = %entry
  %tmp11 = lshr i16 %tmp5, 4
  %tmp12 = and i16 %tmp11, 31
  %tmp1617 = zext i16 %tmp12 to i32
  %tmp1850 = or i32 %tmp1617, 32
  %tmp1920 = zext i16 %tmp6 to i32
  %tmp21 = shl i32 %tmp1850, %tmp1920
  %tmp2122 = trunc i32 %tmp21 to i16
  br label %cond_next

cond_next:                                        ; preds = %cond_true, %entry
  %iftmp.203.0 = phi i16 [ %tmp2122, %cond_true ], [ 15872, %entry ]
  %tmp25 = getelementptr %struct.STATES, %struct.STATES* %tmp, i32 0, i32 8
  %tmp26 = load i16, ptr %tmp25, align 4
  %tmp27 = icmp eq i16 %tmp26, 0
  br i1 %tmp27, label %bb, label %cond_next31

cond_next31:                                      ; preds = %cond_next
  %tmp3233 = zext i16 %tmp2 to i32
  %tmp3435 = zext i16 %iftmp.203.0 to i32
  %tmp38 = lshr i16 %iftmp.203.0, 1
  %tmp3839 = zext i16 %tmp38 to i32
  %tmp40 = add i32 %tmp3839, %tmp3435
  %tmp41 = ashr i32 %tmp40, 1
  %tmp42 = icmp sgt i32 %tmp3233, %tmp41
  br i1 %tmp42, label %bb47, label %bb

bb:                                               ; preds = %cond_next31, %cond_next
  store i1 false, i1* @TR.b, align 1
  ret void

bb47:                                             ; preds = %cond_next31
  store i1 true, i1* @TR.b, align 1
  ret void
}

define internal void @reset_encoder() {
entry:
  store %struct.STATES* @E_STATES, %struct.STATES** @X, align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 0), align 8
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 1), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 2), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 4), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 0), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 1), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 2), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 3), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 4), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 5), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 2), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 6), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 4), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 7), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 8), align 4
  store i32 34816, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 10), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 9), align 2
  ret void
}

define internal zeroext i16 @encoder(i16 zeroext %pcm) {
entry:
  store i16 %pcm, ptr @S, align 2
  store %struct.STATES* @E_STATES, %struct.STATES** @X, align 4
  tail call fastcc void @input_conversion()
  tail call fastcc void @adpt_predict_1()
  %tmp.i = load i16, ptr @SL, align 2
  %tmp1.i = zext i16 %tmp.i to i32
  %tmp3429.i = and i32 %tmp1.i, 8192
  %toBool.i = icmp eq i32 %tmp3429.i, 0
  %tmp6.i = select i1 %toBool.i, i16 0, i16 -16384
  %tmp9.i = load i16, ptr @SE, align 2
  %tmp910.i = zext i16 %tmp9.i to i32
  %tmp121327.i = and i32 %tmp910.i, 16384
  %toBool14.i = icmp eq i32 %tmp121327.i, 0
  %tmp17.i = select i1 %toBool14.i, i16 0, i16 -32768
  %iftmp.132.0.i = xor i16 %tmp17.i, %tmp9.i
  %iftmp.128.0.i = add i16 %tmp6.i, %tmp.i
  %tmp24.i = sub i16 %iftmp.128.0.i, %iftmp.132.0.i
  store i16 %tmp24.i, ptr @D, align 2
  %tmp.i1 = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp1.i2 = getelementptr %struct.STATES, %struct.STATES* %tmp.i1, i32 0, i32 4
  %tmp2.i = load i16, ptr %tmp1.i2, align 4
  %tmp3.i = icmp ult i16 %tmp2.i, 256
  %tmp8.i = lshr i16 %tmp2.i, 2
  %storemerge = select i1 %tmp3.i, i16 %tmp8.i, i16 64
  store i16 %storemerge, ptr @AL, align 2
  %tmp2.i4 = getelementptr %struct.STATES, %struct.STATES* %tmp.i1, i32 0, i32 7
  %tmp3.i5 = load i16, ptr %tmp2.i4, align 2
  %tmp5.i = getelementptr %struct.STATES, %struct.STATES* %tmp.i1, i32 0, i32 9
  %tmp6.i6 = load i16, ptr %tmp5.i, align 2
  %tmp7.i = sub i16 %tmp3.i5, %tmp6.i6
  %tmp9.i7 = and i16 %tmp7.i, 8192
  %tmp11.i = icmp eq i16 %tmp9.i7, 0
  br i1 %tmp11.i, label %scale_factor_1.exit, label %cond_true.i8

cond_true.i8:                                     ; preds = %entry
  %tmp14.i = sub i16 0, %tmp7.i
  %tmp15.i = and i16 %tmp14.i, 8191
  br label %scale_factor_1.exit

scale_factor_1.exit:                              ; preds = %cond_true.i8, %entry
  %iftmp.163.0.i = phi i16 [ %tmp15.i, %cond_true.i8 ], [ %tmp7.i, %entry ]
  %tmp1819.i = zext i16 %iftmp.163.0.i to i32
  %tmp2021.i = zext i16 %storemerge to i32
  %tmp22.i = mul i32 %tmp2021.i, %tmp1819.i
  %tmp23.i = lshr i32 %tmp22.i, 6
  %tmp2324.i = trunc i32 %tmp23.i to i16
  %tmp34.i = sub i16 0, %tmp2324.i
  %iftmp.166.0.i = select i1 %tmp11.i, i16 %tmp2324.i, i16 %tmp34.i
  %tmp39.i = add i16 %iftmp.166.0.i, %tmp6.i6
  %tmp40.i = and i16 %tmp39.i, 8191
  store i16 %tmp40.i, ptr @Y, align 2
  %tmp3.i16 = icmp slt i16 %tmp24.i, 0
  br i1 %tmp3.i16, label %cond_true.i19, label %cond_next.i20

cond_true.i19:                                    ; preds = %scale_factor_1.exit
  %tmp6.i17 = sub i16 0, %tmp24.i
  %tmp7.i18 = and i16 %tmp6.i17, 32767
  %tmp20192.i = icmp ult i16 %tmp7.i18, 2
  br i1 %tmp20192.i, label %bb23.i, label %bb.i

cond_next.i20:                                    ; preds = %scale_factor_1.exit
  %tmp20194.i = icmp ult i16 %tmp24.i, 2
  br i1 %tmp20194.i, label %bb23.i, label %bb.i

bb.i:                                             ; preds = %bb.i, %cond_next.i20, %cond_true.i19
  %iftmp.1.0183.0.ph.ph.i.ph = phi i16 [ %tmp7.i18, %cond_true.i19 ], [ %tmp24.i, %cond_next.i20 ], [ %iftmp.1.0183.0.ph.ph.i.ph, %bb.i ]
  %EXP.0186.0.i = phi i16 [ %tmp17.i21, %bb.i ], [ 0, %cond_next.i20 ], [ 0, %cond_true.i19 ]
  %tmp10.0190.0.in.i = phi i16 [ %tmp10.0190.0.i, %bb.i ], [ %tmp7.i18, %cond_true.i19 ], [ %tmp24.i, %cond_next.i20 ]
  %tmp10.0190.0.i = lshr i16 %tmp10.0190.0.in.i, 1
  %tmp17.i21 = add i16 %EXP.0186.0.i, 1
  %tmp20.i22 = icmp ult i16 %tmp10.0190.0.i, 2
  br i1 %tmp20.i22, label %bb23.i, label %bb.i

bb23.i:                                           ; preds = %bb.i, %cond_next.i20, %cond_true.i19
  %iftmp.1.0183.1.i = phi i16 [ %tmp7.i18, %cond_true.i19 ], [ %tmp24.i, %cond_next.i20 ], [ %iftmp.1.0183.0.ph.ph.i.ph, %bb.i ]
  %EXP.0186.1.i = phi i16 [ 0, %cond_true.i19 ], [ 0, %cond_next.i20 ], [ %tmp17.i21, %bb.i ]
  %tmp26182.i = shl i16 %EXP.0186.1.i, 7
  %tmp2829.i = sext i16 %EXP.0186.1.i to i32
  %tmp30.i = add i32 %tmp2829.i, -7
  %tmp31.i = icmp slt i32 %tmp30.i, 0
  %tmp3536.i = zext i16 %iftmp.1.0183.1.i to i32
  br i1 %tmp31.i, label %cond_true34.i, label %cond_false43.i

cond_true34.i:                                    ; preds = %bb23.i
  %tmp39.i23 = sub i32 7, %tmp2829.i
  %tmp40.i24 = shl i32 %tmp3536.i, %tmp39.i23
  br label %cond_next52.i

cond_false43.i:                                   ; preds = %bb23.i
  %tmp49.i = lshr i32 %tmp3536.i, %tmp30.i
  br label %cond_next52.i

cond_next52.i:                                    ; preds = %cond_false43.i, %cond_true34.i
  %iftmp.4.0.in.in.i = phi i32 [ %tmp49.i, %cond_false43.i ], [ %tmp40.i24, %cond_true34.i ]
  %iftmp.4.0.in.i = trunc i32 %iftmp.4.0.in.in.i to i16
  %iftmp.4.0.i = and i16 %iftmp.4.0.in.i, 127
  %tmp57.i = lshr i16 %tmp40.i, 2
  %tmp54.i = sub i16 %tmp26182.i, %tmp57.i
  %tmp59.i = add i16 %tmp54.i, %iftmp.4.0.i
  %tmp60.i = and i16 %tmp59.i, 4095
  %tmp62.i = icmp ugt i16 %tmp60.i, 299
  br i1 %tmp62.i, label %cond_true65.i, label %cond_false126.i

cond_true65.i:                                    ; preds = %cond_next52.i
  %tmp67.i = icmp ugt i16 %tmp60.i, 2047
  br i1 %tmp67.i, label %cond_true70.i, label %cond_false86.i

cond_true70.i:                                    ; preds = %cond_true65.i
  %tmp72.i = icmp ugt i16 %tmp60.i, 3971
  br i1 %tmp72.i, label %cond_true75.i, label %cond_false84.i

cond_true75.i:                                    ; preds = %cond_true70.i
  %iftmp.6.0.i = select i1 %tmp3.i16, i16 14, i16 1
  store i16 %iftmp.6.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false84.i:                                   ; preds = %cond_true70.i
  store i16 15, ptr @I, align 2
  br label %adapt_quant.exit

cond_false86.i:                                   ; preds = %cond_true65.i
  %tmp88.i = icmp ugt i16 %tmp60.i, 399
  br i1 %tmp88.i, label %cond_true91.i, label %cond_false100.i

cond_true91.i:                                    ; preds = %cond_false86.i
  %iftmp.7.0.i = select i1 %tmp3.i16, i16 8, i16 7
  store i16 %iftmp.7.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false100.i:                                  ; preds = %cond_false86.i
  %tmp102.i = icmp ugt i16 %tmp60.i, 348
  br i1 %tmp102.i, label %cond_true105.i, label %cond_false114.i

cond_true105.i:                                   ; preds = %cond_false100.i
  %iftmp.8.0.i = select i1 %tmp3.i16, i16 9, i16 6
  store i16 %iftmp.8.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false114.i:                                  ; preds = %cond_false100.i
  %iftmp.9.0.i = select i1 %tmp3.i16, i16 10, i16 5
  store i16 %iftmp.9.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false126.i:                                  ; preds = %cond_next52.i
  %tmp128.i = icmp ugt i16 %tmp60.i, 177
  br i1 %tmp128.i, label %cond_true131.i, label %cond_false155.i

cond_true131.i:                                   ; preds = %cond_false126.i
  %tmp133.i = icmp ugt i16 %tmp60.i, 245
  br i1 %tmp133.i, label %cond_true136.i, label %cond_false145.i

cond_true136.i:                                   ; preds = %cond_true131.i
  %iftmp.10.0.i = select i1 %tmp3.i16, i16 11, i16 4
  store i16 %iftmp.10.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false145.i:                                  ; preds = %cond_true131.i
  %iftmp.11.0.i = select i1 %tmp3.i16, i16 12, i16 3
  store i16 %iftmp.11.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false155.i:                                  ; preds = %cond_false126.i
  %tmp157.i = icmp ugt i16 %tmp60.i, 79
  br i1 %tmp157.i, label %cond_true160.i, label %cond_false169.i

cond_true160.i:                                   ; preds = %cond_false155.i
  %iftmp.12.0.i = select i1 %tmp3.i16, i16 13, i16 2
  store i16 %iftmp.12.0.i, ptr @I, align 2
  br label %adapt_quant.exit

cond_false169.i:                                  ; preds = %cond_false155.i
  %iftmp.13.0.i = select i1 %tmp3.i16, i16 14, i16 1
  store i16 %iftmp.13.0.i, ptr @I, align 2
  br label %adapt_quant.exit

adapt_quant.exit:                                 ; preds = %cond_false169.i, %cond_true160.i, %cond_false145.i, %cond_true136.i, %cond_false114.i, %cond_true105.i, %cond_true91.i, %cond_false84.i, %cond_true75.i
  tail call fastcc void @iadpt_quant()
  tail call fastcc void @tone_detector_1()
  tail call fastcc void @adpt_predict_2()
  %tmp.i10 = load i16, ptr @A2P, align 2
  %tmp1.i11 = xor i16 %tmp.i10, -32768
  %tmp2.i12 = icmp ult i16 %tmp1.i11, 20992
  %tmp23.i13 = zext i1 %tmp2.i12 to i16
  store i16 %tmp23.i13, ptr @TDP, align 2
  %tmp4.i = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp5.b.i = load i1, i1* @TR.b, align 1
  %iftmp.207.0.i = select i1 %tmp5.b.i, i16 0, i16 %tmp23.i13
  %tmp9.i14 = getelementptr %struct.STATES, %struct.STATES* %tmp4.i, i32 0, i32 8
  store i16 %iftmp.207.0.i, ptr %tmp9.i14, align 4
  tail call fastcc void @scale_factor_2()
  tail call fastcc void @speed_control_2()
  %tmp1 = load i16, ptr @I, align 2
  ret i16 %tmp1
}

define internal void @reset_decoder() {
entry:
  store %struct.STATES* @D_STATES, %struct.STATES** @X, align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 0), align 8
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 1), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 2), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 4), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 0), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 1), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 2), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 3), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 4), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 5), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 2), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 6), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 4), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 7), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 8), align 4
  store i32 34816, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 10), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 9), align 2
  ret void
}

define internal zeroext i16 @decoder(i16 zeroext %adpcm) {
entry:
  store i16 %adpcm, ptr @I, align 2
  store %struct.STATES* @D_STATES, %struct.STATES** @X, align 4
  %tmp2.i = load i16, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 4), align 4
  %tmp3.i = icmp ult i16 %tmp2.i, 256
  %tmp8.i = lshr i16 %tmp2.i, 2
  %storemerge = select i1 %tmp3.i, i16 %tmp8.i, i16 64
  store i16 %storemerge, ptr @AL, align 2
  %tmp3.i11 = load i16, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 7), align 2
  %tmp6.i12 = load i16, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 9), align 2
  %tmp7.i13 = sub i16 %tmp3.i11, %tmp6.i12
  %tmp9.i14 = and i16 %tmp7.i13, 8192
  %tmp11.i = icmp eq i16 %tmp9.i14, 0
  br i1 %tmp11.i, label %scale_factor_1.exit, label %cond_true.i15

cond_true.i15:                                    ; preds = %entry
  %tmp14.i = sub i16 0, %tmp7.i13
  %tmp15.i = and i16 %tmp14.i, 8191
  br label %scale_factor_1.exit

scale_factor_1.exit:                              ; preds = %cond_true.i15, %entry
  %iftmp.163.0.i = phi i16 [ %tmp15.i, %cond_true.i15 ], [ %tmp7.i13, %entry ]
  %tmp1819.i = zext i16 %iftmp.163.0.i to i32
  %tmp2021.i = zext i16 %storemerge to i32
  %tmp22.i = mul i32 %tmp2021.i, %tmp1819.i
  %tmp23.i = lshr i32 %tmp22.i, 6
  %tmp2324.i = trunc i32 %tmp23.i to i16
  %tmp34.i = sub i16 0, %tmp2324.i
  %iftmp.166.0.i = select i1 %tmp11.i, i16 %tmp2324.i, i16 %tmp34.i
  %tmp39.i17 = add i16 %iftmp.166.0.i, %tmp6.i12
  %tmp40.i18 = and i16 %tmp39.i17, 8191
  store i16 %tmp40.i18, ptr @Y, align 2
  tail call fastcc void @iadpt_quant()
  tail call fastcc void @tone_detector_1()
  tail call fastcc void @adpt_predict_1()
  tail call fastcc void @adpt_predict_2()
  %tmp.i35 = load i16, ptr @A2P, align 2
  %tmp1.i36 = xor i16 %tmp.i35, -32768
  %tmp2.i37 = icmp ult i16 %tmp1.i36, 20992
  %tmp23.i38 = zext i1 %tmp2.i37 to i16
  store i16 %tmp23.i38, ptr @TDP, align 2
  %tmp4.i = load %struct.STATES*, %struct.STATES** @X, align 4
  %tmp5.b.i = load i1, i1* @TR.b, align 1
  %iftmp.207.0.i = select i1 %tmp5.b.i, i16 0, i16 %tmp23.i38
  %tmp9.i39 = getelementptr %struct.STATES, %struct.STATES* %tmp4.i, i32 0, i32 8
  store i16 %iftmp.207.0.i, ptr %tmp9.i39, align 4
  tail call fastcc void @scale_factor_2()
  tail call fastcc void @speed_control_2()
  %tmp.i20 = load i16, ptr @SR, align 2
  %tmp3.i21 = icmp slt i16 %tmp.i20, 0
  br i1 %tmp3.i21, label %cond_true.i24, label %cond_next.i26

cond_true.i24:                                    ; preds = %scale_factor_1.exit
  %tmp6.i22 = sub i16 0, %tmp.i20
  %tmp7.i23 = and i16 %tmp6.i22, 32767
  br label %cond_next.i26

cond_next.i26:                                    ; preds = %cond_true.i24, %scale_factor_1.exit
  %iftmp.149.0.i = phi i16 [ %tmp7.i23, %cond_true.i24 ], [ %tmp.i20, %scale_factor_1.exit ]
  %tmp10.i = load i32, ptr @LAW, align 4
  %tmp11.i25 = icmp eq i32 %tmp10.i, 0
  br i1 %tmp11.i25, label %cond_false89.i, label %cond_true14.i

cond_true14.i:                                    ; preds = %cond_next.i26
  %iftmp.153.0.i = select i1 %tmp3.i21, i16 85, i16 213
  br i1 %tmp3.i21, label %cond_true27.i, label %cond_false34.i

cond_true27.i:                                    ; preds = %cond_true14.i
  %tmp2829.i27 = zext i16 %iftmp.149.0.i to i32
  %tmp30.i28 = add i32 %tmp2829.i27, 1
  %tmp31143.i = lshr i32 %tmp30.i28, 1
  %tmp3132.i = trunc i32 %tmp31143.i to i16
  %tmp33.i = add i16 %tmp3132.i, -1
  %tmp40152.i = icmp ugt i16 %tmp33.i, 4095
  br i1 %tmp40152.i, label %cond_true43.i, label %cond_false46.i

cond_false34.i:                                   ; preds = %cond_true14.i
  %tmp36.i = lshr i16 %iftmp.149.0.i, 1
  %tmp40.i29 = icmp ugt i16 %tmp36.i, 4095
  br i1 %tmp40.i29, label %cond_true43.i, label %cond_false46.i

cond_true43.i:                                    ; preds = %cond_false34.i, %cond_true27.i
  %tmp45.i = xor i16 %iftmp.153.0.i, 127
  store i16 %tmp45.i, ptr @S, align 2
  br label %output_conversion.exit

cond_false46.i:                                   ; preds = %cond_false34.i, %cond_true27.i
  %iftmp.154.0147.0.i = phi i16 [ %tmp33.i, %cond_true27.i ], [ %tmp36.i, %cond_false34.i ]
  %tmp50.i = lshr i16 %iftmp.154.0147.0.i, 4
  %tmp56158.i = lshr i16 %iftmp.154.0147.0.i, 5
  %tmp58159.i = icmp ult i16 %tmp50.i, 2
  br i1 %tmp58159.i, label %bb61.i, label %bb.i31

bb.i31:                                           ; preds = %bb.i31, %cond_false46.i
  %SEG.0153.0.i = phi i16 [ 0, %cond_false46.i ], [ %tmp52.i, %bb.i31 ]
  %tmp56157.0.i = phi i16 [ %tmp56158.i, %cond_false46.i ], [ %tmp56.i, %bb.i31 ]
  %tmp52.i = add i16 %SEG.0153.0.i, 1
  %tmp56.i = lshr i16 %tmp56157.0.i, 1
  %tmp58.i30 = icmp ult i16 %tmp56157.0.i, 2
  br i1 %tmp58.i30, label %bb61.i, label %bb.i31

bb61.i:                                           ; preds = %bb.i31, %cond_false46.i
  %SEG.0153.1.i = phi i16 [ 0, %cond_false46.i ], [ %tmp52.i, %bb.i31 ]
  %tmp64144.i = shl i16 %SEG.0153.1.i, 4
  %tmp67.i = icmp eq i16 %SEG.0153.1.i, 0
  br i1 %tmp67.i, label %cond_next83.i, label %cond_true70.i

cond_true70.i:                                    ; preds = %bb61.i
  %tmp7172.i = zext i16 %iftmp.154.0147.0.i to i32
  %tmp7374.i = zext i16 %SEG.0153.1.i to i32
  %tmp75.i32 = lshr i32 %tmp7172.i, %tmp7374.i
  %tmp7576.i = trunc i32 %tmp75.i32 to i16
  %iftmp.155.0160.i = and i16 %tmp7576.i, 15
  %tmp85161.i = or i16 %iftmp.155.0160.i, %tmp64144.i
  %tmp87162.i = xor i16 %tmp85161.i, %iftmp.153.0.i
  store i16 %tmp87162.i, ptr @S, align 2
  br label %output_conversion.exit

cond_next83.i:                                    ; preds = %bb61.i
  %tmp81.i = lshr i16 %iftmp.154.0147.0.i, 1
  %iftmp.155.0.i = and i16 %tmp81.i, 15
  %tmp85.i33 = or i16 %tmp64144.i, %iftmp.155.0.i
  %tmp87.i = xor i16 %tmp85.i33, %iftmp.153.0.i
  store i16 %tmp87.i, ptr @S, align 2
  br label %output_conversion.exit

cond_false89.i:                                   ; preds = %cond_next.i26
  %iftmp.157.0.i = select i1 %tmp3.i21, i16 127, i16 255
  %tmp102.i = add i16 %iftmp.149.0.i, 33
  %tmp104.i = icmp ugt i16 %tmp102.i, 8191
  br i1 %tmp104.i, label %cond_true107.i, label %bb114.preheader.i

bb114.preheader.i:                                ; preds = %cond_false89.i
  %tmp115116164.i = zext i16 %tmp102.i to i32
  %tmp119166.mask.i = and i32 %tmp115116164.i, 65504
  %tmp120167.i = icmp eq i32 %tmp119166.mask.i, 0
  br i1 %tmp120167.i, label %bb123.i, label %bb111.i

cond_true107.i:                                   ; preds = %cond_false89.i
  %tmp146.i34 = and i16 %iftmp.157.0.i, 128
  store i16 %tmp146.i34, ptr @S, align 2
  br label %output_conversion.exit

bb111.i:                                          ; preds = %bb111.i, %bb114.preheader.i
  %indvar.i = phi i16 [ 0, %bb114.preheader.i ], [ %indvar.next54, %bb111.i ]
  %tmp113.i = add i16 %indvar.i, 6
  %tmp117118.i = zext i16 %tmp113.i to i32
  %tmp119.i = lshr i32 %tmp115116164.i, %tmp117118.i
  %tmp120.i = icmp eq i32 %tmp119.i, 0
  %indvar.next54 = add i16 %indvar.i, 1
  br i1 %tmp120.i, label %bb123.i, label %bb111.i

bb123.i:                                          ; preds = %bb111.i, %bb114.preheader.i
  %SEG92.0.lcssa.i = phi i16 [ -1, %bb114.preheader.i ], [ %indvar.i, %bb111.i ]
  %tmp128145.i = shl i16 %SEG92.0.lcssa.i, 4
  %tmp132133.i = zext i16 %SEG92.0.lcssa.i to i32
  %tmp134.i = add i32 %tmp132133.i, 1
  %tmp135.i = lshr i32 %tmp115116164.i, %tmp134.i
  %tmp135136.i = trunc i32 %tmp135.i to i16
  %tmp137.i = and i16 %tmp135136.i, 15
  %tmp138.i = or i16 %tmp137.i, %tmp128145.i
  %tmp140.i = xor i16 %tmp138.i, %iftmp.157.0.i
  store i16 %tmp140.i, ptr @S, align 2
  br label %output_conversion.exit

output_conversion.exit:                           ; preds = %bb123.i, %cond_true107.i, %cond_next83.i, %cond_true70.i, %cond_true43.i
  tail call fastcc void @input_conversion()
  %tmp.i5 = load i16, ptr @SL, align 2
  %tmp1.i6 = zext i16 %tmp.i5 to i32
  %tmp3429.i = and i32 %tmp1.i6, 8192
  %toBool.i = icmp eq i32 %tmp3429.i, 0
  %tmp6.i7 = select i1 %toBool.i, i16 0, i16 -16384
  %tmp9.i = load i16, ptr @SE, align 2
  %tmp910.i = zext i16 %tmp9.i to i32
  %tmp121327.i = and i32 %tmp910.i, 16384
  %toBool14.i = icmp eq i32 %tmp121327.i, 0
  %tmp17.i8 = select i1 %toBool14.i, i16 0, i16 -32768
  %iftmp.132.0.i = xor i16 %tmp17.i8, %tmp9.i
  %iftmp.128.0.i = add i16 %tmp6.i7, %tmp.i5
  %tmp24.i = sub i16 %iftmp.128.0.i, %iftmp.132.0.i
  store i16 %tmp24.i, ptr @D, align 2
  %tmp3.i2 = icmp slt i16 %tmp24.i, 0
  br i1 %tmp3.i2, label %cond_true.i3, label %cond_next.i4

cond_true.i3:                                     ; preds = %output_conversion.exit
  %tmp6.i = sub i16 0, %tmp24.i
  %tmp7.i = and i16 %tmp6.i, 32767
  %tmp20357.i = icmp ult i16 %tmp7.i, 2
  br i1 %tmp20357.i, label %bb23.i, label %bb.i

cond_next.i4:                                     ; preds = %output_conversion.exit
  %tmp20359.i = icmp ult i16 %tmp24.i, 2
  br i1 %tmp20359.i, label %bb23.i, label %bb.i

bb.i:                                             ; preds = %bb.i, %cond_next.i4, %cond_true.i3
  %iftmp.79.0348.0.ph.ph.i.ph = phi i16 [ %tmp7.i, %cond_true.i3 ], [ %tmp24.i, %cond_next.i4 ], [ %iftmp.79.0348.0.ph.ph.i.ph, %bb.i ]
  %EXP.0351.0.i = phi i16 [ %tmp17.i, %bb.i ], [ 0, %cond_next.i4 ], [ 0, %cond_true.i3 ]
  %tmp10.0355.0.in.i = phi i16 [ %tmp10.0355.0.i, %bb.i ], [ %tmp7.i, %cond_true.i3 ], [ %tmp24.i, %cond_next.i4 ]
  %tmp10.0355.0.i = lshr i16 %tmp10.0355.0.in.i, 1
  %tmp17.i = add i16 %EXP.0351.0.i, 1
  %tmp20.i = icmp ult i16 %tmp10.0355.0.i, 2
  br i1 %tmp20.i, label %bb23.i, label %bb.i

bb23.i:                                           ; preds = %bb.i, %cond_next.i4, %cond_true.i3
  %iftmp.79.0348.1.i = phi i16 [ %tmp7.i, %cond_true.i3 ], [ %tmp24.i, %cond_next.i4 ], [ %iftmp.79.0348.0.ph.ph.i.ph, %bb.i ]
  %EXP.0351.1.i = phi i16 [ 0, %cond_true.i3 ], [ 0, %cond_next.i4 ], [ %tmp17.i, %bb.i ]
  %tmp26347.i = shl i16 %EXP.0351.1.i, 7
  %tmp2829.i = sext i16 %EXP.0351.1.i to i32
  %tmp30.i = add i32 %tmp2829.i, -7
  %tmp31.i = icmp slt i32 %tmp30.i, 0
  %tmp3536.i = zext i16 %iftmp.79.0348.1.i to i32
  br i1 %tmp31.i, label %cond_true34.i, label %cond_false43.i

cond_true34.i:                                    ; preds = %bb23.i
  %tmp39.i = sub i32 7, %tmp2829.i
  %tmp40.i = shl i32 %tmp3536.i, %tmp39.i
  br label %cond_next52.i

cond_false43.i:                                   ; preds = %bb23.i
  %tmp49.i = lshr i32 %tmp3536.i, %tmp30.i
  br label %cond_next52.i

cond_next52.i:                                    ; preds = %cond_false43.i, %cond_true34.i
  %iftmp.82.0.in.in.i = phi i32 [ %tmp49.i, %cond_false43.i ], [ %tmp40.i, %cond_true34.i ]
  %iftmp.82.0.in.i = trunc i32 %iftmp.82.0.in.in.i to i16
  %iftmp.82.0.i = and i16 %iftmp.82.0.in.i, 127
  %tmp55.i = load i16, ptr @Y, align 2
  %tmp57.i = lshr i16 %tmp55.i, 2
  %tmp54.i = sub i16 %tmp26347.i, %tmp57.i
  %tmp58.i = add i16 %tmp54.i, %iftmp.82.0.i
  %tmp59.i = and i16 %tmp58.i, 4095
  %tmp60.i = load i16, ptr @I, align 2
  %tmp6061.i = zext i16 %tmp60.i to i32
  %tmp6364346.i = and i32 %tmp6061.i, 8
  %toBool65.i = icmp eq i32 %tmp6364346.i, 0
  %tmp71.i = add i16 %tmp60.i, 8
  %tmp68.i = and i16 %tmp60.i, 7
  %iftmp.84.0.i = select i1 %toBool65.i, i16 %tmp71.i, i16 %tmp68.i
  %tmp75.i = icmp ugt i16 %tmp59.i, 299
  br i1 %tmp75.i, label %cond_true78.i, label %cond_false139.i

cond_true78.i:                                    ; preds = %cond_next52.i
  %tmp80.i = icmp ugt i16 %tmp59.i, 2047
  br i1 %tmp80.i, label %cond_true83.i, label %cond_false99.i

cond_true83.i:                                    ; preds = %cond_true78.i
  %tmp85.i = icmp ugt i16 %tmp59.i, 3971
  %tmp85.not.i = xor i1 %tmp85.i, true
  %brmerge.i = or i1 %tmp3.i2, %tmp85.not.i
  %.mux.i = select i1 %tmp85.i, i16 6, i16 7
  br i1 %brmerge.i, label %cond_next193.i, label %cond_false94.i

cond_false94.i:                                   ; preds = %cond_true83.i
  %tmp194366.i = load i32, ptr @LAW, align 4
  %tmp195367.i = icmp eq i32 %tmp194366.i, 0
  %tmp199368.i = load i16, ptr @S, align 2
  br i1 %tmp195367.i, label %cond_false271.i, label %cond_true198.i

cond_false99.i:                                   ; preds = %cond_true78.i
  %tmp101.i = icmp ugt i16 %tmp59.i, 399
  br i1 %tmp101.i, label %cond_true104.i, label %cond_false113.i

cond_true104.i:                                   ; preds = %cond_false99.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false110.i

cond_false110.i:                                  ; preds = %cond_true104.i
  %tmp194369.i = load i32, ptr @LAW, align 4
  %tmp195370.i = icmp eq i32 %tmp194369.i, 0
  %tmp199371.i = load i16, ptr @S, align 2
  br i1 %tmp195370.i, label %cond_false271.i, label %cond_true198.i

cond_false113.i:                                  ; preds = %cond_false99.i
  %tmp115.i = icmp ugt i16 %tmp59.i, 348
  br i1 %tmp115.i, label %cond_true118.i, label %cond_false127.i

cond_true118.i:                                   ; preds = %cond_false113.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false124.i

cond_false124.i:                                  ; preds = %cond_true118.i
  %tmp194372.i = load i32, ptr @LAW, align 4
  %tmp195373.i = icmp eq i32 %tmp194372.i, 0
  %tmp199374.i = load i16, ptr @S, align 2
  br i1 %tmp195373.i, label %cond_false271.i, label %cond_true198.i

cond_false127.i:                                  ; preds = %cond_false113.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false133.i

cond_false133.i:                                  ; preds = %cond_false127.i
  %tmp194375.i = load i32, ptr @LAW, align 4
  %tmp195376.i = icmp eq i32 %tmp194375.i, 0
  %tmp199377.i = load i16, ptr @S, align 2
  br i1 %tmp195376.i, label %cond_false271.i, label %cond_true198.i

cond_false139.i:                                  ; preds = %cond_next52.i
  %tmp141.i = icmp ugt i16 %tmp59.i, 177
  br i1 %tmp141.i, label %cond_true144.i, label %cond_false168.i

cond_true144.i:                                   ; preds = %cond_false139.i
  %tmp146.i = icmp ugt i16 %tmp59.i, 245
  br i1 %tmp146.i, label %cond_true149.i, label %cond_false158.i

cond_true149.i:                                   ; preds = %cond_true144.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false155.i

cond_false155.i:                                  ; preds = %cond_true149.i
  %tmp194378.i = load i32, ptr @LAW, align 4
  %tmp195379.i = icmp eq i32 %tmp194378.i, 0
  %tmp199380.i = load i16, ptr @S, align 2
  br i1 %tmp195379.i, label %cond_false271.i, label %cond_true198.i

cond_false158.i:                                  ; preds = %cond_true144.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false164.i

cond_false164.i:                                  ; preds = %cond_false158.i
  %tmp194381.i = load i32, ptr @LAW, align 4
  %tmp195382.i = icmp eq i32 %tmp194381.i, 0
  %tmp199383.i = load i16, ptr @S, align 2
  br i1 %tmp195382.i, label %cond_false271.i, label %cond_true198.i

cond_false168.i:                                  ; preds = %cond_false139.i
  %tmp170.i = icmp ugt i16 %tmp59.i, 79
  br i1 %tmp170.i, label %cond_true173.i, label %cond_false182.i

cond_true173.i:                                   ; preds = %cond_false168.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false179.i

cond_false179.i:                                  ; preds = %cond_true173.i
  %tmp194384.i = load i32, ptr @LAW, align 4
  %tmp195385.i = icmp eq i32 %tmp194384.i, 0
  %tmp199386.i = load i16, ptr @S, align 2
  br i1 %tmp195385.i, label %cond_false271.i, label %cond_true198.i

cond_false182.i:                                  ; preds = %cond_false168.i
  br i1 %tmp3.i2, label %cond_next193.i, label %cond_false188.i

cond_false188.i:                                  ; preds = %cond_false182.i
  br label %cond_next193.i

cond_next193.i:                                   ; preds = %cond_false188.i, %cond_false182.i, %cond_true173.i, %cond_false158.i, %cond_true149.i, %cond_false127.i, %cond_true118.i, %cond_true104.i, %cond_true83.i
  %ID.0.i = phi i16 [ 9, %cond_false188.i ], [ %.mux.i, %cond_true83.i ], [ 0, %cond_true104.i ], [ 1, %cond_true118.i ], [ 2, %cond_false127.i ], [ 3, %cond_true149.i ], [ 4, %cond_false158.i ], [ 5, %cond_true173.i ], [ 6, %cond_false182.i ]
  %tmp194.i = load i32, ptr @LAW, align 4
  %tmp195.i = icmp eq i32 %tmp194.i, 0
  %tmp199.i = load i16, ptr @S, align 2
  br i1 %tmp195.i, label %cond_false271.i, label %cond_true198.i

cond_true198.i:                                   ; preds = %cond_next193.i, %cond_false179.i, %cond_false164.i, %cond_false155.i, %cond_false133.i, %cond_false124.i, %cond_false110.i, %cond_false94.i
  %ID.0360.0.i = phi i16 [ 9, %cond_false94.i ], [ 15, %cond_false110.i ], [ 14, %cond_false124.i ], [ 13, %cond_false133.i ], [ 12, %cond_false155.i ], [ 11, %cond_false164.i ], [ 10, %cond_false179.i ], [ %ID.0.i, %cond_next193.i ]
  %tmp199364.0.i = phi i16 [ %tmp199368.i, %cond_false94.i ], [ %tmp199371.i, %cond_false110.i ], [ %tmp199374.i, %cond_false124.i ], [ %tmp199377.i, %cond_false133.i ], [ %tmp199380.i, %cond_false155.i ], [ %tmp199383.i, %cond_false164.i ], [ %tmp199386.i, %cond_false179.i ], [ %tmp199.i, %cond_next193.i ]
  %tmp200.i = xor i16 %tmp199364.0.i, 85
  store i16 %tmp200.i, ptr @SD, align 2
  %tmp203.i = icmp ugt i16 %ID.0360.0.i, %iftmp.84.0.i
  br i1 %tmp203.i, label %cond_true206.i, label %cond_false233.i

cond_true206.i:                                   ; preds = %cond_true198.i
  %tmp208.i = icmp ult i16 %tmp200.i, 127
  br i1 %tmp208.i, label %cond_true211.i, label %cond_false214.i

cond_true211.i:                                   ; preds = %cond_true206.i
  %tmp213.i = add i16 %tmp200.i, 1
  %tmp270388.i = xor i16 %tmp213.i, 85
  store i16 %tmp270388.i, ptr @SD, align 2
  ret i16 %tmp270388.i

cond_false214.i:                                  ; preds = %cond_true206.i
  %tmp216.i = icmp ugt i16 %tmp200.i, 128
  br i1 %tmp216.i, label %cond_true219.i, label %cond_false222.i

cond_true219.i:                                   ; preds = %cond_false214.i
  %tmp221.i = add i16 %tmp200.i, -1
  %tmp270390.i = xor i16 %tmp221.i, 85
  store i16 %tmp270390.i, ptr @SD, align 2
  ret i16 %tmp270390.i

cond_false222.i:                                  ; preds = %cond_false214.i
  %tmp224.i = icmp eq i16 %tmp199364.0.i, 213
  %tmp270392.i = select i1 %tmp224.i, i16 85, i16 42
  store i16 %tmp270392.i, ptr @SD, align 2
  ret i16 %tmp270392.i

cond_false233.i:                                  ; preds = %cond_true198.i
  %tmp236.i = icmp ult i16 %ID.0360.0.i, %iftmp.84.0.i
  br i1 %tmp236.i, label %cond_true239.i, label %cond_next268.i

cond_true239.i:                                   ; preds = %cond_false233.i
  %tmp241.i = icmp ult i16 %tmp200.i, 128
  br i1 %tmp241.i, label %cond_true244.i, label %cond_false255.i

cond_true244.i:                                   ; preds = %cond_true239.i
  %tmp246.i = icmp eq i16 %tmp199364.0.i, 85
  br i1 %tmp246.i, label %cond_next253.i, label %cond_true249.i

cond_true249.i:                                   ; preds = %cond_true244.i
  %tmp251.i = add i16 %tmp200.i, -1
  %tmp270394.i = xor i16 %tmp251.i, 85
  store i16 %tmp270394.i, ptr @SD, align 2
  ret i16 %tmp270394.i

cond_next253.i:                                   ; preds = %cond_true244.i
  store i16 213, ptr @SD, align 2
  ret i16 213

cond_false255.i:                                  ; preds = %cond_true239.i
  %tmp257.i = icmp eq i16 %tmp199364.0.i, 170
  br i1 %tmp257.i, label %cond_next264.i, label %cond_true260.i

cond_true260.i:                                   ; preds = %cond_false255.i
  %tmp262.i = add i16 %tmp200.i, 1
  %phitmp.i = xor i16 %tmp262.i, 85
  store i16 %phitmp.i, ptr @SD, align 2
  ret i16 %phitmp.i

cond_next264.i:                                   ; preds = %cond_false255.i
  store i16 170, ptr @SD, align 2
  ret i16 170

cond_next268.i:                                   ; preds = %cond_false233.i
  store i16 %tmp199364.0.i, ptr @SD, align 2
  ret i16 %tmp199364.0.i

cond_false271.i:                                  ; preds = %cond_next193.i, %cond_false179.i, %cond_false164.i, %cond_false155.i, %cond_false133.i, %cond_false124.i, %cond_false110.i, %cond_false94.i
  %tmp279.rle.i = phi i16 [ %tmp199368.i, %cond_false94.i ], [ %tmp199371.i, %cond_false110.i ], [ %tmp199374.i, %cond_false124.i ], [ %tmp199377.i, %cond_false133.i ], [ %tmp199380.i, %cond_false155.i ], [ %tmp199383.i, %cond_false164.i ], [ %tmp199386.i, %cond_false179.i ], [ %tmp199.i, %cond_next193.i ]
  %ID.0360.1.i = phi i16 [ 9, %cond_false94.i ], [ 15, %cond_false110.i ], [ 14, %cond_false124.i ], [ 13, %cond_false133.i ], [ 12, %cond_false155.i ], [ 11, %cond_false164.i ], [ 10, %cond_false179.i ], [ %ID.0.i, %cond_next193.i ]
  %tmp199364.1.i = phi i16 [ %tmp199368.i, %cond_false94.i ], [ %tmp199371.i, %cond_false110.i ], [ %tmp199374.i, %cond_false124.i ], [ %tmp199377.i, %cond_false133.i ], [ %tmp199380.i, %cond_false155.i ], [ %tmp199383.i, %cond_false164.i ], [ %tmp199386.i, %cond_false179.i ], [ %tmp199.i, %cond_next193.i ]
  store i16 %tmp199364.1.i, ptr @SD, align 2
  %tmp275.i = icmp ugt i16 %ID.0360.1.i, %iftmp.84.0.i
  br i1 %tmp275.i, label %cond_true278.i, label %cond_false307.i

cond_true278.i:                                   ; preds = %cond_false271.i
  %tmp280.i = add i16 %tmp279.rle.i, -1
  %tmp281.i = icmp ult i16 %tmp280.i, 127
  br i1 %tmp281.i, label %cond_true284.i, label %cond_false287.i

cond_true284.i:                                   ; preds = %cond_true278.i
  %tmp286.i = add i16 %tmp199364.1.i, -1
  store i16 %tmp286.i, ptr @SD, align 2
  ret i16 %tmp286.i

cond_false287.i:                                  ; preds = %cond_true278.i
  %tmp289.i = add i16 %tmp279.rle.i, -128
  %tmp290.i = icmp ult i16 %tmp289.i, 127
  br i1 %tmp290.i, label %cond_true293.i, label %cond_false296.i

cond_true293.i:                                   ; preds = %cond_false287.i
  %tmp295.i = add i16 %tmp199364.1.i, 1
  store i16 %tmp295.i, ptr @SD, align 2
  ret i16 %tmp295.i

cond_false296.i:                                  ; preds = %cond_false287.i
  %tmp298.i = icmp eq i16 %tmp279.rle.i, 0
  %iftmp.120.0.i = select i1 %tmp298.i, i16 0, i16 126
  store i16 %iftmp.120.0.i, ptr @SD, align 2
  ret i16 %iftmp.120.0.i

cond_false307.i:                                  ; preds = %cond_false271.i
  %tmp310.i = icmp ult i16 %ID.0360.1.i, %iftmp.84.0.i
  br i1 %tmp310.i, label %cond_true313.i, label %UnifiedReturnBlock

cond_true313.i:                                   ; preds = %cond_false307.i
  %tmp315.i = icmp ult i16 %tmp279.rle.i, 127
  br i1 %tmp315.i, label %cond_true318.i, label %cond_false321.i

cond_true318.i:                                   ; preds = %cond_true313.i
  %tmp320.i = add i16 %tmp199364.1.i, 1
  store i16 %tmp320.i, ptr @SD, align 2
  ret i16 %tmp320.i

cond_false321.i:                                  ; preds = %cond_true313.i
  %tmp323.i = add i16 %tmp279.rle.i, -129
  %tmp324.i = icmp ult i16 %tmp323.i, 127
  br i1 %tmp324.i, label %cond_true327.i, label %cond_false330.i

cond_true327.i:                                   ; preds = %cond_false321.i
  %tmp329.i = add i16 %tmp199364.1.i, -1
  store i16 %tmp329.i, ptr @SD, align 2
  ret i16 %tmp329.i

cond_false330.i:                                  ; preds = %cond_false321.i
  %tmp332.i = icmp eq i16 %tmp279.rle.i, 127
  %iftmp.126.0.i = select i1 %tmp332.i, i16 254, i16 128
  store i16 %iftmp.126.0.i, ptr @SD, align 2
  ret i16 %iftmp.126.0.i

UnifiedReturnBlock:                               ; preds = %cond_false307.i
  ret i16 %tmp199364.1.i
}

define internal i32 @main(i32 %argc, ptr %argv) {
entry:
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 0), align 8
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 1), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 2), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 4), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 0, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 0), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 1), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 2), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 3), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 4), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 5), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 1, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 2), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 6), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 4), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 7), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 8), align 4
  store i32 34816, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 10), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @E_STATES, i32 0, i32 9), align 2
  store %struct.STATES* @D_STATES, %struct.STATES** @X, align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 0), align 8
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 1), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 2), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 4), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 0, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 0), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 1), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 2), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 3), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 4), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 5), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 7), align 2
  store i16 32, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 1, i32 6), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 3), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 2), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 6), align 4
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 5), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 4), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 7), align 2
  store i16 0, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 8), align 4
  store i32 34816, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 10), align 4
  store i16 544, ptr getelementptr inbounds (%struct.STATES, %struct.STATES* @D_STATES, i32 0, i32 9), align 2
  br label %bb

bb:                                               ; preds = %bb, %entry
  %i.016.0 = phi i32 [ 0, %entry ], [ %indvar.next1, %bb ]
  %tmp2 = getelementptr [32 x i16], [32 x i16]* @Input, i32 0, i32 %i.016.0
  %tmp3 = load i16, ptr %tmp2, align 2
  %tmp4 = tail call zeroext i16 @encoder(i16 zeroext %tmp3)
  %tmp5 = tail call zeroext i16 @decoder(i16 zeroext %tmp4)
  store i16 %tmp5, ptr %tmp2, align 2
  %indvar.next1 = add i32 %i.016.0, 1
  %exitcond2 = icmp eq i32 %indvar.next1, 32
  br i1 %exitcond2, label %bb13, label %bb

bb13:                                             ; preds = %bb
  ret i32 0
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
