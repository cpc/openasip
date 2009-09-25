//
// TCE Baseopset instruction information.
//
// Author: Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
// Author: Mikael Lepistö 2009 (mikael.lepisto@tut.fi)
//


include "TCEInstrFormats.td"


// ----- Addressing modes. -----

// base + offset (e.g. label + offset).
def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
// frameindex + offset (simplifies some stack code generation).
def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex], []>;

def calltarget : Operand<i32>;

def MEMrr : Operand<i32> {
    let PrintMethod = "printMemOperand";
    let MIOperandInfo = (ops I32Regs, I32Regs);
}

def MEMri : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops I32Regs, i32imm);

}

// ----- Call pseudo instructions -----

// These are target-independent nodes, but have target-specific formats.
def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32> ]>;
def SDT_TCECallSeqEnd   : SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                         SDTCisVT<1, i32> ]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_TCECallSeqStart,
                           [SDNPHasChain, SDNPOutFlag]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_TCECallSeqEnd,
                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;

def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;
def call           : SDNode<"TCEISD::CALL", SDT_TCECall,
                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;



// ==========================================================
//  SELECT
//  Selects between two values based on true/false condition
//  and puts the selected value in result register.
//  Expanded by the scheduler into a branch sequence.
// ==========================================================

def SDT_TCERetFlag : SDTypeProfile<0, 0, []>;
def retflag : SDNode<"TCEISD::RET_FLAG", SDT_TCERetFlag,
	             [SDNPHasChain, SDNPOptInFlag]>;

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

//let usesCustomDAGSchedInserter = 0 in {    // Expanded by the scheduler.

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

//}

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

// def IMPLICIT_DEF_I1 : Pseudo<(outs I1Regs:$dst),
//                               (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set I1Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_I8 : Pseudo<(outs I8Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set I8Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_I16 : Pseudo<(outs I16Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set I16Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_I32 : Pseudo<(outs I32Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set I32Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_I64 : Pseudo<(outs I64Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set I64Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_F32  : Pseudo<(outs F32Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set F32Regs:$dst, (undef))]>;

// def IMPLICIT_DEF_F64  : Pseudo<(outs F64Regs:$dst), (ins),
//                               "# IMPLICIT_DEF $dst",
//                               [(set F64Regs:$dst, (undef))]>;


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

def MOVI1I32rr : InstTCE<(outs I32Regs:$dst), (ins I1Regs:$src),
                    "$src -> $dst;",
                    [(set I32Regs:$dst, (zext I1Regs:$src))]>;

// for some reason it cannot automatically use zext for anyext,
// so pattern here for it.
def : Pat <(i32 (anyext I1Regs:$src)),(MOVI1I32rr I1Regs:$src)>;

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
let isReturn = 1, isTerminator = 1, hasDelaySlot = 1 in {
    def RETL: InstTCE<(outs), (ins),
	"return-address -> jump.1; # RETURN", [(retflag)]>;
}

// def : Pat<(ret), (RETL)>;

// jump 
let isTerminator = 1, isBranch = 1 in {
    def TCEBRCOND : InstTCE<(outs), (ins I1Regs:$gr, i32imm:$dst),
        "? $gr $dst -> jump.1;", []>;

    def TCEBR : InstTCE<(outs),
                        (ins i32imm:$dst), "$dst -> jump.1;", []>;
}

// hardcoded boolean loads.
def LDQUb : InstTCE <(outs I1Regs:$dst), (ins MEMrr:$src), "", [(set I1Regs:$dst, (load ADDRrr:$src))]>;
def LDQUib : InstTCE<(outs I1Regs:$dst), (ins MEMri:$src), "", [(set I1Regs:$dst, (load ADDRri:$src))]>;


// and stores
def STQib : InstTCE<(outs), (ins MEMri:$addr, I1Regs:$data), "",[(store I1Regs:$data, ADDRri:$addr)]>;
def STQrb : InstTCE<(outs), (ins MEMrr:$addr, I1Regs:$data), "",[(store I1Regs:$data, ADDRrr:$addr)]>;


include "GenInstrInfo.td"

// anyextloads -> sextloads
def : Pat<(i32 (extloadi1 ADDRrr:$src)), (LDQr ADDRrr:$src)>;
def : Pat<(i32 (extloadi1 ADDRri:$src)), (LDQi ADDRri:$src)>;
def : Pat<(i32 (extloadi8 ADDRrr:$src)), (LDQr ADDRrr:$src)>;
def : Pat<(i32 (extloadi8 ADDRri:$src)), (LDQi ADDRri:$src)>;
def : Pat<(i32 (extloadi16 ADDRrr:$src)), (LDHr ADDRrr:$src)>;
def : Pat<(i32 (extloadi16 ADDRri:$src)), (LDHi ADDRri:$src)>;

//  old bool load/store - need to be changed - or not used anymore?
def : Pat<(i32 (zextloadi1 ADDRrr:$src)), (LDQr ADDRrr:$src)>;
def : Pat<(i32 (zextloadi1 ADDRri:$src)), (LDQi ADDRri:$src)>;

//def : Pat<(truncstorei1 I32Regs:$src, ADDRrr:$addr),
//          (STQrr ADDRrr:$addr, I32Regs:$src)>;

//def : Pat<(truncstorei1 I32Regs:$src, ADDRri:$addr),
//          (STQir ADDRri:$addr, I32Regs:$src)>;

def : Pat<(store F32Regs:$src, ADDRri:$addr),
          (STWir ADDRri:$addr, (MOVFI32rr F32Regs:$src))>;

def : Pat<(store F32Regs:$src, ADDRrr:$addr),
          (STWrr ADDRrr:$addr, (MOVFI32rr F32Regs:$src))>;

def : Pat<(f32 (load ADDRrr:$addr)),
          (MOVIF32rr (LDWr ADDRrr:$addr))>;

def : Pat<(f32 (load ADDRri:$addr)),
          (MOVIF32rr (LDWr ADDRri:$addr))>;

//def SELECTINT : Pat<(select I1Regs:$which, I32Regs:$src1, I32Regs:$src2),
//          (CMOV I1Regs:$which, (MOV GR:$src2), GR:$src1)>; // note order!
