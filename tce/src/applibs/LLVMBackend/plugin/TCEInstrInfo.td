//
// TCE Baseopset instruction information.
//
// Author: Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
// Author: Mikael Lepistö 2009 (mikael.lepisto@tut.fi)
// Author: Heikki Kultala 2012 (heikki.kultala@tut.fi)
//


include "TCEInstrFormats.td"
def LittleEndian : Predicate<"subtarget_.isLittleEndian()">;
def BigEndian : Predicate<"!subtarget_.isLittleEndian()">;

// Addressing modes.
def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex], []>;

// Address operands
def MEMrr : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops R32IRegs, R32IRegs);
}
def MEMri : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops R32IRegs, i32imm);
}

def TCESRAConst : SDNode<"TCEISD::SRA_Const", SDTIntBinOp, []>;
def TCESRLConst : SDNode<"TCEISD::SRL_Const", SDTIntBinOp, []>;
def TCESHLConst : SDNode<"TCEISD::SHL_Const", SDTIntBinOp, []>;

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

def TCEftoi  : SDNode<"TCEISD::FTOI", SDTTCEFTOI>;
def TCEitof  : SDNode<"TCEISD::ITOF", SDTTCEITOF>;

// These are target-independent nodes, but have target-specific formats.
def SDT_TCECallSeqEnd   : SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                        SDTCisVT<1, i32> ]>;

def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_TCECallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;

def call          : SDNode<"TCEISD::CALL", SDT_TCECall,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def retflag       : SDNode<"TCEISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def truncstorev2i8 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v2i8;
}]>;

def truncstorev2i16 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v2i16;
}]>;

def truncstorev4i8 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v4i8;
}]>;

def truncstorev4i16 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v4i16;
}]>;

def truncstorev8i8 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i8;
}]>;

def truncstorev8i16 : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
  return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i16;
}]>;

// =========================
//  Pseudo instructions
// =========================

let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            "# ADJCALLSTACKUP $amt1",
                            [(callseq_end timm:$amt1, timm:$amt2)]>;
}

// =========================
//  Moves
// =========================

// Immediate writes to register
def f16imm   : Operand<f16>;
let isReMaterializable = 1, isMoveImm = 1 in {

def MOVI1ri : InstTCE<(outs R1Regs:$dst), (ins i1imm:$val),
	           "$src -> $dst;",
                   [(set R1Regs:$dst, (i1 imm:$val))]>;

def PRED_TRUE_MOVI1ri : InstTCE<(outs R1Regs:$dst), 
    		   (ins R1Regs:$pred, i1imm:$val),
	           "$src -> $dst;",
                   []>;

def PRED_FALSE_MOVI1ri : InstTCE<(outs R1Regs:$dst), 
    		   (ins R1Regs:$pred, i1imm:$val),
	           "$src -> $dst;",
                   []>;

def MOVI32ri : InstTCE<(outs R32IRegs:$dst), (ins i32imm:$src),
	           "$src -> $dst;",
                   [(set R32IRegs:$dst, (i32 imm:$src))]>;

def PRED_TRUE_MOVI32ri : InstTCE<(outs R32IRegs:$dst), 
    		       (ins R1Regs:$pred, i32imm:$src),
	           	   "$src -> $dst;", []>;

def PRED_FALSE_MOVI32ri : InstTCE<(outs R32IRegs:$dst), 
    		       (ins R1Regs:$pred, i32imm:$src),
	           	   "$src -> $dst;", []>;

def MOVF32fk : InstTCE<(outs R32FPRegs:$dst), (ins f32imm:$val),
	           "$val -> $dst;",
                   [(set R32FPRegs:$dst, (f32 fpimm:$val))]>;
def MOVF16hk : InstTCE<(outs R32HFPRegs:$dst), (ins f16imm:$val),
	           "$src -> $dst;",
                   [(set R32HFPRegs:$dst, (f16 fpimm:$val))]>;
}

// TODO: are these really ever used for anything? propably not?
let isReMaterializable = 1, isMoveImm = 1 in {
def MOVF32fi : InstTCE<(outs R32FPRegs:$dst), (ins i32imm:$src),
	           "$src -> $dst;",
                   []>;
def MOVF16hi : InstTCE<(outs R32HFPRegs:$dst), (ins i32imm:$src),
	           "$src -> $dst;",
                   []>;
}

let isAsCheapAsAMove = 1 in {

// Int -> Int register moves.
def MOVI1rr : InstTCE<(outs R1Regs:$dst), (ins R1Regs:$src),
	           "$src -> $dst;",
                   []>;

def PRED_TRUE_MOVI1rr : InstTCE<(outs R1Regs:$dst), 
    (ins R1Regs:$pred, R1Regs:$src),
	           "$src -> $dst;",
                   []>;

def PRED_FALSE_MOVI1rr : InstTCE<(outs R1Regs:$dst), 
    (ins R1Regs:$pred, R1Regs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI32rr : InstTCE<(outs R32IRegs:$dst), (ins R32IRegs:$src),
	           "$src -> $dst;",
                   []>;

def PRED_TRUE_MOVI32rr : InstTCE<(outs R32IRegs:$dst), 
    (ins R1Regs:$pred, R32IRegs:$src),
	           "$src -> $dst;",
                   []>;

def PRED_FALSE_MOVI32rr : InstTCE<(outs R32IRegs:$dst), 
    (ins R1Regs:$pred, R32IRegs:$src),
	           "$src -> $dst;",
                   []>;

def MOVI1I32rr : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$src),
                     "$src -> $dst;",
                    []>;

def MOVI32I1rr : InstTCE<(outs R1Regs:$dst), (ins R32IRegs:$src),
                     "$src -> $dst;",
                    [(set R1Regs:$dst, (trunc R32IRegs:$src))]>;

def MOVFI32rf : InstTCE<(outs R32IRegs:$dst), (ins R32FPRegs:$src),
	           "$src -> $dst;",
                   [(set R32IRegs:$dst, (bitconvert R32FPRegs:$src))]>;

def MOVIF32fr : InstTCE<(outs R32FPRegs:$dst), (ins R32IRegs:$src),
	           "$src -> $dst;",
                   [(set R32FPRegs:$dst, (bitconvert R32IRegs:$src))]>;

def MOVF32ff : InstTCE<(outs R32FPRegs:$dst), (ins R32FPRegs:$src),
	           "$src -> $dst;",
                   []>;

def MOVF16hh : InstTCE<(outs R32HFPRegs:$dst), (ins R32HFPRegs:$src),
	           "$src -> $dst;",
                   []>;

def MOVFI16rh : InstTCE<(outs R32IRegs:$dst), (ins R32HFPRegs:$src),
	           "$src -> $dst;",
                   [(set R32IRegs:$dst, (bitconvert R32HFPRegs:$src))]>;

def MOVIF16hr : InstTCE<(outs R32HFPRegs:$dst), (ins R32IRegs:$src),
	           "$src -> $dst;",
                   [(set R32HFPRegs:$dst, (bitconvert R32IRegs:$src))]>;
}


def NOP : InstTCE<(outs), (ins), "...;", []>;

// zero extending moves used in some patterns

def ANDext : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$src, i32imm:$val),
	"", []>;

def PRED_TRUE_ANDext : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$pred, R1Regs:$src, i32imm:$val),
	"", []>;

def PRED_FALSE_ANDext : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$pred, R1Regs:$src, i32imm:$val),
	"", []>;

def XORbicmp: InstTCE<(outs R1Regs:$dst),(ins R1Regs:$src, i32imm:$val),
    "", []>;

def PRED_TRUE_XORbicmp: InstTCE<(outs R1Regs:$dst),(ins R1Regs:$pred, R1Regs:$src, i32imm:$val),
    "", []>;

def PRED_FALSE_XORbicmp: InstTCE<(outs R1Regs:$dst),(ins R1Regs:$pred, R1Regs:$src, i32imm:$val),
    "", []>;

// integer ops for floating point values - used for FP select emulation

def IORfff: InstTCE<(outs R32FPRegs:$dst), 
    (ins R32FPRegs:$src1, R32FPRegs:$src2), "", []>;

def IORhhh: InstTCE<(outs R32HFPRegs:$dst), 
    (ins R32HFPRegs:$src1, R32HFPRegs:$src2), "", []>;

def ANDfff: InstTCE<(outs R32FPRegs:$dst), 
    (ins R32FPRegs:$src1, R32FPRegs:$src2), "", []>;

def ANDhhh: InstTCE<(outs R32HFPRegs:$dst), 
    (ins R32HFPRegs:$src1, R32HFPRegs:$src2), "", []>;

def ADDfri: InstTCE<(outs R32FPRegs:$dst), 
    (ins R32IRegs:$src1, i32imm:$src2), "", []>;

def ADDhri: InstTCE<(outs R32HFPRegs:$dst), 
    (ins R32IRegs:$src1, i32imm:$src2), "", []>;

def SUBfir: InstTCE<(outs R32FPRegs:$dst), 
    (ins i32imm:$src1, R32IRegs:$src2), "", []>;

def SUBfri: InstTCE<(outs R32FPRegs:$dst), 
    (ins R32IRegs:$src1, i32imm:$src2), "", []>;

def SUBhir: InstTCE<(outs R32HFPRegs:$dst), 
    (ins i32imm:$src1, R32IRegs:$src2), "", []>;

def SUBhri: InstTCE<(outs R32HFPRegs:$dst), 
    (ins R32IRegs:$src1, i32imm:$src2), "", []>;

// (set R32IRegs:$dst, (zext R1Regs:$src))]>;

def: Pat <(i32 (anyext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;
def: Pat <(i32 (zext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;

include "GenInstrInfo.td"

def: Pat <(i32 (shl R32IRegs:$val, (i32 1))), (ADDrrr R32Regs:$val, R32Regs:$val)>;
def: Pat <(i32 (TCESHLConst R32IRegs:$val, (i32 1))), (ADDrrr R32IRegs:$val, R32IRegs:$val)>;


// ----- Global addresses, constant pool entries ------
def TCEGlobalAddr : SDNode<"TCEISD::GLOBAL_ADDR", SDTIntUnaryOp>;
def TCEConstPool : SDNode<"TCEISD::CONST_POOL", SDTIntUnaryOp>;
def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;
def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI32ri tconstpool:$in)>;
def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;
def : Pat<(TCEConstPool tconstpool:$in), (MOVI32ri tconstpool:$in)>;

// Transfer a basic block address into a register
def TCEBlockAddress : SDNode<"TCEISD::BLOCK_ADDR", SDTIntUnaryOp>;
def : Pat<(TCEBlockAddress tblockaddress:$src1),
          (MOVI32ri tblockaddress:$src1)>;

// ============================
//  Control flow
// ============================

// Return.
let isReturn = 1, isTerminator = 1, 
    hasDelaySlot = 1, isBarrier = 1 in {
    def RETL: InstTCE<(outs), (ins),
   	"return-address -> jump.1; # RETURN", [(retflag)]>;
}

// jump 
let isTerminator = 1, isBranch = 1 in {
    def TCEBRCOND : InstTCE<(outs), (ins R1Regs:$gr, i32imm:$dst),
        "? $gr $dst -> jump.1;", []>;

    def TCEBRICOND : InstTCE<(outs), (ins R1Regs:$gr, i32imm:$dst),
        "! $gr $dst -> jump.1;", []>;

    def TCEBR : InstTCE<(outs), (ins i32imm:$dst), "$dst -> jump.1;", []>;

    let isIndirectBranch = 1 in {
        def TCEBRIND : InstTCE<(outs), (ins R32Regs:$dst), "$dst -> jump.1;", []>;
   }     

}


// indirect branch
def: Pat<(brind (i32 R32Regs:$dst)),
         (TCEBRIND R32Regs:$dst)>;

// FP stores/loads

    def LDWfr : InstTCE<(outs R32FPRegs:$op2),
        (ins MEMrr:$op1), "", [(set R32FPRegs:$op2,
        (load ADDRrr:$op1))]>, Requires<[BigEndian]>;

    def LDWfi : InstTCE<(outs R32FPRegs:$op2),
        (ins MEMri:$op1), "", [(set R32FPRegs:$op2,
        (load ADDRri:$op1))]>, Requires<[BigEndian]>;

    def STWfr : InstTCE<(outs), (ins MEMrr:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRrr:$op1)]>, Requires<[BigEndian]>;
    def STWfi : InstTCE<(outs), (ins MEMri:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRri:$op1)]>, Requires<[BigEndian]>;

    def STHhr : InstTCE<(outs), (ins MEMrr:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRrr:$op1)]>, Requires<[BigEndian]>;

    def STHhi : InstTCE<(outs), (ins MEMri:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRri:$op1)]>, Requires<[BigEndian]>;

    def LD32fr : InstTCE<(outs R32FPRegs:$op2),
        (ins MEMrr:$op1), "", [(set R32FPRegs:$op2,
        (load ADDRrr:$op1))]>, Requires<[LittleEndian]>;

    def LD32fi : InstTCE<(outs R32FPRegs:$op2),
        (ins MEMri:$op1), "", [(set R32FPRegs:$op2,
        (load ADDRri:$op1))]>, Requires<[LittleEndian]>;

    def ST32fr : InstTCE<(outs), (ins MEMrr:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;
    def ST32fi : InstTCE<(outs), (ins MEMri:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRri:$op1)]>, Requires<[LittleEndian]>;


    def ST16hr : InstTCE<(outs), (ins MEMrr:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;

    def ST16hi : InstTCE<(outs), (ins MEMri:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRri:$op1)]>, Requires<[LittleEndian]>;




// ==========================================================
//  SELECT
//  Selects between two values based on true/false condition
//  and puts the selected value in result register.
//  Expanded by the scheduler into a branch sequence.
// ==========================================================

// some peephole patterns.
// 1-bit select with imm values - xor or mov.
def : Pat<(i1 (select R1Regs:$c, (i1 0), (i1 -1))), (XORbbj R1Regs:$c, 1)>;
def : Pat<(i1 (select R1Regs:$c, (i1 -1), (i1 0))), (MOVI1rr R1Regs:$c)>;

def : Pat<(i1 (select R1Regs:$c, (i1 -1), R1Regs:$F)), (IORbbb R1Regs:$c, R1Regs:$F)>;
def : Pat<(i1 (select R1Regs:$c, R1Regs:$T, (i1 0))), (ANDbbb R1Regs:$c, R1Regs:$T)>;

// select of 1 or 0.
def : Pat<(i32 (select R1Regs:$c, (i32 1), (i32 0))), (ANDext R1Regs:$c, 1)>;



// sign extension from i1 means 0 -> 0 , 1(true) -> -1
def: Pat <(i32 (sext R1Regs:$src)), (SUBrir 0, (ANDext R1Regs:$src,1))>;

// boolean store - big endian
def STQBrb : InstTCE<(outs), (ins MEMrr:$op1,R1Regs:$op2), "",
    [(truncstorei8 R1Regs:$op2, ADDRrr:$op1)]>, Requires<[BigEndian]>;

def PRED_TRUE_STQBrb : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,R1Regs:$op2), "", []>, Requires<[BigEndian]>;
def PRED_FALSE_STQBrb : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,R1Regs:$op2), "", []>, Requires<[BigEndian]>;
def STQBib : InstTCE<(outs), (ins MEMri:$op1,R1Regs:$op2), "", [(truncstorei8 R1Regs:$op2, ADDRri:$op1)]>, Requires<[BigEndian]>;
def PRED_TRUE_STQBib : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,R1Regs:$op2), "", []>, Requires<[BigEndian]>;
def PRED_FALSE_STQBib : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,R1Regs:$op2), "", []>, Requires<[BigEndian]>;

def STQBrj : InstTCE<(outs), (ins MEMrr:$op1,i1imm:$op2), "", [(truncstorei8 (i1 imm:$op2), ADDRrr:$op1)]>, Requires<[BigEndian]>;
def PRED_TRUE_STQBrj : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,i1imm:$op2), "", []>, Requires<[BigEndian]>;
def PRED_FALSE_STQBrj : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,i1imm:$op2), "", []>, Requires<[BigEndian]>;
def STQBij : InstTCE<(outs), (ins MEMri:$op1,i1imm:$op2), "", [(truncstorei8 (i1 imm:$op2), ADDRri:$op1)]>, Requires<[BigEndian]>;
def PRED_TRUE_STQBij : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,i1imm:$op2), "", []>, Requires<[BigEndian]>;
def PRED_FALSE_STQBij : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,i1imm:$op2), "", []>, Requires<[BigEndian]>;

def : Pat<(store R1Regs:$src, ADDRrr:$addr), (STQBrb ADDRrr:$addr, R1Regs:$src)>, Requires<[BigEndian]>;
def : Pat<(store R1Regs:$src, ADDRri:$addr), (STQBib ADDRri:$addr, R1Regs:$src)>, Requires<[BigEndian]>;

// boolean store - litte-endain
def ST8Brb : InstTCE<(outs), (ins MEMrr:$op1,R1Regs:$op2), "",
    [(truncstorei8 R1Regs:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;

def PRED_TRUE_ST8Brb : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,R1Regs:$op2), "", []>, Requires<[LittleEndian]>;
def PRED_FALSE_ST8Brb : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,R1Regs:$op2), "", []>, Requires<[LittleEndian]>;
def ST8Bib : InstTCE<(outs), (ins MEMri:$op1,R1Regs:$op2), "", [(truncstorei8 R1Regs:$op2, ADDRri:$op1)]>, Requires<[LittleEndian]>;
def PRED_TRUE_ST8Bib : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,R1Regs:$op2), "", []>, Requires<[LittleEndian]>;
def PRED_FALSE_ST8Bib : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,R1Regs:$op2), "", []>, Requires<[LittleEndian]>;

def ST8Brj : InstTCE<(outs), (ins MEMrr:$op1,i1imm:$op2), "", [(truncstorei8 (i1 imm:$op2), ADDRrr:$op1)]>, Requires<[LittleEndian]>;
def PRED_TRUE_ST8Brj : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,i1imm:$op2), "", []>, Requires<[LittleEndian]>;
def PRED_FALSE_ST8Brj : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$op1,i1imm:$op2), "", []>, Requires<[LittleEndian]>;
def ST8Bij : InstTCE<(outs), (ins MEMri:$op1,i1imm:$op2), "", [(truncstorei8 (i1 imm:$op2), ADDRri:$op1)]>, Requires<[LittleEndian]>;
def PRED_TRUE_ST8Bij : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,i1imm:$op2), "", []>, Requires<[LittleEndian]>;
def PRED_FALSE_ST8Bij : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$op1,i1imm:$op2), "", []>, Requires<[LittleEndian]>;

def : Pat<(store R1Regs:$src, ADDRrr:$addr), (ST8Brb ADDRrr:$addr, R1Regs:$src)>, Requires<[LittleEndian]>;
def : Pat<(store R1Regs:$src, ADDRri:$addr), (ST8Bib ADDRri:$addr, R1Regs:$src)>, Requires<[LittleEndian]>;

def: Pat <(i32 (sext R1Regs:$src)), (SUBrir 0,(ANDext R1Regs:$src, 1))>;

// TODO: why did trunk have this? more overhead.
//def: Pat<(i32 (sext R1Regs:$src)), 
//         (SUBrrr (MOVI32ri 0),(ANDext R1Regs:$src, 1))>;

// return address stack store/load
def STWRArr : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>, Requires<[BigEndian]>;
def LDWRAr : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>, Requires<[BigEndian]>;

def ST32RArr : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;
def LD32RAr : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>, Requires<[LittleEndian]>;



// 1-bit comparison with constant - xor or move.
// 1-bit one becomes -1 as i1 is signed, so the incoming imm is -1, not 1.
// do these comparisons both ways, reg first and imm first.
def : Pat<(i1 (setne R1Regs:$op1, (i1 -1))), (XORbicmp R1Regs:$op1, 1)>;
def : Pat<(i1 (setne (i1 -1), R1Regs:$op1)), (XORbicmp R1Regs:$op1, 1)>;
def : Pat<(i1 (seteq R1Regs:$op1, (i1 0))), (XORbicmp R1Regs:$op1, 1)>;
def : Pat<(i1 (seteq (i1 0), R1Regs:$op1)), (XORbicmp R1Regs:$op1, 1)>;
def : Pat<(i1 (setne R1Regs:$op1, (i1 0))), (MOVI1rr R1Regs:$op1)>;
def : Pat<(i1 (setne (i1 0), R1Regs:$op1)), (MOVI1rr R1Regs:$op1)>;
def : Pat<(i1 (seteq R1Regs:$op1, (i1 -1))), (MOVI1rr R1Regs:$op1)>;
def : Pat<(i1 (seteq (i1 -1), R1Regs:$op1)), (MOVI1rr R1Regs:$op1)>;


// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)
def : Pat<(i1 (setne R1Regs:$op1, R1Regs:$op2)), (XORbbb R1Regs:$op1, R1Regs:$op2)>;
// TODO: should the temp values be converted to i32? usually more i32 regs.
def : Pat<(i1 (seteq R1Regs:$op1, R1Regs:$op2)), (XORbbj (XORbbb R1Regs:$op1, R1Regs:$op2), 1)>;


//def : Pat<(i1 (setne (trunc R32IRegs:$op1), R1Regs:$op2)), (XORrb R1Regs:$op1, R1Regs:$op2)>;
//def : Pat<(i1 (seteq (trunc R1Regs:$op1), R1Regs:$op2)), (XORbj (XORrb R32IRegs:$op1, R1Regs:$op2), 1)>;

// insert handled with insert_subreg
def : Pat <(v2i32 (vector_insert (v2i32 V2R32IRegs:$vec), (i32 R32IRegs:$e), (i32 0))),
    (v2i32 (INSERT_SUBREG (v2i32 V2R32IRegs:$vec), (i32 R32IRegs:$e), lane0))>;
def : Pat <(v2i32 (vector_insert (v2i32 V2R32IRegs:$vec), (i32 R32IRegs:$e), (i32 1))),
    (v2i32 (INSERT_SUBREG (v2i32 V2R32IRegs:$vec), (i32 R32IRegs:$e), lane1))>;
def : Pat <(v2f32 (vector_insert (v2f32 V2R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 0))),
    (v2f32 (INSERT_SUBREG (v2f32 V2R32FPRegs:$vec), (f32 R32FPRegs:$e), lane0))>;
def : Pat <(v2f32 (vector_insert (v2f32 V2R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 1))),
    (v2f32 (INSERT_SUBREG (v2f32 V2R32FPRegs:$vec), (f32 R32FPRegs:$e), lane1))>;

def : Pat <(v4i32 (vector_insert (v4i32 V4R32IRegs:$vec), (i32 R32IRegs:$e), (i32 0))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), 
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$vec), subvector2_0)), 
    (i32 R32IRegs:$e), lane0)), subvector2_0))>;

def : Pat <(v4i32 (vector_insert (v4i32 V4R32IRegs:$vec), (i32 R32IRegs:$e), (i32 1))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), 
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$vec), subvector2_0)), 
    (i32 R32IRegs:$e), lane1)), subvector2_0))>;

def : Pat <(v4i32 (vector_insert (v4i32 V4R32IRegs:$vec), (i32 R32IRegs:$e), (i32 2))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), 
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$vec), subvector2_2)), 
    (i32 R32IRegs:$e), lane2)), subvector2_2))>;

def : Pat <(v4i32 (vector_insert (v4i32 V4R32IRegs:$vec), (i32 R32IRegs:$e), (i32 3))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), 
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$vec), subvector2_2)), 
    (i32 R32IRegs:$e), lane3)), subvector2_2))>;

def : Pat <(v4f32 (vector_insert (v4f32 V4R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 0))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec), 
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$vec), subvector2_0)), 
    (f32 R32FPRegs:$e), lane0)), subvector2_0))>;

def : Pat <(v4f32 (vector_insert (v4f32 V4R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 1))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec), 
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$vec), subvector2_0)), 
    (f32 R32FPRegs:$e), lane1)), subvector2_0))>;

def : Pat <(v4f32 (vector_insert (v4f32 V4R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 2))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec),
    (v2f32 (INSERT_SUBREG
    (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$vec), subvector2_2)),
    (f32 R32FPRegs:$e), lane2)), subvector2_2))>;

def : Pat <(v4f32 (vector_insert (v4f32 V4R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 3))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec),
    (v2f32 (INSERT_SUBREG
    (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$vec), subvector2_2)),
    (f32 R32FPRegs:$e), lane3)), subvector2_2))>;


// 8-way inserts

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 0))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)), subvector2_0)), 
    (i32 R32IRegs:$e), lane0)), subvector2_0)), subvector4_0))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 1))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)), subvector2_0)), 
    (i32 R32IRegs:$e), lane1)), subvector2_0)), subvector4_0))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 2))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)), subvector2_2)),
    (i32 R32IRegs:$e), lane2)), subvector2_2)), subvector4_0))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 3))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)), subvector2_2)),
    (i32 R32IRegs:$e), lane3)), subvector2_2)), subvector4_0))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 4))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)), subvector2_4)),
    (i32 R32IRegs:$e), lane4)), subvector2_4)), subvector4_4))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 5))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)), subvector2_4)),
    (i32 R32IRegs:$e), lane5)), subvector2_4)), subvector4_4))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 6))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)), subvector2_6)),
    (i32 R32IRegs:$e), lane6)), subvector2_6)), subvector4_4))>;

def : Pat <(v8i32 (vector_insert (v8i32 V8R32IRegs:$vec), (i32 R32IRegs:$e), (i32 7))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 (INSERT_SUBREG 
    (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)), subvector2_6)),
    (i32 R32IRegs:$e), lane7)), subvector2_6)), subvector4_4))>;

// 8-way inserts fo FP
def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 0))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)), subvector2_0)), 
    (f32 R32FPRegs:$e), lane0)), subvector2_0)), subvector4_0))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 1))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)), subvector2_0)), 
    (f32 R32FPRegs:$e), lane1)), subvector2_0)), subvector4_0))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 2))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)), subvector2_2)),
    (f32 R32FPRegs:$e), lane2)), subvector2_2)), subvector4_0))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 3))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)), subvector2_2)),
    (f32 R32FPRegs:$e), lane3)), subvector2_2)), subvector4_0))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 4))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)), subvector2_4)),
    (f32 R32FPRegs:$e), lane4)), subvector2_4)), subvector4_4))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 5))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)), subvector2_4)),
    (f32 R32FPRegs:$e), lane5)), subvector2_4)), subvector4_4))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 6))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)), subvector2_6)),
    (f32 R32FPRegs:$e), lane6)), subvector2_6)), subvector4_4))>;

def : Pat <(v8f32 (vector_insert (v8f32 V8R32FPRegs:$vec), (f32 R32FPRegs:$e), (i32 7))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 (INSERT_SUBREG 
    (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)), subvector2_6)),
    (f32 R32FPRegs:$e), lane7)), subvector2_6)), subvector4_4))>;

// subvector inserts, 2->4
def : Pat<(v4i32 (insert_subvector (v4i32 V4R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 0))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), subvector2_0))>;

def : Pat<(v4i32 (insert_subvector (v4i32 V4R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 2))),
    (v4i32 (INSERT_SUBREG (v4i32 V4R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), subvector2_2))>;

def : Pat<(v4f32 (insert_subvector (v4f32 V4R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 0))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), subvector2_0))>;

def : Pat<(v4f32 (insert_subvector (v4f32 V4R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 2))),
    (v4f32 (INSERT_SUBREG (v4f32 V4R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), subvector2_2))>;

// subvector inserts, 4->8
def : Pat<(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v4i32 V4R32IRegs:$sv), (i32 0))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec), (v4i32 V4R32IRegs:$sv), subvector4_0))>;

def : Pat<(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v4i32 V4R32IRegs:$sv), (i32 4))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec), (v4i32 V4R32IRegs:$sv), subvector4_4))>;

def : Pat<(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v4f32 V4R32FPRegs:$sv), (i32 0))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec), (v4f32 V4R32FPRegs:$sv), subvector4_0))>;

def : Pat<(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v4f32 V4R32FPRegs:$sv), (i32 2))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec), (v4f32 V4R32FPRegs:$sv), subvector4_4))>;

// subvector inserts 2->8 for integers
def : Pat <(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 0))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 V2R32IRegs:$sv), subvector2_0)), subvector4_0))>;

def : Pat <(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 2))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_0)),
    (v2i32 V2R32IRegs:$sv), subvector2_2)), subvector4_0))>;

def : Pat <(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 4))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 V2R32IRegs:$sv), subvector2_4)), subvector4_4))>;

def : Pat <(v8i32 (insert_subvector (v8i32 V8R32IRegs:$vec), (v2i32 V2R32IRegs:$sv), (i32 6))),
    (v8i32 (INSERT_SUBREG (v8i32 V8R32IRegs:$vec),
    (v4i32 (INSERT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$vec), subvector4_4)),
    (v2i32 V2R32IRegs:$sv), subvector2_6)), subvector4_4))>;

// subvector inserts 2->8 for fp
def : Pat <(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 0))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 V2R32FPRegs:$sv), subvector2_0)), subvector4_0))>;

def : Pat <(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 2))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_0)),
    (v2f32 V2R32FPRegs:$sv), subvector2_2)), subvector4_0))>;

def : Pat <(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 4))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 V2R32FPRegs:$sv), subvector2_4)), subvector4_4))>;

def : Pat <(v8f32 (insert_subvector (v8f32 V8R32FPRegs:$vec), (v2f32 V2R32FPRegs:$sv), (i32 6))),
    (v8f32 (INSERT_SUBREG (v8f32 V8R32FPRegs:$vec),
    (v4f32 (INSERT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$vec), subvector4_4)),
    (v2f32 V2R32FPRegs:$sv), subvector2_6)), subvector4_4))>;


// scalar_to_vector handled with insert_subreg
def : Pat <(v2i32 (scalar_to_vector (i32 R32IRegs:$e))),
    (v2i32 (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)), (i32 R32IRegs:$e), lane0))>;

def : Pat <(v2f32 (scalar_to_vector (f32 R32FPRegs:$e))),
    (v2f32 (INSERT_SUBREG (v2f32 (IMPLICIT_DEF)), (f32 R32FPRegs:$e), lane0))>;

def : Pat <(v4i32 (scalar_to_vector (i32 R32IRegs:$e))),
    (v4i32 (INSERT_SUBREG (v4i32 (IMPLICIT_DEF)), 
        (v2i32 (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)), (i32 R32IRegs:$e), lane0)),
     subvector2_0))>;

def : Pat <(v4f32 (scalar_to_vector (f32 R32FPRegs:$e))),
    (v4f32 (INSERT_SUBREG (v4f32 (IMPLICIT_DEF)),
        (v2f32 (INSERT_SUBREG (v2f32 (IMPLICIT_DEF)), (f32 R32FPRegs:$e), lane0)),
    subvector2_0))>;

def : Pat <(v8i32 (scalar_to_vector (i32 R32IRegs:$e))),
    (v8i32 (INSERT_SUBREG (v8i32 (IMPLICIT_DEF)),
        (v4i32 (INSERT_SUBREG (v4i32 (IMPLICIT_DEF)),
            (v2i32 (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)), (i32 R32IRegs:$e), lane0)),
        subvector2_0)),
    subvector4_0))>;
    
def : Pat <(v8f32 (scalar_to_vector (f32 R32FPRegs:$e))),
    (v8f32 (INSERT_SUBREG (v8f32 (IMPLICIT_DEF)),
        (v4f32 (INSERT_SUBREG (v4f32 (IMPLICIT_DEF)),
            (v2f32 (INSERT_SUBREG (v2f32 (IMPLICIT_DEF)), (f32 R32FPRegs:$e), lane0)),
        subvector2_0)),
    subvector4_0))>;

// build done with inseet_subreg
def : Pat <(v2i32 (build_vector (i32 R32IRegs:$e1), R32IRegs:$e2)),
    (v2i32 (INSERT_SUBREG
        (v2i32 (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)),
            (i32 R32IRegs:$e1), lane0)),
        (i32 R32IRegs:$e2), lane1))>;


def : Pat <(v2f32 (build_vector (f32 R32FPRegs:$e1), R32FPRegs:$e2)),
    (v2f32 (INSERT_SUBREG
        (v2f32 (INSERT_SUBREG (v2f32 (IMPLICIT_DEF)),
            (f32 R32FPRegs:$e1), lane0)),
        (f32 R32FPRegs:$e2), lane1))>;

// build done with insert_subreg
def : Pat <(v4i32 (build_vector (i32 R32IRegs:$e1), (i32 R32IRegs:$e2), (i32 R32IRegs:$e3), (i32 R32IRegs:$e4))),
    (v4i32 (INSERT_SUBREG
        (v4i32 (INSERT_SUBREG
            (v4i32 (INSERT_SUBREG
                (v4i32 (INSERT_SUBREG (v4i32 (IMPLICIT_DEF)),
                    (i32 R32IRegs:$e1), lane0)),
                (i32 R32IRegs:$e2), lane1)),
            (i32 R32IRegs:$e3), lane2)),
        (i32 R32IRegs:$e4), lane3))>;

// build done with inseet_subreg
def : Pat <(v4f32 (build_vector (f32 R32FPRegs:$e1), (f32 R32FPRegs:$e2), (f32 R32FPRegs:$e3), (f32 R32FPRegs:$e4))),
    (v4f32 (INSERT_SUBREG
        (v4f32 (INSERT_SUBREG
            (v4f32 (INSERT_SUBREG
                (v4f32 (INSERT_SUBREG (v4f32 (IMPLICIT_DEF)),
                    (f32 R32FPRegs:$e1), lane0)),
                (f32 R32FPRegs:$e2), lane1)),
            (f32 R32FPRegs:$e3), lane2)),
        (f32 R32FPRegs:$e4), lane3))>;

// build done with insert_subreg
def : Pat <(v8i32 (build_vector (i32 R32IRegs:$e1), (i32 R32IRegs:$e2), (i32 R32IRegs:$e3), (i32 R32IRegs:$e4), (i32 R32IRegs:$e5), (i32 R32IRegs:$e6), (i32 R32IRegs:$e7), (i32 R32IRegs:$e8))),
    (v8i32 (INSERT_SUBREG
        (v8i32 (INSERT_SUBREG
            (v8i32 (INSERT_SUBREG
                (v8i32 (INSERT_SUBREG
                    (v8i32 (INSERT_SUBREG
                        (v8i32 (INSERT_SUBREG
                            (v8i32 (INSERT_SUBREG
                                (v8i32 (INSERT_SUBREG (v8i32 (IMPLICIT_DEF)),
                                    (i32 R32IRegs:$e1), lane0)),
                                (i32 R32IRegs:$e2), lane1)),
                            (i32 R32IRegs:$e3), lane2)),
                        (i32 R32IRegs:$e4), lane3)),
                    (i32 R32IRegs:$e5), lane4)),
                (i32 R32IRegs:$e6), lane5)),
            (i32 R32IRegs:$e7), lane6)),
        (i32 R32IRegs:$e8), lane7))>;

// build done with insert_subreg
def : Pat <(v8f32 (build_vector (f32 R32FPRegs:$e1), (f32 R32FPRegs:$e2), (f32 R32FPRegs:$e3), (f32 R32FPRegs:$e4), (f32 R32FPRegs:$e5), (f32 R32FPRegs:$e6), (f32 R32FPRegs:$e7), (f32 R32FPRegs:$e8))),
    (v8f32 (INSERT_SUBREG
        (v8f32 (INSERT_SUBREG
            (v8f32 (INSERT_SUBREG
                (v8f32 (INSERT_SUBREG
                    (v8f32 (INSERT_SUBREG
                        (v8f32 (INSERT_SUBREG
                            (v8f32 (INSERT_SUBREG
                                (v8f32 (INSERT_SUBREG (v8f32 (IMPLICIT_DEF)),
                                    (f32 R32FPRegs:$e1), lane0)),
                                (f32 R32FPRegs:$e2), lane1)),
                            (f32 R32FPRegs:$e3), lane2)),
                        (f32 R32FPRegs:$e4), lane3)),
                    (f32 R32FPRegs:$e5), lane4)),
                (f32 R32FPRegs:$e6), lane5)),
            (f32 R32FPRegs:$e7), lane6)),
        (f32 R32FPRegs:$e8), lane7))>;


// do concat vectors also through subvectors
def concat_vectors2 : SDNode<"ISD::CONCAT_VECTORS", SDTypeProfile<1,2,[]>,[]>;
def concat_vectors4 : SDNode<"ISD::CONCAT_VECTORS", SDTypeProfile<1,4,[]>,[]>;


def : Pat<(v8i32 (concat_vectors4 (v2i32 V2R32IRegs:$v1), (v2i32 V2R32IRegs:$v2),
(v2i32 V2R32IRegs:$v3), (v2i32 V2R32IRegs:$v4))),
    (v8i32 (INSERT_SUBREG
        (v8i32 (INSERT_SUBREG 
            (v8i32 (INSERT_SUBREG
                (v8i32 (INSERT_SUBREG (v8i32 (IMPLICIT_DEF)),
            (v2i32 V2R32Regs:$v1), subvector2_0)),
        (v2i32 V2R32Regs:$v2), subvector2_2)),
    (v2i32 V2R32Regs:$v3), subvector2_4)),
(v2i32 V2R32Regs:$v4), subvector2_6))>;
    
def : Pat<(v8f32 (concat_vectors4 (v2f32 V2R32FPRegs:$v1), 
    (v2f32 V2R32FPRegs:$v2), (v2f32 V2R32FPRegs:$v3), (v2f32 V2R32FPRegs:$v4))),
    (v8f32 (INSERT_SUBREG
        (v8f32 (INSERT_SUBREG 
            (v8f32 (INSERT_SUBREG
                (v8f32 (INSERT_SUBREG (v8f32 (IMPLICIT_DEF)),
            (v2f32 V2R32FPRegs:$v1), subvector2_0)),
        (v2f32 V2R32FPRegs:$v2), subvector2_2)),
    (v2f32 V2R32FPRegs:$v3), subvector2_4)),
(v2f32 V2R32FPRegs:$v4), subvector2_6)) >;


def : Pat<(v4i32 (concat_vectors2 (v2i32 V2R32IRegs:$v1), (v2i32 V2R32IRegs:$v2))),
    (v4i32 (INSERT_SUBREG
        (v4i32 (INSERT_SUBREG (v4i32 (IMPLICIT_DEF)),
            (v2i32 V2R32IRegs:$v1), subvector2_0)),
        (v2i32 V2R32IRegs:$v2), subvector2_2))>;

def : Pat<(v4f32 (concat_vectors2 (v2f32 V2R32FPRegs:$v1), (v2f32 V2R32FPRegs:$v2))),
    (v4f32 (INSERT_SUBREG
        (v4f32 (INSERT_SUBREG (v4f32 (IMPLICIT_DEF)),
            (v2f32 V2R32FPRegs:$v1), subvector2_0)),
        (v2f32 V2R32FPRegs:$v2), subvector2_2))>;

def : Pat<(v8i32 (concat_vectors2 (v4i32 V4R32IRegs:$v1), (v4i32 V4R32IRegs:$v2))),
    (v8i32 (INSERT_SUBREG
        (v8i32 (INSERT_SUBREG (v8i32 (IMPLICIT_DEF)),
            (v4i32 V4R32IRegs:$v1), subvector4_0)),
        (v4i32 V4R32IRegs:$v2), subvector4_4))>;

def : Pat<(v8f32 (concat_vectors2 (v4f32 V4R32FPRegs:$v1), (v4f32 V4R32FPRegs:$v2))),
    (v8f32 (INSERT_SUBREG
        (v8f32 (INSERT_SUBREG (v8f32 (IMPLICIT_DEF)),
            (v4f32 V4R32FPRegs:$v1), subvector4_0)),
        (v4f32 V4R32FPRegs:$v2), subvector4_4))>;



// vector extract is done by just using the scalar subregister directly.
def : Pat<(i32 (vector_extract (v2i32 V2R32IRegs:$op1), 0)),
        (i32 (EXTRACT_SUBREG (v2i32 V2R32IRegs:$op1), lane0))>;
def : Pat<(i32 (vector_extract (v2i32 V2R32IRegs:$op1), (i32 1))),
        (i32 (EXTRACT_SUBREG (v2i32 V2R32IRegs:$op1), lane1))>;
def : Pat<(f32 (vector_extract (v2f32 V2R32FPRegs:$op1), (i32 0))),
        (f32 (EXTRACT_SUBREG (v2f32 V2R32FPRegs:$op1), lane0))>;
def : Pat<(f32 (vector_extract (v2f32 V2R32FPRegs:$op1), (i32 1))),
        (f32 (EXTRACT_SUBREG (v2f32 V2R32FPRegs:$op1), lane1))>;


// subregisters of subvectors used for extract
def : Pat<(i32 (vector_extract (v4i32 V4R32IRegs:$op1), (i32 0))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_0)), lane0))>;
def : Pat<(i32 (vector_extract (v4i32 V4R32IRegs:$op1), (i32 1))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_0)), lane1))>;
def : Pat<(i32 (vector_extract (v4i32 V4R32IRegs:$op1), (i32 2))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_2)), lane2))>;
def : Pat<(i32 (vector_extract (v4i32 V4R32IRegs:$op1), (i32 3))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_2)), lane3))>;

// subregisters of subvectors used for extract
def : Pat<(f32 (vector_extract (v4f32 V4R32FPRegs:$op1), (i32 0))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_0)), lane0))>;
def : Pat<(f32 (vector_extract (v4f32 V4R32FPRegs:$op1), (i32 1))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_0)), lane1))>;
def : Pat<(f32 (vector_extract (v4f32 V4R32FPRegs:$op1), (i32 2))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_2)), lane2))>;
def : Pat<(f32 (vector_extract (v4f32 V4R32FPRegs:$op1), (i32 3))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_2)), lane3))>;


// subregisters of subvectors of subvectors of subvectused for extract
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 0))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_0)), lane0))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 1))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_0)), lane1))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 2))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_2)), lane2))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 3))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_2)), lane3))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 4))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_4)), lane4))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 5))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_4)), lane5))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 6))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_6)), lane6))>;
def : Pat<(i32 (vector_extract (v8i32 V8R32IRegs:$op1), (i32 7))),
        (i32 (EXTRACT_SUBREG (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_6)), lane7))>;

def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 0))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_0)), lane0))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 1))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_0)), lane1))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 2))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_2)), lane2))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 3))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_2)), lane3))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 4))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_4)), lane4))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 5))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_4)), lane5))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 6))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_6)), lane6))>;
def : Pat<(f32 (vector_extract (v8f32 V8R32FPRegs:$op1), (i32 7))),
        (f32 (EXTRACT_SUBREG (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_6)), lane7))>;

// subvector extracts. not yet used for anything.
def : Pat<(v2i32 (extract_subvector (v4i32 V4R32IRegs:$op1), (i32 0))),
        (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_0))>;
def : Pat<(v2i32 (extract_subvector (v4i32 V4R32IRegs:$op1), (i32 2))),
        (v2i32 (EXTRACT_SUBREG (v4i32 V4R32IRegs:$op1), subvector2_2))>;
def : Pat<(v2f32 (extract_subvector (v4f32 V4R32FPRegs:$op1), (i32 0))),
        (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_0))>;
def : Pat<(v2f32 (extract_subvector (v4f32 V4R32FPRegs:$op1), (i32 2))),
        (v2f32 (EXTRACT_SUBREG (v4f32 V4R32FPRegs:$op1), subvector2_2))>;

def : Pat<(v4i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 0))),
        (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0))>;
def : Pat<(v4i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 4))),
        (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4))>;
def : Pat<(v4f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 0))),
        (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0))>;
def : Pat<(v4f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 4))),
        (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4))>;

def : Pat<(v2i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 0))),
        (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_0))>;
def : Pat<(v2i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 2))),
        (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_0)), subvector2_2))>;
def : Pat<(v2i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 4))),
        (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_4))>;
def : Pat<(v2i32 (extract_subvector (v8i32 V8R32IRegs:$op1), (i32 6))),
        (v2i32 (EXTRACT_SUBREG (v4i32 (EXTRACT_SUBREG (v8i32 V8R32IRegs:$op1), subvector4_4)), subvector2_6))>;

def : Pat<(v2f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 0))),
        (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_0))>;
def : Pat<(v2f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 2))),
        (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_0)), subvector2_2))>;
def : Pat<(v2f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 4))),
        (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_4))>;
def : Pat<(v2f32 (extract_subvector (v8f32 V8R32FPRegs:$op1), (i32 6))),
        (v2f32 (EXTRACT_SUBREG (v4f32 (EXTRACT_SUBREG (v8f32 V8R32FPRegs:$op1), subvector4_4)), subvector2_6))>;




//def BROADCASTV2vrr : InstTCE<(outs V2R32IRegs:$res), (ins R32IRegs:$e1, R32IRegs:$e1),"",
//[(set V2R32IRegs:$res, (build_vector R32IRegs:$e1, R32IRegs:$e1))]>;


def BUILDV2vii : InstTCE<(outs V2R32IRegs:$res), (ins i32imm:$e1, i32imm:$e2),"",
[(set V2R32IRegs:$res, (build_vector (i32 imm:$e1), (i32 imm:$e2)))]>;

def BUILDV2mkk : InstTCE<(outs V2R32FPRegs:$res), (ins f32imm:$e1, f32imm:$e2),"",
[(set V2R32FPRegs:$res, (build_vector (f32 fpimm:$e1), (f32 fpimm:$e2)))]>;




def MOV2vv : InstTCE<(outs V2R32IRegs:$res), (ins V2R32IRegs:$src), "", []>;
def MOV2mm : InstTCE<(outs V2R32FPRegs:$res), (ins V2R32FPRegs:$src), "", []>;
def MOV4ww : InstTCE<(outs V4R32IRegs:$res), (ins V4R32IRegs:$src), "", []>;
def MOV4nn : InstTCE<(outs V4R32FPRegs:$res), (ins V4R32FPRegs:$src), "", []>;
def MOV8xx : InstTCE<(outs V8R32IRegs:$res), (ins V8R32IRegs:$src), "", []>;
def MOV8oo : InstTCE<(outs V8R32FPRegs:$res), (ins V8R32FPRegs:$src), "", []>;

def BUILDV4wiiii : InstTCE<(outs V4R32IRegs:$res), (ins i32imm:$e1, i32imm:$e2, i32imm:$e3, i32imm:$e4),"",
[(set V4R32IRegs:$res, (build_vector (i32 imm:$e1), (i32 imm:$e2), (i32 imm:$e3), (i32 imm:$e4)))]>;

def BUILDV4nkkkk : InstTCE<(outs V4R32FPRegs:$res), (ins f32imm:$e1, f32imm:$e2, f32imm:$e3, f32imm:$e4),"",
[(set V4R32FPRegs:$res, (build_vector (f32 fpimm:$e1), (f32 fpimm:$e2), (f32 fpimm:$e3), (f32 fpimm:$e4)))]>;


def BUILDV8xiiiiiiii : InstTCE<(outs V8R32IRegs:$res), (ins i32imm:$e1, i32imm:$e2, i32imm:$e3, i32imm:$e4, i32imm:$e5, i32imm:$e6, i32imm:$e7, i32imm:$e8),"",
[(set V8R32IRegs:$res, (build_vector (i32 imm:$e1), (i32 imm:$e2), (i32 imm:$e3), (i32 imm:$e4), (i32 imm:$e5), (i32 imm:$e6), (i32 imm:$e7), (i32 imm:$e8)))]>;

def BUILDV8okkkkkkkk : InstTCE<(outs V8R32FPRegs:$res), (ins f32imm:$e1, f32imm:$e2, f32imm:$e3, f32imm:$e4, f32imm:$e5, f32imm:$e6, f32imm:$e7, f32imm:$e8),"",
[(set V8R32FPRegs:$res, (build_vector (f32 fpimm:$e1), (f32 fpimm:$e2), (f32 fpimm:$e3), (f32 fpimm:$e4), (f32 fpimm:$e5), (f32 fpimm:$e6), (f32 fpimm:$e7), (f32 fpimm:$e8)))]>;

