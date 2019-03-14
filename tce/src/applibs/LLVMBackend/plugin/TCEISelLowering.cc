/*
    Copyright (c) 2002-2013 Tampere University of Technology.

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
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011-2012 (heikki.kultala-no.spam-tut.fi)
 */

#include <assert.h>
#include <string>
#include "tce_config.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/CallingConv.h>
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetLowering.h>
#else
#include <llvm/CodeGen/TargetLowering.h>
#endif
#include <llvm/CodeGen/CallingConvLower.h>
#include <llvm/CodeGen/SelectionDAG.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/Support/raw_ostream.h>

#ifndef LLVM_6_0
#include <llvm/Target/TargetLoweringObjectFile.h>
#else
#include <llvm/CodeGen/TargetLoweringObjectFile.h>
#endif

//#include <llvm/Config/config.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCETargetMachine.hh"
#include "TCETargetObjectFile.hh"
#include "TCESubtarget.hh"
#include "TCEISelLowering.hh"
#include "tce_config.h"
#include "LLVMTCECmdLineOptions.hh"
#include "Application.hh"

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
                               const SmallVectorImpl<SDValue> &OutVals,
                               SDLOC_PARAM_TYPE dl, SelectionDAG &DAG) const 
{

  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
#ifdef LLVM_3_5
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 DAG.getTarget(), RVLocs, *DAG.getContext());
#else
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());
#endif

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_TCE);

  SmallVector<SDValue, 4> RetOps(1, Chain);

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), 
                             OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(
      TCEISD::RET_FLAG, dl, MVT::Other, ArrayRef<SDValue>(RetOps));
}

/**
 * Lowers formal arguments.
 */
SDValue
TCETargetLowering::LowerFormalArguments(
    SDValue Chain,
    CallingConv::ID CallConv, 
    bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins,
    SDLOC_PARAM_TYPE dl, 
    SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const 
{

    MachineFunction &MF = DAG.getMachineFunction();
#if LLVM_OLDER_THAN_4_0
    auto& frameInfo = *MF.getFrameInfo();
#else
    auto& frameInfo = MF.getFrameInfo();
#endif
    MachineRegisterInfo &RegInfo = MF.getRegInfo();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
#ifdef LLVM_3_5
    CCState CCInfo(
        CallConv, isVarArg, DAG.getMachineFunction(),
        getTargetMachine(), ArgLocs, *DAG.getContext());
#else
    CCState CCInfo(
        CallConv, isVarArg, DAG.getMachineFunction(),
        ArgLocs, *DAG.getContext());
#endif
    CCInfo.AnalyzeFormalArguments(Ins, CC_TCE);

    const unsigned *CurArgReg = ArgRegs, *ArgRegEnd = ArgRegs + argRegCount;

    unsigned ArgOffset = 0;

    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        SDValue ArgValue;
        CCValAssign &VA = ArgLocs[i];
        // FIXME: We ignore the register assignments of AnalyzeFormalArguments
        // because it doesn't know how to split a double into two i32 registers.
        EVT ObjectVT = VA.getValVT();
        MVT sType = ObjectVT.getSimpleVT().SimpleTy;

        if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 || 
            sType == MVT::i32) {
            // There may be a bug that marked as not used if varargs
            if (!Ins[i].Used) {
                if (CurArgReg < ArgRegEnd) {
                    ++CurArgReg;
                }
                
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else if (CurArgReg < ArgRegEnd && !isVarArg) {
                unsigned VReg = RegInfo.createVirtualRegister(
                    &TCE::R32IRegsRegClass);
                MF.getRegInfo().addLiveIn(*CurArgReg++, VReg);
                SDValue Arg = DAG.getCopyFromReg(Chain, dl, VReg, MVT::i32);
                if (ObjectVT != MVT::i32) {
                    unsigned AssertOp = ISD::AssertSext;
                    Arg = DAG.getNode(
                        AssertOp, dl, MVT::i32, Arg,
                        DAG.getValueType(ObjectVT));
                    Arg = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Arg);
                }
                InVals.push_back(Arg);

            } else {
                int FrameIdx = frameInfo.CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);

                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
                SDValue Load;
                if (ObjectVT == MVT::i32) {
                    Load = DAG.getLoad(
#ifdef LLVM_OLDER_THAN_3_9
                        MVT::i32, dl, Chain, FIPtr, MachinePointerInfo(), 
                        false, false, false, 0);
#else
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo());
#endif
                } else {
                    ISD::LoadExtType LoadOp = ISD::SEXTLOAD;
                    
                    // TCE is big endian, add an offset based on the ObjectVT.
                    unsigned Offset = 4 - std::max(
                        1U, ObjectVT.getSizeInBits()/8);
#ifdef LLVM_OLDER_THAN_3_7
                    FIPtr = DAG.getNode(
                        ISD::ADD, dl, MVT::i32, FIPtr, 
                        DAG.getConstant(Offset, MVT::i32));
#else
                    FIPtr = DAG.getNode(
                        ISD::ADD, dl, MVT::i32, FIPtr, 
                        DAG.getConstant(Offset, dl, MVT::i32));
#endif
#ifdef LLVM_3_5
                    Load = DAG.getExtLoad(
                        LoadOp, dl, MVT::i32, Chain, FIPtr, 
                        MachinePointerInfo(), ObjectVT, false, false,0);
#elif defined LLVM_OLDER_THAN_3_9
                    Load = DAG.getExtLoad(
                        LoadOp, dl, MVT::i32, Chain, FIPtr, // is invariant.. true?
                        MachinePointerInfo(), ObjectVT, false, false, false,0);
#else
                    Load = DAG.getExtLoad(
                        LoadOp, dl, MVT::i32, Chain, FIPtr,
                        MachinePointerInfo(), ObjectVT);
#endif
                    Load = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Load);
                }
                InVals.push_back(Load);
            }
            
            ArgOffset += 4;
        } else if (sType == MVT::f16) {
            if (!Ins[i].Used) {                  // Argument is dead.
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
                int FrameIdx = frameInfo.CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#ifdef LLVM_OLDER_THAN_3_9
                SDValue Load = DAG.getLoad(
                    MVT::f16, dl, Chain, FIPtr, MachinePointerInfo(),
                    false, false, false, 0);
#else
                SDValue Load = DAG.getLoad(
                    MVT::f16, dl, Chain, FIPtr, MachinePointerInfo());
#endif
                InVals.push_back(Load);
            }
            ArgOffset += 4;
        } else if (sType == MVT::f32) {
            if (!Ins[i].Used) {                  // Argument is dead.
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
                int FrameIdx = frameInfo.CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#ifdef LLVM_OLDER_THAN_3_9
                SDValue Load = DAG.getLoad(
                    MVT::f32, dl, Chain, FIPtr, MachinePointerInfo(),
                    false, false, false, 0);
#else
                SDValue Load = DAG.getLoad(
                    MVT::f32, dl, Chain, FIPtr, MachinePointerInfo());
#endif
                InVals.push_back(Load);
            }
            ArgOffset += 4;
        } else if (sType == MVT::i64 || sType == MVT::f64) {
            if (!Ins[i].Used) {                // Argument is dead.
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
                SDValue HiVal;
                int FrameIdx = frameInfo.CreateFixedObject(
                    4, ArgOffset, /*immutable=*/true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#ifdef LLVM_OLDER_THAN_3_9
                HiVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo(),
                    false, false, false, 0);
#else
                HiVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo());
#endif
                SDValue LoVal;
                FrameIdx = frameInfo.CreateFixedObject(
                    4, ArgOffset+4, /*immutable=*/true);
                FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
#ifdef LLVM_OLDER_THAN_3_9
                LoVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo(),
                    false, false, false, 0);
#else
                LoVal = DAG.getLoad(
                    MVT::i32, dl, Chain, FIPtr, MachinePointerInfo());
#endif
                // Compose the two halves together into an i64 unit.
                SDValue WholeValue =
                    DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i64, LoVal, HiVal);
                
                // If we want a double, do a bit convert.
                if (ObjectVT == MVT::f64) {
                    WholeValue = 
                        DAG.getNode(ISD::BITCAST, dl, MVT::f64, WholeValue);
                }
                InVals.push_back(WholeValue);
            }
            ArgOffset += 8;
        } else {
            std::cerr << "Unhandled argument type: " 
                      << ObjectVT.getEVTString() << std::endl;
            assert(false);
        }    
    }
    
    // inspired from ARM
    if (isVarArg) {
        // This will point to the next argument passed via stack.

        VarArgsFrameOffset = frameInfo.CreateFixedObject(
            4, ArgOffset, /*immutable=*/true);
    }
    
    return Chain;
}


SDValue
TCETargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                             SmallVectorImpl<SDValue> &InVals) const {

    SelectionDAG &DAG                     = CLI.DAG;
    SDLoc &dl                             = CLI.DL;
    SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
    SmallVector<SDValue, 32> &OutVals     = CLI.OutVals;
    SmallVector<ISD::InputArg, 32> &Ins   = CLI.Ins;
    SDValue Chain                         = CLI.Chain;
    SDValue Callee                        = CLI.Callee;
    bool &isTailCall                      = CLI.IsTailCall;
    CallingConv::ID CallConv              = CLI.CallConv;
    bool isVarArg                         = CLI.IsVarArg;

    // we do not yet support tail call optimization.
    isTailCall = false;

    (void)CC_TCE;

    int regParams = 0;

    // Count the size of the outgoing arguments.
    unsigned ArgsSize = 0;
    for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
        EVT ObjectVT = Outs[i].VT;
        MVT sType = Outs[i].VT.SimpleTy;

        if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 || 
            sType == MVT::i32) {
            ArgsSize += 4;
            if (regParams < argRegCount) {
                regParams++;
            } 
        } else if (sType == MVT::f16 || sType == MVT::f32) {
            ArgsSize += 4;
            if (regParams < argRegCount) {
                regParams++;
            } 
        } else if (sType == MVT::i64 || sType == MVT::f64) {
            ArgsSize += 8;
        } else {
            std::cerr << "Unknown argument type: " 
                      << ObjectVT.getEVTString() << std::endl;
            assert(false);
        }
    }

    // Keep stack frames 4-byte aligned.
    ArgsSize = (ArgsSize+3) & ~3;
#ifdef LLVM_OLDER_THAN_3_7
    Chain = 
        DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(ArgsSize, true), dl);
#else
#ifdef LLVM_OLDER_THAN_5_0
    Chain = 
        DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(ArgsSize, dl, true), dl);
#else
    Chain = DAG.getCALLSEQ_START(Chain, ArgsSize, 0, dl);
#endif
#endif
    SmallVector<SDValue, 8> MemOpChains;
   
    SmallVector<std::pair<unsigned, SDValue>, argRegCount> RegsToPass;

    unsigned ArgOffset = 0;

  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    SDValue Val = OutVals[i];
    EVT ObjectVT = Val.getValueType();
    MVT sType = ObjectVT.getSimpleVT().SimpleTy;
    SDValue ValToStore(0, 0);
    unsigned ObjSize = 0;

    if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 || 
        sType == MVT::i32) {
        ObjSize = 4;
        if (RegsToPass.size() >= argRegCount || isVarArg) {
            ValToStore = Val;
        } 
        if (RegsToPass.size() < argRegCount) {
            RegsToPass.push_back(
                std::make_pair(ArgRegs[RegsToPass.size()], Val));
        }
    } else if (sType == MVT::f16 || sType == MVT::f32) {
        ObjSize = 4;
        ValToStore = Val;
    } else if (sType == MVT::i64 || sType == MVT::f64) {
        ObjSize = 8;
        ValToStore = Val;    // Whole thing is passed in memory.
    } else {
        std::cerr << "Unknown argument type: " 
                  << ObjectVT.getEVTString() << std::endl;
        assert(false);
    }

    if (ValToStore.getNode()) {
#ifdef LLVM_OLDER_THAN_3_7
        SDValue StackPtr = DAG.getCopyFromReg(Chain, dl, TCE::SP, getPointerTy());
        SDValue PtrOff = DAG.getConstant(ArgOffset, MVT::i32);
#else
#if defined(LLVM_OLDER_THAN_3_8)
        SDValue StackPtr = DAG.getCopyFromReg(
            Chain, dl, TCE::SP, getPointerTy(
                *getTargetMachine().getDataLayout(), 0));
#else
        SDValue StackPtr = DAG.getCopyFromReg(
            Chain, dl, TCE::SP, getPointerTy(
                getTargetMachine().createDataLayout(), 0));
#endif
      SDValue PtrOff = DAG.getConstant(ArgOffset, dl, MVT::i32);
#endif
      PtrOff = DAG.getNode(ISD::ADD, dl, MVT::i32, StackPtr, PtrOff);
#ifdef LLVM_OLDER_THAN_3_9
      MemOpChains.push_back(DAG.getStore(Chain, dl, ValToStore,
                                         PtrOff, MachinePointerInfo(),
                                         false, false, 0));
#else
      MemOpChains.push_back(DAG.getStore(Chain, dl, ValToStore,
                                         PtrOff, MachinePointerInfo()));
#endif
    }
    ArgOffset += ObjSize;
  }

  // Emit all stores, make sure the occur before any copies into physregs.
  if (!MemOpChains.empty()) {
      Chain = DAG.getNode(
          ISD::TokenFactor, dl, MVT::Other, ArrayRef<SDValue>(MemOpChains));
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
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  std::vector<EVT> NodeTys;
  NodeTys.push_back(MVT::Other);   // Returns a chain
  NodeTys.push_back(MVT::Glue);    // Returns a flag for retval copy to use.
  SDValue Ops[] = { Chain, Callee, InFlag };

  Chain = DAG.getNode(
      TCEISD::CALL, dl, ArrayRef<EVT>(NodeTys), 
      ArrayRef<SDValue>(Ops, InFlag.getNode() ? 3 : 2));

  InFlag = Chain.getValue(1);

#ifdef LLVM_OLDER_THAN_3_7
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, true),
                             DAG.getIntPtrConstant(0, true), InFlag, dl);
#else
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, dl, true),
                             DAG.getIntPtrConstant(0, dl, true), InFlag, dl);
#endif
  InFlag = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
#ifdef LLVM_3_5
  CCState RVInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 DAG.getTarget(), RVLocs, *DAG.getContext());

  RVInfo.AnalyzeCallResult(Ins, RetCC_TCE);
#else
  CCState RVInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());

  RVInfo.AnalyzeCallResult(Ins, RetCC_TCE);
#endif

  // Copy all of the result registers out of their specified physreg. (only one rv reg)
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
#ifdef LLVM_3_5
TCETargetLowering::TCETargetLowering(
    TargetMachine& TM) :
    TargetLowering(TM,  new TCETargetObjectFile()), tm_(static_cast<TCETargetMachine&>(TM)) 
#elif LLVM_OLDER_THAN_3_7
TCETargetLowering::TCETargetLowering(
    TargetMachine& TM) :
    TargetLowering(TM), tm_(static_cast<TCETargetMachine&>(TM)) 
#else
TCETargetLowering::TCETargetLowering(
    TargetMachine& TM, const TCESubtarget &subt) :
    TargetLowering(TM), tm_(static_cast<TCETargetMachine&>(TM)) 
#endif
{
    LLVMTCECmdLineOptions* opts = dynamic_cast<LLVMTCECmdLineOptions*>(
        Application::cmdLineOptions());

    if (opts != NULL && opts->conservativePreRAScheduler()) {
        setSchedulingPreference(llvm::Sched::RegPressure);
    }

    addRegisterClass(MVT::i1, &TCE::R1RegsRegClass);
    addRegisterClass(MVT::i32, &TCE::R32IRegsRegClass);
    addRegisterClass(MVT::f32, &TCE::R32FPRegsRegClass);
    addRegisterClass(MVT::f16, &TCE::R32HFPRegsRegClass);

    if (opts->useVectorBackend()) {
        switch (tm_.maxVectorSize()) {
        default: // more than 8? 
        case 8:
            addRegisterClass(MVT::v8i32, &TCE::V8R32IRegsRegClass);
            addRegisterClass(MVT::v8f32, &TCE::V8R32FPRegsRegClass);
            // TODO: the expanded code is suboptimal for subvectors
            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v8i32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v8i32, Legal);
            setOperationAction(ISD::CONCAT_VECTORS, MVT::v8i32, Legal);
            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v8i32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v8i32, Expand);
            setOperationAction(ISD::SELECT, MVT::v8i32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v8i32, Expand);

            // TODO: the expanded code is suboptimal for subvectors
            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v8f32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v8f32, Legal);
            setOperationAction(ISD::CONCAT_VECTORS, MVT::v8f32, Legal);
            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v8f32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v8f32, Expand);
            setOperationAction(ISD::SELECT, MVT::v8f32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v8f32, Expand);

        case 4:
            addRegisterClass(MVT::v4i32, &TCE::V4R32IRegsRegClass);
            addRegisterClass(MVT::v4f32, &TCE::V4R32FPRegsRegClass);
            // TODO: the expanded code is suboptimal for subvectors
            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v4i32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v4i32, Legal);
            setOperationAction(ISD::CONCAT_VECTORS, MVT::v4i32, Legal);
            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v4i32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v4i32, Expand);
            setOperationAction(ISD::SELECT, MVT::v4i32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v4i32, Expand);
            
            // try to use signext or anyext for ext.
            //	    setLoadExtAction(ISD::EXTLOAD, MVT::v4i8, Promote);

            // TODO: the expanded code is suboptimal for subvectors
            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v4f32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v4f32, Legal);
            setOperationAction(ISD::CONCAT_VECTORS, MVT::v4f32, Legal);
            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v4f32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v4f32, Expand);
            setOperationAction(ISD::SELECT, MVT::v4f32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v4f32, Expand);

        case 2:
            addRegisterClass(MVT::v2i32, &TCE::V2R32IRegsRegClass);
            addRegisterClass(MVT::v2f32, &TCE::V2R32FPRegsRegClass);
            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v2i32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v2i32, Legal);
            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v2i32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v2i32, Expand);
            setOperationAction(ISD::SELECT, MVT::v2i32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v2i32, Expand);

            setOperationAction(ISD::INSERT_SUBVECTOR, MVT::v2f32, Legal);
            setOperationAction(ISD::EXTRACT_SUBVECTOR, MVT::v2f32, Legal);

            setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v2f32, Legal);
            setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v2f32, Expand);
            setOperationAction(ISD::SELECT, MVT::v2f32, Expand);
            setOperationAction(ISD::VSELECT, MVT::v2f32, Expand);

        case 1:
            break;
        }
    }

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
    setOperationAction(ISD::BlockAddress, MVT::i32, Custom);
    setOperationAction(ISD::ConstantPool , MVT::i32, Custom);
    setOperationAction(ISD::TRAP, MVT::Other, Custom);

// TODO: define TCE instruction for leading/trailing zero count
#ifndef LLVM_OLDER_THAN_7
    setOperationAction(ISD::CTLZ, MVT::i32, Expand);
    setOperationAction(ISD::CTTZ, MVT::i32, Expand);
    setOperationAction(ISD::CTPOP, MVT::i32, Expand);
#endif
    // Using 'old way' MVT::Other to cover all value types is illegal now.
    setOperationAction(ISD::SELECT_CC, MVT::f16, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::f64, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::f80, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i1, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i8, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i16, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i64, Expand);

    for (int i = MVT::FIRST_VECTOR_VALUETYPE;
         i <= MVT::LAST_VECTOR_VALUETYPE; ++i) {
        MVT VT = (MVT::SimpleValueType)i;
        setOperationAction(ISD::SELECT_CC, VT, Expand);
    }

    // not needed when we uses xor for boolean comparison
//    setOperationAction(ISD::SETCC, MVT::i1, Promote);

    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

    // Expand indirect branches.
    setOperationAction(ISD::BRIND, MVT::Other, Expand);
    // Expand jumptable branches.
    setOperationAction(ISD::BR_JT, MVT::Other, Expand);
    // Expand conditional branches.
    setOperationAction(ISD::BR_CC, MVT::i1, Expand);
    setOperationAction(ISD::BR_CC, MVT::i32, Expand);
    setOperationAction(ISD::BR_CC, MVT::f16, Expand);
    setOperationAction(ISD::BR_CC, MVT::f32, Expand);
    setOperationAction(ISD::MULHU,  MVT::i32, Expand);
    setOperationAction(ISD::MULHS,  MVT::i32, Expand);
    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

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

    setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::SMUL_LOHI, MVT::i64, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i64, Expand);

    setOperationAction(ISD::BSWAP, MVT::i32, Expand);

    setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
    setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

    setTruncStoreAction(MVT::f32, MVT::f16, Expand);
#ifdef LLVM_OLDER_THAN_3_6
    setLoadExtAction(ISD::EXTLOAD, MVT::f16, Expand);
#else
    setLoadExtAction(ISD::EXTLOAD, MVT::f16, MVT::f32, Expand);
#endif

    setOperationAction(ISD::ADDE, MVT::i32, Expand);
    setOperationAction(ISD::ADDC, MVT::i32, Expand);
    setOperationAction(ISD::ADDE, MVT::i16, Expand);
    setOperationAction(ISD::ADDC, MVT::i16, Expand);
    setOperationAction(ISD::ADDE, MVT::i8, Expand);
    setOperationAction(ISD::ADDC, MVT::i8, Expand);
    

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
#ifdef LLVM_OLDER_THAN_3_7
    computeRegisterProperties();
#else
    computeRegisterProperties(subt.getRegisterInfo());
#endif
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
    case TCEISD::FTOI: return "TCEISD::FTOI";
    case TCEISD::ITOF: return "TCEISD::ITOF";
    case TCEISD::SELECT_I1: return "TCEISD::SELECT_I1";
    case TCEISD::SELECT_I8: return "TCEISD::SELECT_I8";
    case TCEISD::SELECT_I16: return "TCEISD::SELECT_I16";
    case TCEISD::SELECT_I32: return "TCEISD::SELECT_I32";
    case TCEISD::SELECT_I64: return "TCEISD::SELECT_I64";
    case TCEISD::SELECT_F16: return "TCEISD::SELECT_F16";
    case TCEISD::SELECT_F32: return "TCEISD::SELECT_F32";
    case TCEISD::SELECT_F64: return "TCEISD::SELECT_F64";
    }
}

SDValue TCETargetLowering::LowerTRAP(SDValue Op, SelectionDAG &DAG) const { 
    SDLoc dl(Op);

    TargetLowering::ArgListTy Args;

    TargetLowering::CallLoweringInfo CLI(DAG);
    CLI.setDebugLoc(dl);
    CLI.setChain(Op->getOperand(0));
#ifdef LLVM_OLDER_THAN_3_7
    CLI.setCallee(
        CallingConv::C, 
        Type::getVoidTy(*DAG.getContext()),
        DAG.getExternalSymbol("_exit", getPointerTy()),
        std::move(Args),
        0);
#elif defined(LLVM_OLDER_THAN_3_8)
    CLI.setCallee(
        CallingConv::C, 
        Type::getVoidTy(*DAG.getContext()),
        DAG.getExternalSymbol("_exit", 
             getPointerTy(*getTargetMachine().getDataLayout(), 0)),
        std::move(Args),
        0);
#elif defined LLVM_OLDER_THAN_3_9
    CLI.setCallee(
        CallingConv::C, 
        Type::getVoidTy(*DAG.getContext()),
        DAG.getExternalSymbol("_exit", 
             getPointerTy(getTargetMachine().createDataLayout(), 0)),
        std::move(Args),
        0);
#else
    CLI.setCallee(
        CallingConv::C,
        Type::getVoidTy(*DAG.getContext()),
        DAG.getExternalSymbol("_exit",
             getPointerTy(getTargetMachine().createDataLayout(), 0)),
        std::move(Args));
#endif
    CLI.setInRegister(false);
    CLI.setNoReturn(true);
    CLI.setVarArg(false);
    CLI.setTailCall(false);
    CLI.setDiscardResult(false);
    CLI.setSExtResult(false);
    CLI.setZExtResult(false);

    std::pair<SDValue, SDValue> CallResult =
        LowerCallTo(CLI);
    return CallResult.second;

}

// TODO: why is there custom selector for this??
static SDValue LowerGLOBALADDRESS(SDValue Op, SelectionDAG &DAG) {
    const GlobalValue* gv = cast<GlobalAddressSDNode>(Op)->getGlobal();
  // FIXME there isn't really any debug info here
    SDLoc dl(Op);
    SDValue ga = DAG.getTargetGlobalAddress(gv, dl, MVT::i32);
    return DAG.getNode(TCEISD::GLOBAL_ADDR, SDLoc(Op), MVT::i32, ga);
}

SDValue
TCETargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  SDValue BA_SD =  DAG.getTargetBlockAddress(BA, MVT::i32);
  SDLoc dl(Op);
#ifdef LLVM_OLDER_THAN_3_7
  return DAG.getNode(TCEISD::BLOCK_ADDR, dl, getPointerTy(), BA_SD);
#elif defined(LLVM_OLDER_THAN_3_8)
  return DAG.getNode(
      TCEISD::BLOCK_ADDR, dl, 
      getPointerTy(*getTargetMachine().getDataLayout(), 0), BA_SD);
#else
  return DAG.getNode(
      TCEISD::BLOCK_ADDR, dl, 
      getPointerTy(getTargetMachine().createDataLayout(), 0), BA_SD);
#endif
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
    return DAG.getNode(TCEISD::CONST_POOL, SDLoc(Op), MVT::i32, res);
}

SDValue 
TCETargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {

    // ARM ripoff 

    // vastart just stores the address of the VarArgsFrameIndex slot into the
    // memory location argument.
    SDLoc dl(Op);
#ifdef LLVM_OLDER_THAN_3_7
    EVT PtrVT = DAG.getTargetLoweringInfo().getPointerTy();
#elif defined(LLVM_OLDER_THAN_3_8)
    EVT PtrVT = 
        DAG.getTargetLoweringInfo().getPointerTy(
            *getTargetMachine().getDataLayout(), 0);
#else
    EVT PtrVT = 
        DAG.getTargetLoweringInfo().getPointerTy(
            getTargetMachine().createDataLayout(), 0);
#endif
    SDValue FR = DAG.getFrameIndex(getVarArgsFrameOffset(), PtrVT);
    const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
#ifdef LLVM_OLDER_THAN_3_9
    return DAG.getStore(
        Op.getOperand(0), dl, FR, Op.getOperand(1), MachinePointerInfo(SV), 
        false, false, 0);
#else
    return DAG.getStore(
        Op.getOperand(0), dl, FR, Op.getOperand(1), MachinePointerInfo(SV));
#endif
}


/**
 * Returns the preferred comparison result type.
 */
#ifdef LLVM_OLDER_THAN_3_7
EVT
TCETargetLowering::getSetCCResultType(LLVMContext&, llvm::EVT VT) const
#else
llvm::EVT
TCETargetLowering::getSetCCResultType(
    const DataLayout&, 
    LLVMContext &Context,
    EVT VT) const
#endif
{

    if (!VT.isVector()) return llvm::MVT::i1;
    return VT.changeVectorElementTypeToInteger();
}

/**
 * Handles custom operation lowerings.
 */
SDValue
TCETargetLowering::LowerOperation(SDValue op, SelectionDAG& dag) const {
    switch(op.getOpcode()) {
    case ISD::TRAP: return LowerTRAP(op, dag);
    case ISD::GlobalAddress: return LowerGLOBALADDRESS(op, dag);
    case ISD::BlockAddress: return LowerBlockAddress(op, dag);
    case ISD::VASTART: return LowerVASTART(op, dag);
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
#ifdef LLVM_OLDER_THAN_3_7
TCETargetLowering::ConstraintType
TCETargetLowering::getConstraintType(const std::string &Constraint) const {
#else
TCETargetLowering::ConstraintType
TCETargetLowering::getConstraintType(StringRef Constraint) const {
#endif
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:  break;
    case 'r': return C_RegisterClass;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

#ifdef LLVM_OLDER_THAN_3_7
std::pair<unsigned, const TargetRegisterClass*>
TCETargetLowering::getRegForInlineAsmConstraint(const std::string &Constraint,
                                                  MVT VT) const 
#else
std::pair<unsigned, const TargetRegisterClass *>
TCETargetLowering::getRegForInlineAsmConstraint(
    const TargetRegisterInfo* TRI,
    StringRef Constraint, MVT VT) const 

#endif
{
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
        return std::make_pair(0U, &TCE::R32IRegsRegClass);
    case 'f':
        if (VT == MVT::f32) {
            return std::make_pair(0U, &TCE::R32FPRegsRegClass);
        }
    }
  }
#ifdef LLVM_OLDER_THAN_3_7
  return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
#else
  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
#endif
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
      return std::vector<unsigned>(1,0);

  }

  return std::vector<unsigned>();
}

bool
TCETargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
  return false;
}


#if defined(LLVM_3_5)

bool
TCETargetLowering::allowsUnalignedMemoryAccesses(EVT, unsigned, bool*) const {
    /// @todo This commented area and the whole function is probably not
    /// needed anymore. The base class version returns false as default.
    /*
    return (VT==MVT::v2i8 || VT == MVT::v4i8 || VT == MVT::v8i8 ||
	    VT==MVT::v2i16 || VT == MVT::v4i16 || VT == MVT::v8i16);
    */
    return false;
}

#else

bool
TCETargetLowering::allowsMisalignedMemoryAccesses(EVT, unsigned, unsigned, bool*) const {
    /// @todo This commented area and the whole function is probably not
    /// needed anymore. The base class version returns false as default.
    /*
    return (VT==MVT::v2i8 || VT == MVT::v4i8 || VT == MVT::v8i8 ||
	    VT==MVT::v2i16 || VT == MVT::v4i16 || VT == MVT::v8i16);
    */
    return false;
}

#endif
