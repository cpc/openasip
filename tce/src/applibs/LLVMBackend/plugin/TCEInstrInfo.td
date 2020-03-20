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
def Bitness64 : Predicate<"subtarget_.is64bit()">;
def Bitness32 : Predicate<"!subtarget_.is64bit()">;
def LE32 : Predicate<"(!subtarget_.is64bit() && subtarget_.isLittleEndian())">;

include "GenInstrInfo0.td"

// Addressing modes.
//def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
//def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex], []>;

// Address operands
//def MEMrr : Operand<i32> {
//  let PrintMethod = "printMemOperand";
//  let MIOperandInfo = (ops R32IRegs, R32IRegs);
//}
//def MEMri : Operand<i32> {
//  let PrintMethod = "printMemOperand";
//  let MIOperandInfo = (ops R32IRegs, i32imm);
//}

def TCESRAConst : SDNode<"TCEISD::SRA_Const", SDTIntBinOp, []>;
def TCESRLConst : SDNode<"TCEISD::SRL_Const", SDTIntBinOp, []>;
def TCESHLConst : SDNode<"TCEISD::SHL_Const", SDTIntBinOp, []>;

// Branch targets have OtherVT type.
//def brtarget : Operand<OtherVT>;
//def calltarget : Operand<i32>;

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
                   [(set R32IRegs:$dst, (i32 imm:$src))]>, Requires<[Bitness32]>;

def MOVI64sa : InstTCE<(outs R64IRegs:$dst), (ins i64imm:$src),
                   "$src -> $dst;",
                   [(set R64IRegs:$dst, (i64 imm:$src))]>, Requires<[Bitness64]>;

def PRED_TRUE_MOVI64sa : InstTCE<(outs R64IRegs:$dst),
                   (ins R1Regs:$pred, i64imm:$src),
                   "$src -> $dst;", []>;

def PRED_FALSE_MOVI64sa : InstTCE<(outs R64IRegs:$dst),
                   (ins R1Regs:$pred, i64imm:$src),
                   "$src -> $dst;", []>;

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

def MOV64ss : InstTCE<(outs R64Regs:$dst), (ins R64Regs:$src),
	           "$src -> $dst;",
                   []>, Requires<[Bitness64]>;

def PRED_TRUE_MOV64ss : InstTCE<(outs R64Regs:$dst),
    (ins R1Regs:$pred, R64Regs:$src),
	           "$src -> $dst;",
                   []>, Requires<[Bitness64]>;

def PRED_FALSE_MOV64ss : InstTCE<(outs R64Regs:$dst),
    (ins R1Regs:$pred, R64Regs:$src),
	           "$src -> $dst;",
                   []>, Requires<[Bitness64]>;

def MOVI1I32rr : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$src),
                     "$src -> $dst;",
                    []>;

def MOVI32I1rr : InstTCE<(outs R1Regs:$dst), (ins R32IRegs:$src),
                     "$src -> $dst;",
                    [(set R1Regs:$dst, (trunc R32IRegs:$src))]>;

def MOVI64I1ss : InstTCE<(outs R1Regs:$dst), (ins R64IRegs:$src),
                     "$src -> $dst;",
                    [(set R1Regs:$dst, (trunc R64IRegs:$src))]>, Requires<[Bitness64]>;

def MOVFI32rf : InstTCE<(outs R32IRegs:$dst), (ins R32FPRegs:$src),
	           "$src -> $dst;",
                   [(set R32IRegs:$dst, (bitconvert R32FPRegs:$src))]>;

def MOVIF32fr : InstTCE<(outs R32FPRegs:$dst), (ins R32IRegs:$src),
	           "$src -> $dst;",
                   [(set R32FPRegs:$dst, (bitconvert R32IRegs:$src))]>;

def MOVF32ff : InstTCE<(outs R32FPRegs:$dst), (ins R32FPRegs:$src),
	           "$src -> $dst;",
                   []>;

def MOVFI64rf : InstTCE<(outs R64IRegs:$dst), (ins R64FPRegs:$src),
	           "$src -> $dst;",
                   [(set R64IRegs:$dst, (bitconvert R64FPRegs:$src))]>, Requires<[Bitness64]>;

def MOVIF64fr : InstTCE<(outs R64FPRegs:$dst), (ins R64IRegs:$src),
	           "$src -> $dst;",
                   [(set R64FPRegs:$dst, (bitconvert R64IRegs:$src))]>, Requires<[Bitness64]>;

def MOVF64ff : InstTCE<(outs R64FPRegs:$dst), (ins R64FPRegs:$src),
	           "$src -> $dst;",
                   []>, Requires<[Bitness64]>;


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

include "GenInstrInfo.td"

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

    def LD64fr : InstTCE<(outs R64FPRegs:$op2),
        (ins MEMrr:$op1), "", [(set R64FPRegs:$op2,
        (load ADDRrr:$op1))]>, Requires<[Bitness64]>;

    def LD64fi : InstTCE<(outs R64FPRegs:$op2),
        (ins MEMri:$op1), "", [(set R64FPRegs:$op2,
        (load ADDRri:$op1))]>, Requires<[Bitness64]>;

    def ST32fr : InstTCE<(outs), (ins MEMrr:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;
    def ST32fi : InstTCE<(outs), (ins MEMri:$op1, R32FPRegs:$op2), "",
        [(store R32FPRegs:$op2, ADDRri:$op1)]>, Requires<[LittleEndian]>;

    def ST64fr : InstTCE<(outs), (ins MEMrr:$op1, R64FPRegs:$op2), "",
        [(store R64FPRegs:$op2, ADDRrr:$op1)]>, Requires<[Bitness64]>;

    def ST64fi : InstTCE<(outs), (ins MEMri:$op1, R64FPRegs:$op2), "",
        [(store R64FPRegs:$op2, ADDRri:$op1)]>, Requires<[Bitness64]>;

    def ST16hr : InstTCE<(outs), (ins MEMrr:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRrr:$op1)]>, Requires<[LittleEndian]>;

    def ST16hi : InstTCE<(outs), (ins MEMri:$op1, R32HFPRegs:$op2), "",
        [(store R32HFPRegs:$op2, ADDRri:$op1)]>, Requires<[LittleEndian]>;



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

// return address stack store/load
def STWRArr : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>, Requires<[BigEndian]>;
def LDWRAr : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>, Requires<[BigEndian]>;

def ST32RArr : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>, Requires<[LE32]>;
def LD32RAr : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>, Requires<[LE32]>;

def ST64RAss : InstTCE<(outs), (ins MEMrr:$op1,RAReg:$op2), "", [(store RAReg:$op2, ADDRrr:$op1)]>, Requires<[Bitness64]>;
def LD64RAs : InstTCE<(outs RAReg:$op2), (ins MEMrr:$op1), "", [(set RAReg:$op2, (load ADDRrr:$op1))]>, Requires<[Bitness64]>;



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

//def : Pat<(i1 (setne (trunc R32IRegs:$op1), R1Regs:$op2)), (XORrb R1Regs:$op1, R1Regs:$op2)>, Requires<[Bitness32]>;
//def : Pat<(i1 (seteq (trunc R1Regs:$op1), R1Regs:$op2)), (XORbj (XORrb R32IRegs:$op1, R1Regs:$op2), 1)>, Requires<[Bitness32]>;





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

