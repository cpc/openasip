/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
//    setLoadXAction(ISD::EXTLOAD, MVT::i1   , Promote);
//    setLoadXAction(ISD::ZEXTLOAD, MVT::i1   , Expand);

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

    setOperationAction(ISD::LOCATION, MVT::Other, Expand);
    setOperationAction(ISD::DEBUG_LOC, MVT::Other, Expand);
    setOperationAction(ISD::LABEL, MVT::Other, Expand);

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

    setStackPointerRegisterToSaveRestore(TCE::SP);

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
SDOperand
TCETargetLowering::LowerOperation(SDOperand op, SelectionDAG& dag) {

    switch(op.getOpcode()) {
    case ISD::RET: return LowerRET(op, dag);
    case ISD::GlobalAddress: {
        GlobalValue* gv = cast<GlobalAddressSDNode>(op)->getGlobal();
        SDOperand ga = dag.getTargetGlobalAddress(gv, MVT::i32);
        return dag.getNode(TCEISD::GLOBAL_ADDR, MVT::i32, ga);
    }
    case ISD::DYNAMIC_STACKALLOC: {
        assert(false && "Dynamic stack allocation not yet implemented.");
    }
    case ISD::SELECT: {
        return lowerSELECT(op, dag);
    }
    case ISD::VASTART: {
        MVT::ValueType ptrVT = dag.getTargetLoweringInfo().getPointerTy();
        SDOperand fr = dag.getFrameIndex(varArgsFrameIndex_, ptrVT);
        SrcValueSDNode* sv = cast<SrcValueSDNode>(op.getOperand(2));
        return dag.getStore(
            op.getOperand(0), fr, op.getOperand(1), sv->getValue(), 0);
    }
    case ISD::ConstantPool: {
        // TODO: Check this.
        MVT::ValueType ptrVT = op.getValueType();
        ConstantPoolSDNode* cp = cast<ConstantPoolSDNode>(op);
        SDOperand res;
        if (cp->isMachineConstantPoolEntry()) {
            res = dag.getTargetConstantPool(
                cp->getMachineCPVal(), ptrVT,
                cp->getAlignment());
        } else {
            res = dag.getTargetConstantPool(
                cp->getConstVal(), ptrVT,
                cp->getAlignment());
        }
        return dag.getNode(TCEISD::CONST_POOL, MVT::i32, res);
    }
    }
    op.Val->dump(&dag);
    assert(0 && "Custom lowerings not implemented!");
}

/**
 * Lowers return operation.
 */
SDOperand
TCETargetLowering::LowerRET(SDOperand op, SelectionDAG& dag) {
    
    SDOperand copy;
    switch (op.getNumOperands()) {
    default: {
        std::cerr  << "Do not know how to return "
                   << op.getNumOperands() << " operands!"
                   << std::endl;

        assert(false);
    }
    case 1: {
        return SDOperand();
    }
    case 3: {
        unsigned argReg;
        switch(op.getOperand(1).getValueType()) {
        default: assert(0 && "Unknown type to return");
        case MVT::i32: argReg = TCE::IRES0; break;
        case MVT::f32: argReg = TCE::FRES0; break;
	 //case MVT::i64: argReg = TCE::DIRES0; break;
	 //case MVT::f64: argReg = TCE::DRES0; break;
        }
        copy = dag.getCopyToReg(
            op.getOperand(0), argReg, op.getOperand(1),
            SDOperand());
        break;
    }
    case 5: {
        // TODO: i64 return value is now split in two registers:
        // lo part in IRES0 and hi part in KLUDGE_REGISTER
        assert(op.getOperand(1).getValueType() == MVT::i32);
        assert(op.getOperand(3).getValueType() == MVT::i32);

        copy = dag.getCopyToReg(
            op.getOperand(0), TCE::IRES0, op.getOperand(3),
            SDOperand());

        copy = dag.getCopyToReg(
            copy, TCE::KLUDGE_REGISTER, op.getOperand(1),
            copy.getValue(1));

        break;
    }
    }
    return dag.getNode(
        TCEISD::RET_FLAG, MVT::Other, copy, copy.getValue(1));

}

/**
 * Lowers FORMAL_ARGUMENTS node.
 */
std::vector<SDOperand>
TCETargetLowering::LowerArguments(Function& f, SelectionDAG& dag) {
 
    MachineFunction& mf = dag.getMachineFunction();
    unsigned argOffset = 0;
    std::vector<SDOperand> argValues;
    SDOperand root = dag.getRoot();

    Function::arg_iterator iter = f.arg_begin();
    for (; iter != f.arg_end(); iter++) {
        MVT::ValueType objectVT = getValueType(iter->getType());

        switch (objectVT) {
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
            SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            SDOperand load;
            if (objectVT == MVT::i32) {
                load = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);
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
                    ISD::ADD, MVT::i32, fiPtr,
                    dag.getConstant(offset, MVT::i32));
                    
                load = dag.getExtLoad(loadOp, MVT::i32, root, fiPtr,
                                      NULL, 0, objectVT);
                    
                load = dag.getNode(ISD::TRUNCATE, objectVT, load);
            }
            argValues.push_back(load);
            argOffset += 4;
            break;
        }
        case MVT::i64: {
            // Following sparc example: i64 is split into lo/hi parts.
            SDOperand loVal, hiVal;

            int frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset);
            SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            hiVal = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);

            frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);
            fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            loVal = dag.getLoad(MVT::i32, root, fiPtr, NULL, 0);

            // Compose the two halves together into an i64 unit.
            SDOperand wholeValue =
                dag.getNode(ISD::BUILD_PAIR, MVT::i64, loVal, hiVal);

            argValues.push_back(wholeValue);
            argOffset += 8;
            break;
        }
        case MVT::f32: {
            // For now, f32s are always passed in the stack.
            int frameIdx = mf.getFrameInfo()->CreateFixedObject(4, argOffset);
            SDOperand fiPtr = dag.getFrameIndex(frameIdx, MVT::i32);
            SDOperand load = dag.getLoad(MVT::f32, root, fiPtr, NULL, 0);
            argValues.push_back(load);
            argOffset += 4;
            break;
        }
        case MVT::f64:
            SDOperand loVal, hiVal;

            int frameIdxHi =
                mf.getFrameInfo()->CreateFixedObject(4, argOffset);

            SDOperand hiFiPtr = dag.getFrameIndex(frameIdxHi, MVT::i32);
            hiVal = dag.getLoad(MVT::i32, root, hiFiPtr, NULL, 0);
	       
            int frameIdxLo =
                mf.getFrameInfo()->CreateFixedObject(4, argOffset+4);

            SDOperand loFiPtr = dag.getFrameIndex(frameIdxLo, MVT::i32);
            loVal = dag.getLoad(MVT::i32, root, loFiPtr, NULL, 0);

            // Compose the two halves together into an i64 unit.
            SDOperand wholeValue =
                dag.getNode(ISD::BUILD_PAIR, MVT::f64, loVal, hiVal);

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

    switch (getValueType(f.getReturnType())) {
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
    return argValues;
}


/**
 * Lowers call pseudo instruction to target specific sequence.
 */
std::pair<SDOperand, SDOperand>
TCETargetLowering::LowerCallTo(
    SDOperand chain, const Type* retTy, bool retTyIsSigned, bool,
    bool isVarArg, unsigned cc, bool isTailCall, SDOperand callee,
    ArgListTy& args, SelectionDAG& dag) {

    unsigned argsSize = 0;
    for (unsigned i = 0; i < args.size(); i++) {
        switch(getValueType(args[i].Ty)) {
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

    SDOperand stackPtr;
    std::vector<SDOperand> stores;
    unsigned argOffset = 0;

    for (unsigned i = 0; i < args.size(); i++) {
        SDOperand val = args[i].Node;
        MVT::ValueType objectVT = val.getValueType();
        SDOperand valToStore(0, 0);
        unsigned objSize = 0;

        switch (objectVT) {
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
            val = dag.getNode(ext, MVT::i32, val);
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

        if (valToStore.Val) {
            if (!stackPtr.Val) {
                stackPtr = dag.getRegister(TCE::SP, MVT::i32);
            }
            SDOperand ptrOff = dag.getConstant(argOffset, getPointerTy());
            ptrOff = dag.getNode(ISD::ADD, MVT::i32, stackPtr, ptrOff);
            stores.push_back(
                dag.getStore(chain, valToStore, ptrOff, NULL, 0));

            argOffset += objSize;
        }
    }

    
    // Emit all stores, make sure the occur before any copies into physregs.
    if (!stores.empty())
        chain = dag.getNode(
            ISD::TokenFactor, MVT::Other, &stores[0], stores.size());

    SDOperand inFlag;

    // Turn global addressses to target global adresses.
    if (GlobalAddressSDNode* g = dyn_cast<GlobalAddressSDNode>(callee))
        callee = dag.getTargetGlobalAddress(g->getGlobal(), MVT::i32);
    else if (ExternalSymbolSDNode* e = dyn_cast<ExternalSymbolSDNode>(callee))
        callee = dag.getTargetExternalSymbol(e->getSymbol(), MVT::i32);

    std::vector<MVT::ValueType> nodeTys;
    nodeTys.push_back(MVT::Other);
    nodeTys.push_back(MVT::Flag);
    SDOperand ops[] = { chain, callee, inFlag };
    chain = dag.getNode(TCEISD::CALL, nodeTys, ops, inFlag.Val ? 3 : 2);
    inFlag = chain.getValue(1);

    // Return value
    MVT::ValueType retTyVT = getValueType(retTy);
    SDOperand retVal;
    if (retTyVT != MVT::isVoid) {
        switch (retTyVT) {
        default: assert(false && "Unsupported return value type.");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16: {
            retVal = dag.getCopyFromReg(chain, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            retVal = dag.getNode(ISD::TRUNCATE, retTyVT, retVal);
            break;
        }
        case MVT::i32: {
            retVal = dag.getCopyFromReg(chain, TCE::IRES0, MVT::i32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::i64: {
            SDOperand lo = dag.getCopyFromReg(
                chain, TCE::IRES0, MVT::i32, inFlag);

            SDOperand hi = dag.getCopyFromReg(
                lo.getValue(1), TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, MVT::i64, lo, hi);
            chain = hi.getValue(1);
            break;
        }
        case MVT::f32: {
            retVal = dag.getCopyFromReg(chain, TCE::FRES0, MVT::f32, inFlag);
            chain = retVal.getValue(1);
            break;
        }
        case MVT::f64: {
            SDOperand lo = dag.getCopyFromReg(
                chain, TCE::IRES0, MVT::i32, inFlag);

            SDOperand hi = dag.getCopyFromReg(
                lo.getValue(1), TCE::KLUDGE_REGISTER, MVT::i32, 
                lo.getValue(2));

            retVal = dag.getNode(ISD::BUILD_PAIR, MVT::i64, lo, hi);
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

#if 0
    // LLVM 2.2:
    chain = dag.getNode(ISD::CALLSEQ_END, MVT::Other, chain,
                        dag.getConstant(argsSize, getPointerTy()));
#endif
    return std::make_pair(retVal, chain);
}


/**
 * Converts SELECT nodes to TCE-specific pseudo instructions.
 */
SDOperand
TCETargetLowering::lowerSELECT(
    SDOperand op, SelectionDAG& dag) {

    SDOperand cond = op.getOperand(0);
    SDOperand trueVal = op.getOperand(1);
    SDOperand falseVal = op.getOperand(2);

    unsigned opcode = 0;
    switch(trueVal.getValueType()) {
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
        opcode, trueVal.getValueType(), trueVal, falseVal, cond);
}

/**
 * Inline ASM constraint support for custom op definitions.
 */
std::pair<unsigned, const TargetRegisterClass*>
TCETargetLowering::getRegForInlineAsmConstraint(
    const std::string& constraint,
    MVT::ValueType vt) const {

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
