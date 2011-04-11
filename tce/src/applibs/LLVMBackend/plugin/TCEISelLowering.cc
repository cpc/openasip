
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
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#include <assert.h>
#include <string>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/Intrinsics.h>
#include <llvm/CallingConv.h>
#include <llvm/CodeGen/CallingConvLower.h>
#include <llvm/CodeGen/SelectionDAG.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/VectorExtras.h>

#include <llvm/Target/TargetLoweringObjectFile.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCETargetMachine.hh"
#include "TCETargetObjectFile.hh"
#include "TCESubtarget.hh"
#include "Application.hh"
#include "TCEISelLowering.hh"
#include "tce_config.h"
#include "LLVMTCECmdLineOptions.hh"

#include <iostream> // DEBUG

using namespace llvm;

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "TCEGenCallingConv.inc"

static const unsigned ArgRegs[] = {
    TCE::IRES0
};

static const int argRegCount = 1;



SDValue
TCETargetLowering::LowerReturn(SDValue Chain,
                               CallingConv::ID CallConv, bool isVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
#ifndef LLVM_2_7
                                 const SmallVectorImpl<SDValue> &OutVals,
#endif
                               DebugLoc dl, SelectionDAG &DAG) LR_CONST {

  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getTarget(),
                 RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_TCE);

  // If this is the first return lowered for this function, add the regs to the
  // liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), 
#ifdef LLVM_2_7
                             Outs[i].Val, Flag);
#else
                             OutVals[i], Flag);
#endif

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
  }

  if (Flag.getNode())
    return DAG.getNode(TCEISD::RET_FLAG, dl, MVT::Other, Chain, Flag);
  return DAG.getNode(TCEISD::RET_FLAG, dl, MVT::Other, Chain);
}

/**
 * Lowers formal arguments.
 */
SDValue
TCETargetLowering::LowerFormalArguments(
    SDValue Chain,
    CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins,
    DebugLoc dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) LFA_CONST {

    MachineFunction &MF = DAG.getMachineFunction();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                   ArgLocs, *DAG.getContext());

    CCInfo.AnalyzeFormalArguments(Ins, CC_TCE);
  
    const unsigned *CurArgReg = ArgRegs, *ArgRegEnd = ArgRegs + argRegCount;

    unsigned ArgOffset = 0;

    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        SDValue ArgValue;
        CCValAssign &VA = ArgLocs[i];
        // FIXME: We ignore the register assignments of AnalyzeFormalArguments
        // because it doesn't know how to split a double into two i32 registers.
        EVT ObjectVT = VA.getValVT();
        switch (ObjectVT.getSimpleVT().SimpleTy) {
        default: assert(false && "Unhandled argument type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16:
        case MVT::i32: {
            // There may be a bug that marked as not used if varargs
            if (!Ins[i].Used) {
                if (CurArgReg < ArgRegEnd) {
                    ++CurArgReg;
                }
                
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else if (CurArgReg < ArgRegEnd && !isVarArg) {
                unsigned VReg = RegInfo.createVirtualRegister(
                    TCE::I32RegsRegisterClass);
                MF.getRegInfo().addLiveIn(*CurArgReg++, VReg);
                SDValue Arg = DAG.getCopyFromReg(Chain, dl, VReg, MVT::i32);
                if (ObjectVT != MVT::i32) {
                    unsigned AssertOp = ISD::AssertSext;
                    Arg = DAG.getNode(AssertOp, dl, MVT::i32, Arg,
                                      DAG.getValueType(ObjectVT));
                    Arg = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Arg);
                   }
                InVals.push_back(Arg);

            } else {
#ifdef LLVM_2_7
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true, /*isSpillSlot=*/false);
#else
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
#endif		
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                SDValue Load;
                if (ObjectVT == MVT::i32) {
#if defined(LLVM_2_7) || defined(LLVM_2_8)
                    Load = DAG.getLoad(MVT::i32, dl, Chain, FIPtr, NULL, 0,
                                       false, false, 0);
#else // LLVM-29-svn
                    Load = DAG.getLoad(MVT::i32, dl, Chain, FIPtr, 
                                       MachinePointerInfo(), false, false, 0);
#endif
                } else {
                    ISD::LoadExtType LoadOp = ISD::SEXTLOAD;
                    
                    // TCE is big endian, so add an offset based on the ObjectVT.
                    unsigned Offset = 4-std::max(1U, ObjectVT.getSizeInBits()/8);
                    FIPtr = DAG.getNode(ISD::ADD, dl, MVT::i32, FIPtr,
                                        DAG.getConstant(Offset, MVT::i32));
#ifdef LLVM_2_7
                    Load = DAG.getExtLoad(LoadOp, dl, MVT::i32, Chain, FIPtr,
                                          NULL, 0, ObjectVT, false, false, 0);
#else
#ifdef LLVM_2_8
                    Load = DAG.getExtLoad(LoadOp, MVT::i32, dl, Chain, FIPtr,
                                          NULL, 0, ObjectVT, false, false, 0);
#else // LLVM_29_svn
                    Load = DAG.getExtLoad(LoadOp, dl, MVT::i32, Chain, FIPtr,
                                          MachinePointerInfo(), ObjectVT, 
                                          false, false,0);
#endif
#endif
                    Load = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Load);
                }
                InVals.push_back(Load);
            }
            
            ArgOffset += 4;
            break;
        }
            
        case MVT::f32: {
            if (!Ins[i].Used) {                  // Argument is dead.
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
#ifdef LLVM_2_7
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true, /*isSpillSlot=*/false);
#else
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
#endif
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#if defined(LLVM_2_7) || defined(LLVM_2_8)
                SDValue Load = DAG.getLoad(MVT::f32, dl, Chain, FIPtr, NULL, 0,
                             false, false, 0);
#else // LLVM_29-svn
                SDValue Load = DAG.getLoad(MVT::f32, dl, Chain, FIPtr,
                                           MachinePointerInfo(),
                                           false, false, 0);
#endif
                InVals.push_back(Load);
            }
            ArgOffset += 4;
            break;
        }
            
        case MVT::i64:
        case MVT::f64: {            
            if (!Ins[i].Used) {                // Argument is dead.
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
                SDValue HiVal;
#ifdef LLVM_2_7
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true, /*isSpillSlot=*/false);
#else
                int FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
#endif
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#if defined(LLVM_2_7) || defined(LLVM_2_8) 
                HiVal = DAG.getLoad(MVT::i32, dl, Chain, FIPtr, NULL, 0,
                             false, false, 0);
#else // LLVM_29-svn
		HiVal = DAG.getLoad(MVT::i32, dl, Chain, FIPtr, MachinePointerInfo(),
				    false, false, 0);
#endif
                SDValue LoVal;
#ifdef LLVM_2_7
                FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset+4, /*immutable=*/true, /*isSpillSlot=*/false);
#else
                FrameIdx = MF.getFrameInfo()->CreateFixedObject(
                    4, ArgOffset+4, /*immutable=*/true);
#endif
                FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#if defined(LLVM_2_7) || defined(LLVM_2_8) 

                LoVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, NULL, 0, false, false, 0);
#else // LLVM_29-svn
                LoVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo(),
				    false, false, 0);
#endif
                // Compose the two halves together into an i64 unit.
                SDValue WholeValue =
                    DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i64, LoVal, HiVal);
                
                // If we want a double, do a bit convert.
                if (ObjectVT == MVT::f64) {
#if defined(LLVM_2_7) || defined(LLVM_2_8)
                    WholeValue = 
			DAG.getNode(
			    ISD::BIT_CONVERT, dl, MVT::f64, WholeValue);
#else                    
		    WholeValue = 
			DAG.getNode(
			    ISD::BITCAST, dl, MVT::f64, WholeValue);
#endif
		}
                InVals.push_back(WholeValue);
            }
            ArgOffset += 8;
            break;
        }
        }
        
    }
    
    // inspired from ARM
    if (isVarArg) {        
        // This will point to the next argument passed via stack.

#ifdef LLVM_2_7
        VarArgsFrameOffset = MF.getFrameInfo()->CreateFixedObject(
            4, ArgOffset, /*immutable=*/true, /*isSpillSlot=*/false);
#else
        VarArgsFrameOffset = MF.getFrameInfo()->CreateFixedObject(
            4, ArgOffset, /*immutable=*/true);
#endif

    }
    
    return Chain;
}

SDValue
TCETargetLowering::LowerCall(SDValue Chain, SDValue Callee,
                             CallingConv::ID CallConv, bool isVarArg,
                             bool &isTailCall,
                             const SmallVectorImpl<ISD::OutputArg> &Outs,
#ifndef LLVM_2_7
                             const SmallVectorImpl<SDValue> &OutVals,
#endif
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) LC_CONST {


    // we do not yet support tail call optimization.
    isTailCall = false;

    (void)CC_TCE;

    int regParams = 0;

    // Count the size of the outgoing arguments.
    unsigned ArgsSize = 0;
    for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
#ifdef LLVM_2_7
        switch (Outs[i].Val.getValueType().getSimpleVT().SimpleTy) {
#else
#ifdef LLVM_2_8
        switch (Outs[i].VT.getSimpleVT().SimpleTy) {
#else
        switch (Outs[i].VT.SimpleTy) {
#endif
#endif
        default: assert(false && "Unknown value type!");
        case MVT::i1:
        case MVT::i8:
        case MVT::i16:
        case MVT::i32:
            ArgsSize += 4;
            if (regParams < argRegCount) {
                regParams++;
            } 
            break;
        case MVT::f32:
            ArgsSize += 4;
            break;
        case MVT::i64:
        case MVT::f64:
            ArgsSize += 8;
            break;
        }
    }

    // Keep stack frames 4-byte aligned.
    ArgsSize = (ArgsSize+3) & ~3;

    Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(ArgsSize, true));
  
    SmallVector<SDValue, 8> MemOpChains;
   
    SmallVector<std::pair<unsigned, SDValue>, argRegCount> RegsToPass;

    unsigned ArgOffset = 0;

  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
#ifdef LLVM_2_7
      SDValue Val = Outs[i].Val;
#else
    SDValue Val = OutVals[i];
#endif
    EVT ObjectVT = Val.getValueType();
    SDValue ValToStore(0, 0);
    unsigned ObjSize = 0;
    switch (ObjectVT.getSimpleVT().SimpleTy) {
    default: assert(false && "Unhandled argument type!");
        
    case MVT::i1:
    case MVT::i8:
    case MVT::i16: {
        // TODO: is actually needed (sparc did not have this)?
        // Promote the integer to 32-bits.
        //        ISD::NodeType ext = ISD::ANY_EXTEND;
        //        if (Ins[i].isSExt) {
        //            ext = ISD::SIGN_EXTEND;
        //        } else if (Ins[i].isZExt) {
        //            ext = ISD::ZERO_EXTEND;
        //        }
        //        Val = DAG.getNode(ext, dl, MVT::i32, Val);
        // FALL THROUGH
    }
    case MVT::i32:
        ObjSize = 4;
        if (RegsToPass.size() >= argRegCount || isVarArg) {
            ValToStore = Val;
        } 
        if (RegsToPass.size() < argRegCount) {
            RegsToPass.push_back(
                std::make_pair(ArgRegs[RegsToPass.size()], Val));
        }
        break;
    case MVT::f32:
        ObjSize = 4;
        ValToStore = Val;
        break;
    case MVT::f64: 
    case MVT::i64: 
        ObjSize = 8;
        ValToStore = Val;    // Whole thing is passed in memory.
        break;
    }
  
    if (ValToStore.getNode()) {
      SDValue StackPtr = DAG.getRegister(TCE::SP, MVT::i32);
      SDValue PtrOff = DAG.getConstant(ArgOffset, MVT::i32);
      PtrOff = DAG.getNode(ISD::ADD, dl, MVT::i32, StackPtr, PtrOff);
#if defined(LLVM_2_7) || defined(LLVM_2_8)
      MemOpChains.push_back(DAG.getStore(Chain, dl, ValToStore, 
                                         PtrOff, NULL, 0,
                                       false, false, 0));
#else // LLVM_29-svn
      MemOpChains.push_back(DAG.getStore(Chain, dl, ValToStore, 
                                         PtrOff, MachinePointerInfo(),
                                         false, false, 0));
#endif
    }
    ArgOffset += ObjSize;
  }

  // Emit all stores, make sure the occur before any copies into physregs.
  if (!MemOpChains.empty()) {
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &MemOpChains[0], MemOpChains.size());
  }

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emited instructions must be
  // stuck together.
  SDValue InFlag;

  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    unsigned Reg = RegsToPass[i].first;
    Chain = DAG.getCopyToReg(Chain, dl, Reg, RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
#ifdef LLVM_2_7
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), MVT::i32);
#else
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
#endif
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  std::vector<EVT> NodeTys;
  NodeTys.push_back(MVT::Other);   // Returns a chain
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
  NodeTys.push_back(MVT::Flag);    // Returns a flag for retval copy to use.
#else
  NodeTys.push_back(MVT::Glue);    // Returns a flag for retval copy to use.
#endif
  SDValue Ops[] = { Chain, Callee, InFlag };
  Chain = DAG.getNode(TCEISD::CALL, dl, NodeTys, Ops, InFlag.getNode() ? 3 : 2);
  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, true),
                             DAG.getIntPtrConstant(0, true), InFlag);
  InFlag = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RVInfo(CallConv, isVarArg, DAG.getTarget(),
                 RVLocs, *DAG.getContext());

  RVInfo.AnalyzeCallResult(Ins, RetCC_TCE);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
      unsigned Reg = RVLocs[i].getLocReg();
   
    Chain = DAG.getCopyFromReg(Chain, dl, Reg,
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

/**
 * The Constructor.
 *
 * Initializes the target lowering.
 */
TCETargetLowering::TCETargetLowering(
    TargetMachine& TM) :
    TargetLowering(TM,  new TCETargetObjectFile()), tm_(static_cast<TCETargetMachine&>(TM)) {

    LLVMTCECmdLineOptions* opts = dynamic_cast<LLVMTCECmdLineOptions*>(
        Application::cmdLineOptions());

    if (opts != NULL && opts->conservativePreRAScheduler()) {
#ifdef LLVM_2_7
	setSchedulingPreference(SchedulingForRegPressure);
#else
	setSchedulingPreference(llvm::Sched::RegPressure);
#endif
    }

    addRegisterClass(MVT::i1, TCE::I1RegsRegisterClass);
    addRegisterClass(MVT::i32, TCE::I32RegsRegisterClass);
    addRegisterClass(MVT::f32, TCE::F32RegsRegisterClass);

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
    setOperationAction(ISD::TRAP, MVT::Other, Custom);

    // SELECT is used instead of SELECT_CC
    setOperationAction(ISD::SELECT_CC, MVT::Other, Expand);

    // not needed when we uses xor for boolean comparison
//    setOperationAction(ISD::SETCC, MVT::i1, Promote);

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

/*TODO: how are these handled in 2.7?
    setOperationAction(ISD::DBG_STOPPOINT, MVT::Other, Expand);
    setOperationAction(ISD::DEBUG_LOC, MVT::Other, Expand);
    setOperationAction(ISD::DBG_LABEL, MVT::Other, Expand);
*/

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
    // or LLVM built-in emulation pattern to be expanded.
    const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >* 
        missingOps = tm_.missingOperations();
    
    std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >::const_iterator 
        iter = missingOps->begin();

    if (Application::verboseLevel() > 0) {
        Application::logStream() << "Missing ops: ";
    }

    while (iter != missingOps->end()) {
        unsigned nodetype = (*iter).first;
        llvm::MVT::SimpleValueType valuetype = (*iter).second;
        if (Application::verboseLevel() > 0) {
            switch (nodetype) {
            case ISD::SDIV: std::cerr << "SDIV,"; break;
            case ISD::UDIV: std::cerr << "UDIV,"; break;
            case ISD::SREM: std::cerr << "SREM,"; break;
            case ISD::UREM: std::cerr << "UREM,"; break;
            case ISD::ROTL: std::cerr << "ROTL,"; break;
            case ISD::ROTR: std::cerr << "ROTR,"; break;
            case ISD::MUL:  std::cerr << "MUL,"; break;
            case ISD::SIGN_EXTEND_INREG:
                if (valuetype == MVT::i8) std::cerr << "SXQW,";
                if (valuetype == MVT::i16) std::cerr << "SXHW,";
                break;
            default: std::cerr << nodetype << ", "; break;
            };
        }
        setOperationAction(nodetype, valuetype, Expand);
        iter++;
    }

    if (Application::verboseLevel() > 0) {
        std::cerr << std::endl;
    }
    computeRegisterProperties();
}

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
    case TCEISD::Hi: return "TCEISD::Hi";
    case TCEISD::Lo: return "TCEISD::Lo";
    case TCEISD::FTOI: return "TCEISD::FTOI";
    case TCEISD::ITOF: return "TCEISD::ITOF";
    case TCEISD::SELECT_I1: return "TCEISD::SELECT_I1";
    case TCEISD::SELECT_I8: return "TCEISD::SELECT_I8";
    case TCEISD::SELECT_I16: return "TCEISD::SELECT_I16";
    case TCEISD::SELECT_I32: return "TCEISD::SELECT_I32";
    case TCEISD::SELECT_I64: return "TCEISD::SELECT_I64";
    case TCEISD::SELECT_F32: return "TCEISD::SELECT_F32";
    case TCEISD::SELECT_F64: return "TCEISD::SELECT_F64";
    }
}

static SDValue LowerSELECT(
    SDValue op, SelectionDAG& dag) {

    SDValue cond = op.getOperand(0);
    SDValue trueVal = op.getOperand(1);
    SDValue falseVal = op.getOperand(2);

    unsigned opcode = 0;
    switch(trueVal.getValueType().getSimpleVT().SimpleTy) {
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

SDValue TCETargetLowering::LowerTRAP(SDValue Op, SelectionDAG &DAG) const {
    TargetLowering::ArgListTy Args;
    DebugLoc dl = Op->getDebugLoc();
    std::pair<SDValue, SDValue> CallResult =
      LowerCallTo(Op->getOperand(0), Type::getVoidTy(*DAG.getContext()),
                  false, false, false, false, 0, CallingConv::C,
                  /*isTailCall=*/false,
                  /*isReturnValueUsed=*/true,
                  DAG.getExternalSymbol("_exit", getPointerTy()),
                  Args, DAG, dl);
    return CallResult.second;

}

static SDValue LowerGLOBALADDRESS(SDValue Op, SelectionDAG &DAG) {
    const GlobalValue* gv = cast<GlobalAddressSDNode>(Op)->getGlobal();
#ifdef LLVM_2_7
    SDValue ga = DAG.getTargetGlobalAddress(gv, MVT::i32);
#else
  // FIXME there isn't really any debug info here
    DebugLoc dl = Op.getDebugLoc();
    SDValue ga = DAG.getTargetGlobalAddress(gv, dl, MVT::i32);
#endif
    return DAG.getNode(TCEISD::GLOBAL_ADDR, Op.getDebugLoc(), MVT::i32, ga);
}

static SDValue LowerCONSTANTPOOL(SDValue Op, SelectionDAG &DAG) {
    // TODO: Check this.
    llvm::MVT ptrVT = Op.getValueType().getSimpleVT();
    ConstantPoolSDNode* cp = cast<ConstantPoolSDNode>(Op);
    SDValue res;
    if (cp->isMachineConstantPoolEntry()) {
        res = DAG.getTargetConstantPool(
            cp->getMachineCPVal(), ptrVT,
            cp->getAlignment());
    } else {
        res = DAG.getTargetConstantPool(
            cp->getConstVal(), ptrVT,
            cp->getAlignment());
    }
    return DAG.getNode(TCEISD::CONST_POOL, Op.getDebugLoc(), MVT::i32, res);
}

static SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG,
                            const TCETargetLowering &TLI) {

    // ARM ripoff 

    // vastart just stores the address of the VarArgsFrameIndex slot into the
    // memory location argument.
    DebugLoc dl = Op.getDebugLoc();
    EVT PtrVT = DAG.getTargetLoweringInfo().getPointerTy();
    SDValue FR = DAG.getFrameIndex(TLI.getVarArgsFrameOffset(), PtrVT);
    const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
#if defined(LLVM_2_7) || defined(LLVM_2_8)
    return DAG.getStore(
        Op.getOperand(0), dl, FR, Op.getOperand(1), SV, 0, false, false, 0);
#else // LLVM_29-svn
    return DAG.getStore(
        Op.getOperand(0), dl, FR, Op.getOperand(1), MachinePointerInfo(SV), 
	false, false, 0);
#endif
}


/**
 * Returns the preferred comparison result type.
 */
MVT::SimpleValueType 
TCETargetLowering::getSetCCResultType(llvm::EVT VT) const { 
    return llvm::MVT::i1;
}

/**
 * Handles custom operation lowerings.
 */
SDValue
TCETargetLowering::LowerOperation(SDValue op, SelectionDAG& dag) LO_CONST {
    switch(op.getOpcode()) {
    case ISD::TRAP: return LowerTRAP(op, dag);
    case ISD::GlobalAddress: return LowerGLOBALADDRESS(op, dag);
    case ISD::SELECT: return LowerSELECT(op, dag);
    case ISD::VASTART: return LowerVASTART(op, dag, *this);
    case ISD::ConstantPool: return LowerCONSTANTPOOL(op, dag);    
    case ISD::DYNAMIC_STACKALLOC: {
        assert(false && "Dynamic stack allocation not yet implemented.");
    }
    }
    op.getNode()->dump(&dag);
    assert(0 && "Custom lowerings not implemented!");
}


//===----------------------------------------------------------------------===//
//                         Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
TCETargetLowering::ConstraintType
TCETargetLowering::getConstraintType(const std::string &Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:  break;
    case 'r': return C_RegisterClass;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

std::pair<unsigned, const TargetRegisterClass*>
TCETargetLowering::getRegForInlineAsmConstraint(const std::string &Constraint,
                                                  EVT VT) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
        return std::make_pair(0U, TCE::I32RegsRegisterClass);
    case 'f':
        if (VT == MVT::f32) {
            return std::make_pair(0U, TCE::F32RegsRegisterClass);
        } else if (VT == MVT::f64) {
            return std::make_pair(0U, TCE::F64RegsRegisterClass);
        }
    }
  }
  return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}


std::vector<unsigned> TCETargetLowering::
getRegClassForInlineAsmConstraint(const std::string &Constraint,
                                  EVT VT) const {
  if (Constraint.size() != 1)
    return std::vector<unsigned>();

  switch (Constraint[0]) {
  default: break;
  case 'r':
      // TODO: WHAT TO DO WITH THESE?
    return make_vector<unsigned>(/*SP::L0, SP::L1, SP::L2, SP::L3,
                                 SP::L4, SP::L5, SP::L6, SP::L7,
                                 SP::I0, SP::I1, SP::I2, SP::I3,
                                 SP::I4, SP::I5,
                                 SP::O0, SP::O1, SP::O2, SP::O3,
                                 SP::O4, SP::O5, SP::O7,*/ 0);
  }

  return std::vector<unsigned>();
}

bool
TCETargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
  return false;
}

/// getFunctionAlignment - Return the Log2 alignment of this function.
unsigned TCETargetLowering::getFunctionAlignment(const Function *) const {
  return 1;
}
