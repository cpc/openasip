//
// TCE Baseopset instruction information.
//
// Author: Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
// Author: Mikael Lepistö 2009 (mikael.lepisto@tut.fi)
//


include "TCEInstrFormats.td"

// Addressing modes.
def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex], []>;

// Address operands
def MEMrr : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops I32Regs, I32Regs);
}
def MEMri : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops I32Regs, i32imm);
}

// Branch targets have OtherVT type.
def brtarget : Operand<OtherVT>;
def calltarget : Operand<i32>;

// Operand for printing out a condition code.
let PrintMethod = "printCCOperand" in
  def CCOp : Operand<i32>;

def SDTTCEFTOI :
SDTypeProfile<1, 1, [SDTCisVT<0, f32>, SDTCisFP<1>]>;

def SDTTCEITOF :
SDTypeProfile<1, 1, [SDTCisFP<0>, SDTCisVT<1, f32>]>;

// not used for anything?
def TCEhi    : SDNode<"TCEISD::Hi", SDTIntUnaryOp>;
def TCElo    : SDNode<"TCEISD::Lo", SDTIntUnaryOp>;

def TCEftoi  : SDNode<"TCEISD::FTOI", SDTTCEFTOI>;
def TCEitof  : SDNode<"TCEISD::ITOF", SDTTCEITOF>;

// These are target-independent nodes, but have target-specific formats.
def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32> ]>;
def SDT_TCECallSeqEnd   : SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                        SDTCisVT<1, i32> ]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_TCECallSeqStart,
                           [SDNPHasChain, SDNPOutFlag]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_TCECallSeqEnd,
                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;

def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;

def call          : SDNode<"TCEISD::CALL", SDT_TCECall,
                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;

def retflag       : SDNode<"TCEISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInFlag]>;

// ==========================================================
//  SELECT
//  Selects between two values based on true/false condition
//  and puts the selected value in result register.
//  Expanded by the scheduler into a branch sequence.
// ==========================================================


def SDT_TCESelect : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>,
                                         SDTCisSameAs<1, 2>,
                                         SDTCisVT<3, i1>]>;

def TCEselectI1 : SDNode<"TCEISD::SELECT_I1", SDT_TCESelect, []>;
def TCEselectI8 : SDNode<"TCEISD::SELECT_I8", SDT_TCESelect, []>;
def TCEselectI16 : SDNode<"TCEISD::SELECT_I16", SDT_TCESelect, []>;
def TCEselectI32 : SDNode<"TCEISD::SELECT_I32", SDT_TCESelect, []>;
def TCEselectI64 : SDNode<"TCEISD::SELECT_I64", SDT_TCESelect, []>;
def TCEselectF32 : SDNode<"TCEISD::SELECT_F32", SDT_TCESelect, []>;
def TCEselectF64 : SDNode<"TCEISD::SELECT_F64", SDT_TCESelect, []>;

  def SELECT_I1 : InstTCE<(outs I1Regs:$dst),
                          (ins I1Regs:$c, I1Regs:$T, I1Regs:$F),
                          "# SELECT_I1 PSEUDO!",
                          [(set I1Regs:$dst, 
    (select I1Regs:$c, I1Regs:$T, I1Regs:$F))]>;

  def SELECT_I8 : InstTCE<(outs I8Regs:$dst),
                          (ins I1Regs:$c, I8Regs:$T, I8Regs:$F),
                          "# SELECT_I8 PSEUDO!",
                          [(set I8Regs:$dst,
                            (select I1Regs:$c, I8Regs:$T, I8Regs:$F))]>;

  def SELECT_I16 : InstTCE<(outs I16Regs:$dst),
                          (ins I1Regs:$c, I16Regs:$T, I16Regs:$F),
                          "# SELECT_I16 PSEUDO!",
                          [(set I16Regs:$dst,
                           (select I1Regs:$c, I16Regs:$T, I16Regs:$F))
                          ]>;

  def SELECT_I32 : InstTCE<(outs I32Regs:$dst),
                          (ins I1Regs:$c, I32Regs:$T, I32Regs:$F),
                          "# SELECT_I32 PSEUDO!",
                          [(set I32Regs:$dst, 
                            (select I1Regs:$c, I32Regs:$T, I32Regs:$F))]>;


  def SELECT_I64 : InstTCE<(outs I64Regs:$dst),
                          (ins I1Regs:$c, I64Regs:$T, I64Regs:$F),
                          "# SELECT_I32 PSEUDO!",
                          [(set I64Regs:$dst, 
                            (select I1Regs:$c, I64Regs:$T, I64Regs:$F))]>;

  def SELECT_F32 : InstTCE<(outs F32Regs:$dst),
                          (ins I1Regs:$c, F32Regs:$T, F32Regs:$F),
                          "# SELECT_F32 PSEUDO!",
                          [(set F32Regs:$dst, 
                            (select I1Regs:$c, F32Regs:$T, F32Regs:$F))]>;

  // 64-bit ints not supported.


  def SELECT_F64 : InstTCE<(outs F64Regs:$dst),
                          (ins I1Regs:$c, F64Regs:$T, F64Regs:$F),
                          "# SELECT_F64 PSEUDO!",
                          [(set F64Regs:$dst,
                            (select I1Regs:$c, F64Regs:$T, F64Regs:$F))]>;


// =========================
//  Pseudo instructions
// =========================


let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),
                               "# ADJCALLSTACKDOWN $amt",
                               [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            "# ADJCALLSTACKUP $amt1",
                            [(callseq_end timm:$amt1, timm:$amt2)]>;
}

// =========================
//  Moves
// =========================


// Int -> Int register move.
def MOVI1rr : InstTCE<(outs I1Regs:$dst), (ins I1Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI1ri : InstTCE<(outs I1Regs:$dst), (ins i1imm:$src),
	           "$src -> $dst;",
                   []>;

def MOVI8rr : InstTCE<(outs I8Regs:$dst), (ins I8Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI8ri : InstTCE<(outs I8Regs:$dst), (ins i8imm:$src),
	           "$src -> $dst;",
                   []>;

def MOVI16rr : InstTCE<(outs I16Regs:$dst), (ins I16Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI16ri : InstTCE<(outs I16Regs:$dst), (ins i16imm:$src),
	           "$src -> $dst;",
                   []>;

def MOVI32rr : InstTCE<(outs I32Regs:$dst), (ins I32Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI32ri : InstTCE<(outs I32Regs:$dst), (ins i32imm:$src),
	           "$src -> $dst;",
                   []>;

def MOVI64rr : InstTCE<(outs I64Regs:$dst), (ins I64Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI64ri : InstTCE<(outs I64Regs:$dst), (ins i64imm:$src),
	           "$src -> $dst;",
                   []>;


// --- Truncating integer register moves. ---
def MOVI8I1rr : InstTCE<(outs I1Regs:$dst), (ins I8Regs:$src),
                     "$src -> $dst;",
                    [(set I1Regs:$dst, (trunc I8Regs:$src))]>;

def MOVI16I1rr : InstTCE<(outs I1Regs:$dst), (ins I16Regs:$src),
                     "$src -> $dst;",
                    [(set I1Regs:$dst, (trunc I16Regs:$src))]>;

def MOVI16I8rr : InstTCE<(outs I8Regs:$dst), (ins I16Regs:$src),
                     "$src -> $dst;",
                    [(set I8Regs:$dst, (trunc I16Regs:$src))]>;

def MOVI32I1rr : InstTCE<(outs I1Regs:$dst), (ins I32Regs:$src),
                     "$src -> $dst;",
                    [(set I1Regs:$dst, (trunc I32Regs:$src))]>;

def MOVI32I8rr : InstTCE<(outs I8Regs:$dst), (ins I32Regs:$src),
                     "$src -> $dst;",
                    [(set I8Regs:$dst, (trunc I32Regs:$src))]>;

def MOVI32I16rr : InstTCE<(outs I16Regs:$dst), (ins I32Regs:$src),
                     "$src -> $dst;",
                    [(set I16Regs:$dst, (trunc I32Regs:$src))]>;

def MOVI64I1rr : InstTCE<(outs I1Regs:$dst), (ins I64Regs:$src),
                     "$src -> $dst;",
                    [(set I1Regs:$dst, (trunc I64Regs:$src))]>;

def MOVI64I8rr : InstTCE<(outs I8Regs:$dst), (ins I64Regs:$src),
                     "$src -> $dst;",
                    [(set I8Regs:$dst, (trunc I64Regs:$src))]>;

def MOVI64I16rr : InstTCE<(outs I16Regs:$dst), (ins I64Regs:$src),
                     "$src -> $dst;",
                    [(set I16Regs:$dst, (trunc I64Regs:$src))]>;

def MOVI64I32rr : InstTCE<(outs I32Regs:$dst), (ins I64Regs:$src),
                     "$src -> $dst;",
                    [(set I32Regs:$dst, (trunc I64Regs:$src))]>;

def MOVFI32rr : InstTCE<(outs I32Regs:$dst), (ins F32Regs:$src),
	           "$src -> $dst;",
                   [(set I32Regs:$dst, (bitconvert F32Regs:$src))]>;

def MOVIF32rr : InstTCE<(outs F32Regs:$dst), (ins I32Regs:$src),
	           "$src -> $dst;",
                   [(set F32Regs:$dst, (bitconvert I32Regs:$src))]>;

def MOVF32rr : InstTCE<(outs F32Regs:$dst), (ins F32Regs:$src),
	           "$src -> $dst;",
                   []>;
	
def MOVF32ri : InstTCE<(outs F32Regs:$dst), (ins i32imm:$src),
	           "$src -> $dst;",
                   []>;

def MOVF64rr : InstTCE<(outs F64Regs:$dst), (ins F64Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVF64ri : InstTCE<(outs F64Regs:$dst), (ins i64imm:$src),
	           "$src -> $dst;",
                   []>;

def NOP : InstTCE<(outs), (ins), "...;", []>;

// zero extending moves

def ANDext : InstTCE<(outs I32Regs:$dst), (ins I1Regs:$src, i32imm:$val),
	"", []>;

def XORbicmp: InstTCE<(outs I1Regs:$dst),(ins I1Regs:$src, i32imm:$val),
    "", []>;

// (set I32Regs:$dst, (zext I1Regs:$src))]>;

def: Pat <(i32 (anyext I1Regs:$src)), (ANDext I1Regs:$src, 1)>;
def: Pat <(i32 (zext I1Regs:$src)), (ANDext I1Regs:$src, 1)>;


// for some reason it cannot automatically use zext for anyext,
// so pattern here for it.
//def : Pat <(i32 (anyext I1Regs:$src)),(MOVI1I32rr I1Regs:$src)>;

// ----- Immediates -----
def : Pat<(i1 imm:$val), (MOVI1ri imm:$val)>;
def : Pat<(i32 imm:$val), (MOVI32ri imm:$val)>;
def : Pat<(i64 imm:$val), (MOVI64ri imm:$val)>;

// ----- Global addresses, constant pool entries ------
def TCEGlobalAddr : SDNode<"TCEISD::GLOBAL_ADDR", SDTIntUnaryOp>;
def TCEConstPool : SDNode<"TCEISD::CONST_POOL", SDTIntUnaryOp>;
def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;
def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI32ri tconstpool:$in)>;
def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;
def : Pat<(TCEConstPool tconstpool:$in), (MOVI32ri tconstpool:$in)>;

// ============================
//  Control flow
// ============================

// Return
let isReturn = 1, isTerminator = 1, 
    hasDelaySlot = 1, isBarrier = 1 in {
    def RETL: InstTCE<(outs), (ins),
   	"return-address -> jump.1; # RETURN", [(retflag)]>;
}

// jump 
let isTerminator = 1, isBranch = 1 in {
    def TCEBRCOND : InstTCE<(outs), (ins I1Regs:$gr, i32imm:$dst),
        "? $gr $dst -> jump.1;", []>;

    def TCEBR : InstTCE<(outs), (ins i32imm:$dst), "$dst -> jump.1;", []>;
}

include "GenInstrInfo.td"

// boolean store and load
def STQBrb : InstTCE<(outs), (ins MEMrr:$op1,I1Regs:$op2), "", [(truncstorei8 I1Regs:$op2, ADDRrr:$op1)]>;
def STQBib : InstTCE<(outs), (ins MEMri:$op1,I1Regs:$op2), "", [(truncstorei8 I1Regs:$op2, ADDRri:$op1)]>;
def : Pat<(store I1Regs:$src, ADDRrr:$addr), (STQBrb ADDRrr:$addr, I1Regs:$src)>;
def : Pat<(store I1Regs:$src, ADDRri:$addr), (STQBib ADDRri:$addr, I1Regs:$src)>;
def : Pat<(store (i1 imm:$src), ADDRrr:$addr), (STQBrb ADDRrr:$addr, imm:$src)>;
def : Pat<(store (i1 imm:$src), ADDRri:$addr), (STQBib ADDRri:$addr, imm:$src)>;

def LDQBr : InstTCE<(outs I1Regs:$op2), (ins MEMrr:$op1), "", [(set I1Regs:$op2, (sextloadi1 ADDRrr:$op1))]>;
def LDQBi : InstTCE<(outs I1Regs:$op2), (ins MEMri:$op1), "", [(set I1Regs:$op2, (sextloadi1 ADDRri:$op1))]>;
def LDQUBr : InstTCE<(outs I1Regs:$op2), (ins MEMrr:$op1), "", [(set I1Regs:$op2, (zextloadi1 ADDRrr:$op1))]>;
def LDQUBi : InstTCE<(outs I1Regs:$op2), (ins MEMri:$op1), "", [(set I1Regs:$op2, (zextloadi1 ADDRri:$op1))]>;
def : Pat<(i1 (load ADDRrr:$addr)), (LDQBr ADDRrr:$addr)>;
def : Pat<(i1 (load ADDRri:$addr)), (LDQBi ADDRri:$addr)>;

def : Pat<(i32 (zextloadi1 ADDRrr:$addr)), (ANDri (LDQr ADDRrr:$addr),1)>;
def : Pat<(i32 (zextloadi1 ADDRri:$addr)), (ANDri (LDQi ADDRri:$addr),1)>;

// the and could be dropped if we knew our store1 cannot write garbage to upper bits?
// so TODO: create true 1-bit mem operations
def : Pat<(i32 (sextloadi1 ADDRrr:$addr)), (SUBir 0,(ANDri (LDQr ADDRrr:$addr),1))>;
def : Pat<(i32 (sextloadi1 ADDRri:$addr)), (SUBir 0,(ANDri (LDQi ADDRri:$addr),1))>;

// float store and load in a way that machine code validator likes them
def STWFir : InstTCE<(outs), (ins MEMri:$op1, F32Regs:$op2), "", [(store F32Regs:$op2, ADDRri:$op1)]>;
def STWFrr : InstTCE<(outs), (ins MEMrr:$op1, F32Regs:$op2), "", [(store F32Regs:$op2, ADDRrr:$op1)]>;
def LDWFi : InstTCE<(outs F32Regs:$op2), (ins MEMri:$op1), "", [(set F32Regs:$op2, (load ADDRri:$op1))]>;
def LDWFr : InstTCE<(outs F32Regs:$op2), (ins MEMrr:$op1), "", [(set F32Regs:$op2, (load ADDRrr:$op1))]>;

// return address stack store/load
def STWRArr : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>;
def LDWRAr : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>;

// anyextloads -> sextloads
def : Pat<(i32 (extloadi1 ADDRrr:$src)), (LDQr ADDRrr:$src)>;
def : Pat<(i32 (extloadi1 ADDRri:$src)), (LDQi ADDRri:$src)>;
def : Pat<(i32 (extloadi8 ADDRrr:$src)), (LDQr ADDRrr:$src)>;
def : Pat<(i32 (extloadi8 ADDRri:$src)), (LDQi ADDRri:$src)>;
def : Pat<(i32 (extloadi16 ADDRrr:$src)), (LDHr ADDRrr:$src)>;
def : Pat<(i32 (extloadi16 ADDRri:$src)), (LDHi ADDRri:$src)>;

// 1-bit comparison with constant - xor or move.
// 1-bit one becomes -1 as i1 is signed, so the incoming imm is -1, not 1.
// do these comparisons both ways, reg first and imm first.
def : Pat<(i1 (setne I1Regs:$op1, (i1 -1))), (XORbicmp I1Regs:$op1, 1)>;
def : Pat<(i1 (setne (i1 -1), I1Regs:$op1)), (XORbicmp I1Regs:$op1, 1)>;
def : Pat<(i1 (seteq I1Regs:$op1, (i1 0))), (XORbicmp I1Regs:$op1, 1)>;
def : Pat<(i1 (seteq (i1 0), I1Regs:$op1)), (XORbicmp I1Regs:$op1, 1)>;
def : Pat<(i1 (setne I1Regs:$op1, (i1 0))), (MOVI1rr I1Regs:$op1)>;
def : Pat<(i1 (setne (i1 0), I1Regs:$op1)), (MOVI1rr I1Regs:$op1)>;
def : Pat<(i1 (seteq I1Regs:$op1, (i1 -1))), (MOVI1rr I1Regs:$op1)>;
def : Pat<(i1 (seteq (i1 -1), I1Regs:$op1)), (MOVI1rr I1Regs:$op1)>;


// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)
def : Pat<(i1 (setne I1Regs:$op1, I1Regs:$op2)), (XORbb I1Regs:$op1, I1Regs:$op2)>;
def : Pat<(i1 (seteq I1Regs:$op1, I1Regs:$op2)), (XORbj (XORbb I1Regs:$op1, I1Regs:$op2), 1)>;


//def : Pat<(i1 (setne (trunc I32Regs:$op1), I1Regs:$op2)), (XORrb I1Regs:$op1, I1Regs:$op2)>;
//def : Pat<(i1 (seteq (trunc I1Regs:$op1), I1Regs:$op2)), (XORbj (XORrb I32Regs:$op1, I1Regs:$op2), 1)>;