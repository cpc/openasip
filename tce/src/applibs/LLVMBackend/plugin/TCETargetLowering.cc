/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TCETargetLowering.cpp
 *
 * Implementation of TCETargetLowering class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <assert.h>
#include <string>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/CodeGen/SelectionDAG.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/ADT/VectorExtras.h>
#include <llvm/Intrinsics.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCETargetMachine.hh"
#include "TCESubtarget.hh"
#include "TCETargetLowering.hh"
#include "tce_config.h"

#include <iostream> // DEBUG

using namespace llvm;


/**
 * Returns target node opcode names for debugging purposes.
 *
 * @param opcode Opcode to convert to string.
 * @return Opcode name.
 */
const char*
TCETargetLowering::getTargetNodeName(unsigned opcode) const {
    switch (opcode) {
    default: return NULL;
    case TCEISD::CALL: return "TCEISD::CALL";
    case TCEISD::RET_FLAG: return "TCEISD::RET_FLAG";
    case TCEISD::GLOBAL_ADDR: return "TCEISD::GLOBAL_ADDR";
    case TCEISD::CONST_POOL: return "TCEISD::CONST_POOL";

    case TCEISD::SELECT_I1: return "TCEISD::SELECT_I1";
    case TCEISD::SELECT_I8: return "TCEISD::SELECT_I8";
    case TCEISD::SELECT_I16: return "TCEISD::SELECT_I16";
    case TCEISD::SELECT_I32: return "TCEISD::SELECT_I32";
    case TCEISD::SELECT_I64: return "TCEISD::SELECT_I64";
    case TCEISD::SELECT_F32: return "TCEISD::SELECT_F32";
    case TCEISD::SELECT_F64: return "TCEISD::SELECT_F64";
    }
}

/**
 * The Constructor.
 *
 * Initializes the target lowering.
 */
TCETargetLowering::TCETargetLowering(TCETargetMachine& tm) :
    TargetLowering(tm), tm_(tm) {

    addRegisterClass(MVT::i1, TCE::I1RegsRegisterClass);
    addRegisterClass(MVT::i32, TCE::I32RegsRegisterClass);
    addRegisterClass(MVT::f32, TCE::F32RegsRegisterClass);

    //setLoadXAction(ISD::EXTLOAD, MVT::f32, Expand);
    //setLoadXAction(ISD::EXTLOAD, MVT::i1 , Promote);
    //setLoadXAction(ISD::ZEXTLOAD, MVT::i1, Expand);

    setOperationAction(ISD::UINT_TO_FP, MVT::i1   , Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i8   , Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i16  , Promote);

    setOperationAction(ISD::SINT_TO_FP, MVT::i1   , Promote);
    setOperationAction(ISD::SINT_TO_FP, MVT::i8   , Promote);
    setOperationAction(ISD::SINT_TO_FP, MVT::i16  , Promote);

    setOperationAction(ISD::FP_TO_UINT, MVT::i1   , Promote);
    setOperationAction(ISD::FP_TO_UINT, MVT::i8   , Promote);
    setOperationAction(ISD::FP_TO_UINT, MVT::i16  , Promote);

    setOperationAction(ISD::FP_TO_SINT, MVT::i1   , Promote);
    setOperationAction(ISD::FP_TO_SINT, MVT::i8   , Promote);
    setOperationAction(ISD::FP_TO_SINT, MVT::i16  , Promote);

    setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
    setOperationAction(ISD::ConstantPool , MVT::i32, Custom);
    setOperationAction(ISD::RET,           MVT::Other, Custom);

    // SELECT is used instead of SELECT_CC
    setOperationAction(ISD::SELECT_CC, MVT::Other, Expand);

    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

    // Expand indirect branches.
    setOperationAction(ISD::BRIND, MVT::Other, Expand);
    // Expand jumptable branches.
    setOperationAction(ISD::BR_JT, MVT::Other, Expand);
    // Expand conditional branches.
    setOperationAction(ISD::BR_CC, MVT::Other, Expand);

    setOperationAction(ISD::MULHU,  MVT::i32, Expand);
    setOperationAction(ISD::MULHS,  MVT::i32, Expand);
    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

    setOperationAction(ISD::DBG_STOPPOINT, MVT::Other, Expand);
    setOperationAction(ISD::DEBUG_LOC, MVT::Other, Expand);
    setOperationAction(ISD::DBG_LABEL, MVT::Other, Expand);

    setOperationAction(ISD::VASTART           , MVT::Other, Custom);
    setOperationAction(ISD::VAARG             , MVT::Other, Expand);
    setOperationAction(ISD::VACOPY            , MVT::Other, Expand);
    setOperationAction(ISD::VAEND             , MVT::Other, Expand);

    setOperationAction(ISD::STACKSAVE         , MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE      , MVT::Other, Expand);
    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32  , Expand);

    setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
    setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);

    setOperationAction(ISD::ConstantFP, MVT::f64, Expand);
    setOperationAction(ISD::ConstantFP, MVT::f32, Expand);

    setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::SMUL_LOHI, MVT::i64, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i64, Expand);

    setOperationAction(ISD::BSWAP, MVT::i32, Expand);

    setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
    setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

    setStackPointerRegisterToSaveRestore(TCE::SP);

    // Set missing operations that can be emulated with emulation function
    // to be expanded.
    const std::set<unsigned>* missingOps = tm.missingOperations();
    std::set<unsigned>::const_iterator iter = missingOps->begin();

    std::cerr << "Missing ops: ";

    while (iter != missingOps->end()) {
      switch (*iter) {
      case ISD::SDIV: std::cerr << "SDIV,"; break;
      case ISD::UDIV: std::cerr << "UDIV,"; break;
      case ISD::SREM: std::cerr << "SREM,"; break;
      case ISD::UREM: std::cerr << "UREM,"; break;
      case ISD::ROTL: std::cerr << "ROTL,"; break;
      case ISD::ROTR: std::cerr << "ROTR,"; break;
      case ISD::MUL:  std::cerr << "MUL,"; break;
      default: std::cerr << *iter << ", "; break;
      };
      unsigned nodetype = *iter;
      setOperationAction(nodetype, MVT::i32, Expand);
      iter++;
    }
    std::cerr << std::endl;

    computeRegisterProperties();
}


/**
 * The Destructor.
 */
TCETargetLowering::~TCETargetLowering() {
}


/**
 * Handles custom operation lowerings.
 */
SDValue
TCETargetLowering::LowerOperation(SDValue op, SelectionDAG& dag) {

    switch(op.getOpcode()) {
    case ISD::RET: return LowerRET(op, dag);
    case ISD::GlobalAddress: {
        GlobalValue* gv = cast<GlobalAddressSDNode>(op)->getGlobal();
        SDValue ga = dag.getTargetGlobalAddress(gv, MVT::i32);
        return dag.getNode(TCEISD::GLOBAL_ADDR, op.getDebugLoc(), MVT::i32, ga);
    }
    case ISD::DYNAMIC_STACKALLOC: {
        assert(false && "Dynamic stack allocation not yet implemented.");
    }
    case ISD::SELECT: {
        return lowerSELECT(op, dag);
    }
    case ISD::VASTART: {
        llvm::MVT ptrVT = dag.getTargetLoweringInfo().getPointerTy();
        SDValue fr = dag.getFrameIndex(varArgsFrameIndex_, ptrVT);
        SrcValueSDNode* sv = cast<SrcValueSDNode>(op.getOperand(2));
        return dag.getStore(
            op.getOperand(0), op.getDebugLoc(), fr, op.getOperand(1), sv->getValue(), 0);
    }
    case ISD::ConstantPool: {
        // TODO: Check this.
        llvm::MVT ptrVT = op.getValueType();
        ConstantPoolSDNode* cp = cast<ConstantPoolSDNode>(op);
        SDValue res;
        if (cp->isMachineConstantPoolEntry()) {
            res = dag.getTargetConstantPool(
                cp->getMachineCPVal(), ptrVT,
                cp->getAlignment());
        } else {
            res = dag.getTargetConstantPool(
                cp->getConstVal(), ptrVT,
                cp->getAlignment());
        }
        return dag.getNode(TCEISD::CONST_POOL, op.getDebugLoc(), MVT::i32, res);
    }     
    }
    op.getNode()->dump(&dag);
    assert(0 && "Custom lowerings not implemented!");
}

/**
 * Lowers return operation.
 */
SDValue
TCETargetLowering::LowerRET(SDValue op, SelectionDAG& dag) {
    
    SDValue copy;
    switch (op.getNumOperands()) {
    default: {
        std::cerr  << "Do not know how to return "
                   << op.getNumOperands() << " operands!"
                   << std::endl;

        assert(false);
    }
    case 1: {
        return SDValue();
    }
    case 3: {
        unsigned argReg;
        switch(op.getOperand(1).getValueType().getSimpleVT()) {
        default: assert(0 && "Unknown type to return");
        case MVT::i32: argReg = TCE::IRES0; break;
        case MVT::f32: argReg = TCE::FRES0; break;
	 //case MVT::i64: argReg = TCE::DIRES0; break;
	 //case MVT::f64: argReg = TCE::DRES0; break;
        }
        copy = dag.getCopyToReg(
            op.getOperand(0), op.getDebugLoc(), argReg, op.getOperand(1),
            SDValue());
        break;
    }
    case 5: {
        // TODO: i64 return value is now split in two registers:
        // lo part in IRES0 and hi part in KLUDGE_REGISTER
        assert(op.getOperand(1).getValueType() == MVT::i32);
        assert(op.getOperand(3).getValueType() == MVT::i32);

        copy = dag.getCopyToReg(
            op.getOperand(0), op.getDebugLoc(), TCE::IRES0, op.getOperand(3),
            SDValue());

        copy = dag.getCopyToReg(
            copy, op.getDebugLoc(), TCE::KLUDGE_REGISTER, op.getOperand(1),
            copy.getValue(1));

        break;
    }
    }
    return dag.getNode(
        TCEISD::RET_FLAG, op.getDebugLoc(), MVT::Other, copy, copy.getValue(1));

}

/**
 * Lowers FORMAL_ARGUMENTS node.
 */
void
TCETargetLowering::LowerArguments(
    Function& f, SelectionDAG& dag, 
    SmallVectorImpl<SDValue>& argValues,
    DebugLoc dl) {
 
    MachineFunction& mf = dag.getMachineFunction();
    unsigned argOffset = 0;
    
    SDValue root = dag.getRoot();

    Function::arg_iterator iter = f.arg_begin();
    for (; iter != f.arg_end(); iter++) {
        llvm::MVT objectVT = getValueType(iter->getType());

        switch (objectVT.getSimpleVT()) {
        default: assert(
            false &&
            "TCETargetLowering::LowerArguments(): unhandled argument type");

        case MVT::i1:
            // fall through
        case MVT::i8:
            // fall through
        case MVT::i16:
            // fall through
        case MVT::i32: {
            // Argument always in stack.
            int frameIdx =
                mf.getFrameInfo()->CreateFixedObject(4, argOffset);
            SDValue fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            SDValue load;
            if (objectVT == MVT::i32) {
                load = dag.getLoad(MVT::i32, dl, root, fiPtr, NULL, 0);
            } else {
                ISD::LoadExtType loadOp = ISD::SEXTLOAD;
                unsigned offset = 0;
                if (objectVT == MVT::i1 || objectVT == MVT::i8) {
                    offset = 3;
                    } else if (objectVT == MVT::i16) {
                    offset = 2;
                } else {
                    assert(false);
                }
                fiPtr = dag.getNode(
                    ISD::ADD, dl, MVT::i32, fiPtr,
                    dag.getConstant(offset, MVT::i32));
                    
                load = dag.getExtLoad(loadOp, dl, MVT::i32, root, fiPtr,
                                      NULL, 0, objectVT);
                    
                load = dag.getNode(ISD::TRUNCATE, dl, objectVT, load);
            }
            argValues.push_back(load);
            argOffset += 4;
            break;
        }
        case MVT::i64: {
            // Following sparc example: i64 is split into lo/hi parts.
            SDValue loVal, hiVal;

            int frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset);
            SDValue fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            hiVal = dag.getLoad(MVT::i32, dl, root, fiPtr, NULL, 0);

            frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);
            fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            loVal = dag.getLoad(MVT::i32, dl, root, fiPtr, NULL, 0);

            // Compose the two halves together into an i64 unit.
            SDValue wholeValue =
                dag.getNode(ISD::BUILD_PAIR, dl, MVT::i64, loVal, hiVal);

            argValues.push_back(wholeValue);
            argOffset += 8;
            break;
        }
        case MVT::f32: {
            // For now, f32s are always passed in the stack.
            int frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset);
            SDValue fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            SDValue load = dag.getLoad(MVT::f32, dl, root, fiPtr, NULL, 0);
            argValues.push_back(load);
            argOffset += 4;
            break;
        }
        case MVT::f64:
            SDValue loVal, hiVal;

            int frameIdxHi =
                mf.getFrameInfo()->CreateFixedObject(4, argOffset);

            SDValue hiFiPtr = dag.getFrameIndex(frameIdxHi, MVT::i32);
            hiVal = dag.getLoad(MVT::i32, dl, root, hiFiPtr, NULL, 0);
	       
            int frameIdxLo =
                mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);

            SDValue loFiPtr = dag.getFrameIndex(frameIdxLo, MVT::i32);
            loVal = dag.getLoad(MVT::i32, dl, root, loFiPtr, NULL, 0);

            // Compose the two halves together into an i64 unit.
            SDValue wholeValue =
                dag.getNode(ISD::BUILD_PAIR, dl, MVT::f64, loVal, hiVal);

            argValues.push_back(wholeValue);
            argOffset += 8;
	    break;
        }
    }

    // In case of a variadic argument function, all parameters are passed
    // in the stack.
    if(f.isVarArg()) {
        varArgsFrameIndex_ =
            mf.getFrameInfo()->CreateFixedObject(4, argOffset);
    }

    switch (getValueType(f.getReturnType()).getSimpleVT()) {
    default: assert(0 && "Unknown type!");
    case MVT::isVoid: break;
    case MVT::i1:
    case MVT::i8:
    case MVT::i16:
    case MVT::i32:
        //mf.addLiveOut(TCE::IRES0);
        mf.getRegInfo().addLiveOut(TCE::IRES0);
        break;
    case MVT::f32:
        //mf.addLiveOut(TCE::FRES0);
        mf.getRegInfo().addLiveOut(TCE::FRES0);
        break;
    case MVT::i64:
    case MVT::f64: {
        //mf.addLiveOut(TCE::IRES0);
        //mf.addLiveOut(TCE::KLUDGE_REGISTER);
        mf.getRegInfo().addLiveOut(TCE::IRES0);
        mf.getRegInfo().addLiveOut(TCE::KLUDGE_REGISTER);
        break;
    }
    }
}


/**
 * Lowers call pseudo instruction to target specific sequence.
 */
std::pair<SDValue, SDValue>
TCETargetLowering::LowerCallTo(
    SDValue chain, const Type* retTy, bool retTyIsSigned, bool,
    bool isVarArg, bool,  unsigned cc, bool isTailCall, SDValue callee,
    ArgListTy& args, SelectionDAG& dag, DebugLoc dl) {

    unsigned argsSize = 0;
    for (unsigned i = 0; i < args.size(); i++) {
        switch(getValueType(args[i].Ty).getSimpleVT()) {
        default: assert(false && "Unknown value type!");
        case MVT::i1: {
            argsSize += 4;
            break;
        }
        case MVT::i8: {
            argsSize += 4;
            break;
        }
        case MVT::i16: {
            argsSize += 4;
            break;
        }
        case MVT::f32: {
            argsSize += 4;
            break;
        }
        case MVT::i32: {
            argsSize += 4;
            break;
        }
        case MVT::i64: {
            argsSize += 8;
            break;
        }
        case MVT::f64: {
            argsSize += 8;
            break;
        }
        }
    }

    // Keep stack frames 4-byte aligned.
    argsSize = (argsSize+3) & ~3;

    chain = dag.getCALLSEQ_START(
        chain, dag.getConstant(argsSize, getPointerTy()));

    SDValue stackPtr;
    std::vector<SDValue> stores;
    unsigned argOffset = 0;

    for (unsigned i = 0; i < args.size(); i++) {
        SDValue val = args[i].Node;
        llvm::MVT objectVT = val.getValueType();
        SDValue valToStore(0, 0);
        unsigned objSize = 0;

        switch (objectVT.getSimpleVT()) {
        default: assert(false && "Unhandled argument type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            // Promote the integer to 32-bits.
            ISD::NodeType ext = ISD::ANY_EXTEND;
            if (args[i].isSExt) {
                ext = ISD::SIGN_EXTEND;
            } else if (args[i].isZExt) {
                ext = ISD::ZERO_EXTEND;
            }
            val = dag.getNode(ext, dl, MVT::i32, val);
            // FALL THROUGH
        }
        case MVT::i32: {
            objSize = 4;
            valToStore = val;
            break;
        }
        case MVT::i64: {
            objSize = 8;
            valToStore = val;    // Pass in stack.
            break;
        }
        case MVT::f32: {
            objSize = 4;
            valToStore = val;
            break;
        }
        case MVT::f64: {
            objSize = 8;
            valToStore = val;
            break;
        }
        }

        if (valToStore.getNode()) {
            if (!stackPtr.getNode()) {
                stackPtr = dag.getRegister(TCE::SP, MVT::i32);
            }
            SDValue ptrOff = dag.getConstant(argOffset, getPointerTy());
            ptrOff = dag.getNode(ISD::ADD, dl, MVT::i32, stackPtr, ptrOff);
            stores.push_back(
                dag.getStore(chain, dl, valToStore, ptrOff, NULL, 0));

            argOffset += objSize;
        }
    }

    
    // Emit all stores, make sure the occur before any copies into physregs.
    if (!stores.empty())
        chain = dag.getNode(
            ISD::TokenFactor, dl, MVT::Other, &stores[0], stores.size());

    SDValue inFlag;

    // Turn global addressses to target global adresses.
    if (GlobalAddressSDNode* g = dyn_cast<GlobalAddressSDNode>(callee))
        callee = dag.getTargetGlobalAddress(g->getGlobal(), MVT::i32);
    else if (ExternalSymbolSDNode* e = dyn_cast<ExternalSymbolSDNode>(callee))
        callee = dag.getTargetExternalSymbol(e->getSymbol(), MVT::i32);

    std::vector<llvm::MVT> nodeTys;
    nodeTys.push_back(MVT::Other);
    nodeTys.push_back(MVT::Flag);
    SDValue ops[] = { chain, callee, inFlag };
    chain = dag.getNode(TCEISD::CALL, dl, nodeTys, ops, inFlag.getNode() ? 3 : 2);
    inFlag = chain.getValue(1);

    // Return value
    MVT retTyVT = getValueType(retTy);
    SDValue retVal;
    if (retTyVT != MVT::isVoid) {
        switch (retTyVT.getSimpleVT()) {
        default: assert(false && "Unsupported return value type.");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            retVal = dag.getCopyFromReg(chain, dl, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            retVal = dag.getNode(ISD::TRUNCATE, dl, retTyVT, retVal);
            break;
        }
        case MVT::i32: {
            retVal = dag.getCopyFromReg(chain, dl, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::i64: {
            SDValue lo = dag.getCopyFromReg(
                chain, dl, TCE::IRES0, MVT::i32, inFlag);

            SDValue hi = dag.getCopyFromReg(
                lo.getValue(1), dl, TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, dl, MVT::i64, lo, hi);
            chain = hi.getValue(1);
            break;
        }
        case MVT::f32: {
            retVal = dag.getCopyFromReg(chain, dl, TCE::FRES0, MVT::f32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::f64: {
            SDValue lo = dag.getCopyFromReg(
                chain, dl, TCE::IRES0, MVT::i32, inFlag);

            SDValue hi = dag.getCopyFromReg(
                lo.getValue(1), dl, TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, dl, MVT::i64, lo, hi);
            chain = hi.getValue(1);
            break;
        }
        }
    }

    // copied from SPARC backend:
    chain = 
        dag.getCALLSEQ_END(
            chain, dag.getConstant(argsSize, getPointerTy()),
            dag.getConstant(0, MVT::i32), chain.getValue(1));

    return std::make_pair(retVal, chain);
}


/**
 * Converts SELECT nodes to TCE-specific pseudo instructions.
 */
SDValue
TCETargetLowering::lowerSELECT(
    SDValue op, SelectionDAG& dag) {

    SDValue cond = op.getOperand(0);
    SDValue trueVal = op.getOperand(1);
    SDValue falseVal = op.getOperand(2);

    unsigned opcode = 0;
    switch(trueVal.getValueType().getSimpleVT()) {
    default: assert(0 && "Unknown type to select.");
    case MVT::i1: {
        opcode = TCEISD::SELECT_I1;
        break;
    }
    case MVT::i8: {
        opcode = TCEISD::SELECT_I8;
        break;
    }
    case MVT::i16: {
        opcode = TCEISD::SELECT_I16;
        break;
    }
    case MVT::i32: {
        opcode = TCEISD::SELECT_I32;
        break;
    }
    case MVT::i64: {
        opcode = TCEISD::SELECT_I64;
        break;
    }
    case MVT::f32: {
        opcode = TCEISD::SELECT_F32;
        break;
    }
    case MVT::f64: {
        opcode = TCEISD::SELECT_F64;
        break;
    }
    }

    return dag.getNode(
        opcode, op.getDebugLoc(), trueVal.getValueType(), trueVal, falseVal, cond);
}

/**
 * Inline ASM constraint support for custom op definitions.
 */
std::pair<unsigned, const TargetRegisterClass*>
TCETargetLowering::getRegForInlineAsmConstraint(
    const std::string& constraint,
    llvm::MVT vt) const {

    if (constraint == "r") {
        return std::make_pair(0U, TCE::I32RegsRegisterClass);
    }

    if (constraint == "f") {
        if (vt == MVT::f32) {
            return std::make_pair(0U, TCE::F32RegsRegisterClass);
        } else if (vt == MVT::f64) {
            return std::make_pair(0U, TCE::F64RegsRegisterClass);
        }
    }
    return TargetLowering::getRegForInlineAsmConstraint(constraint, vt);
}

/**
 * Returns the preferred comparison result type.
 */
llvm::MVT
TCETargetLowering::getSetCCResultType(llvm::MVT VT) const {
   return llvm::MVT::i1;
}
