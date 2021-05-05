/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @author Pekka Jääskeläinen 2010
 * @author Heikki Kultala 2011-2012 (heikki.kultala-no.spam-tut.fi)
 */

#include <assert.h>
#include <string>
#include "tce_config.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/CodeGen/TargetLowering.h>
#include <llvm/CodeGen/CallingConvLower.h>
#include <llvm/CodeGen/SelectionDAG.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/Target/TargetLoweringObjectFile.h>

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
#include "Machine.hh"
#include "AddressSpace.hh"
#include "MachineInfo.hh"

#include "llvm/Support/ErrorHandling.h"

#include <iostream> // DEBUG


#ifdef TARGET64BIT
#define DEFAULT_TYPE MVT::i64
#define DEFAULT_IMM_INSTR TCE::MOVI64sa
#define DEFAULT_SIZE 8
#define DEFAULT_REG_CLASS TCE::R64IRegsRegClass
#else
#define DEFAULT_TYPE MVT::i32
#define DEFAULT_IMM_INSTR TCE::MOVI32ri
#define DEFAULT_SIZE 4
#define DEFAULT_REG_CLASS TCE::R32IRegsRegClass
#endif



using namespace llvm;

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "TCEGenCallingConv.inc"

#include "ArgRegs.hh"

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
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());

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
    auto& frameInfo = MF.getFrameInfo();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(
        CallConv, isVarArg, DAG.getMachineFunction(),
        ArgLocs, *DAG.getContext());

    CCInfo.AnalyzeFormalArguments(Ins, CC_TCE);

    const unsigned *CurArgReg = ArgRegs, *ArgRegEnd = ArgRegs + argRegCount;
    const unsigned maxMemAlignment = isVarArg ? 4 : tm_.stackAlignment();
    unsigned ArgOffset = 0;

    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        SDValue ArgValue;
        CCValAssign &VA = ArgLocs[i];
        // FIXME: We ignore the register assignments of AnalyzeFormalArguments
        // because it doesn't know how to split a double into two i32 registers.
        EVT ObjectVT = VA.getValVT();
        MVT sType = ObjectVT.getSimpleVT().SimpleTy;

        if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 ||
#ifdef TARGET64BIT
            sType == MVT::i64 ||
#endif
            sType == MVT::i32) {
            // There may be a bug that marked as not used if varargs
            if (!Ins[i].Used) {
                if (CurArgReg < ArgRegEnd) {
                    ++CurArgReg;
                }
                
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else if (CurArgReg < ArgRegEnd && !isVarArg) {
                unsigned VReg = RegInfo.createVirtualRegister(
                    &DEFAULT_REG_CLASS);
                MF.getRegInfo().addLiveIn(*CurArgReg++, VReg);
                SDValue Arg = DAG.getCopyFromReg(Chain, dl, VReg, DEFAULT_TYPE);
                if (ObjectVT != DEFAULT_TYPE) {
                    unsigned AssertOp = ISD::AssertSext;
                    Arg = DAG.getNode(
                        AssertOp, dl, DEFAULT_TYPE, Arg,
                        DAG.getValueType(ObjectVT));
                    Arg = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Arg);
                }
                InVals.push_back(Arg);

            } else {
                int FrameIdx = frameInfo.CreateFixedObject(
                    DEFAULT_SIZE, ArgOffset, /*immutable=*/true);

                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, DEFAULT_TYPE);
                SDValue Load;
                if (ObjectVT == DEFAULT_TYPE) {
                    Load = DAG.getLoad(
                    DEFAULT_TYPE, dl, Chain, FIPtr, MachinePointerInfo());
                } else {
                    ISD::LoadExtType LoadOp = ISD::SEXTLOAD;

#ifndef LITTLE_ENDIAN_TARGET // big endian extload hack starts

                    // TODO: WHAT IS THIS??
                    // TCE IS NO LONGER ALWAYS BIG-ENDIAN!
                    // TCE is big endian, add an offset based on the ObjectVT.
                    unsigned Offset = DEFAULT_SIZE - std::max(
                        1UL, ObjectVT.getSizeInBits().getFixedSize()/8);
                    FIPtr = DAG.getNode(
                        ISD::ADD, dl, DEFAULT_TYPE, FIPtr,
                        DAG.getConstant(Offset, dl, DEFAULT_TYPE));

#endif // big endian hack ends

                    Load = DAG.getExtLoad(
                        LoadOp, dl, DEFAULT_TYPE, Chain, FIPtr,
                        MachinePointerInfo(), ObjectVT);
                    Load = DAG.getNode(ISD::TRUNCATE, dl, ObjectVT, Load);
                }
                InVals.push_back(Load);
            }
        } else if (sType == MVT::f16) {
            if (!Ins[i].Used) {                  // Argument is dead.
                if (CurArgReg < ArgRegEnd) {
                    ++CurArgReg;
                }
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else if (CurArgReg < ArgRegEnd && !isVarArg) {
                unsigned VReg = RegInfo.createVirtualRegister(
                    &TCE::HFPRegsRegClass);
                MF.getRegInfo().addLiveIn(*CurArgReg++, VReg);
                SDValue Arg = DAG.getCopyFromReg(Chain, dl, VReg, MVT::f16);
                InVals.push_back(Arg);
            } else {
                int FrameIdx = frameInfo.CreateFixedObject(
                    DEFAULT_SIZE, ArgOffset, /*immutable=*/true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, DEFAULT_TYPE);
                SDValue Load = DAG.getLoad(
                    MVT::f16, dl, Chain, FIPtr, MachinePointerInfo());
                InVals.push_back(Load);
            }
        } else if (sType == MVT::f32 || sType == MVT::f64) {
            if (!Ins[i].Used) {                  // Argument is dead.
                if (CurArgReg < ArgRegEnd) {
                    ++CurArgReg;
                }
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else if (CurArgReg < ArgRegEnd && !isVarArg) { // reg argument
                auto regClass = sType == MVT::f32 ?
                    &TCE::FPRegsRegClass:
                    &TCE::R64DFPRegsRegClass;
                unsigned VReg = RegInfo.createVirtualRegister(regClass);
                MF.getRegInfo().addLiveIn(*CurArgReg++, VReg);
                SDValue Arg = DAG.getCopyFromReg(Chain, dl, VReg, sType);
                InVals.push_back(Arg);
            } else { // argument in stack.
                int FrameIdx = frameInfo.CreateFixedObject(
                    DEFAULT_SIZE, ArgOffset, /*immutable=*/true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, DEFAULT_TYPE);
                SDValue Load = DAG.getLoad(
                    sType, dl, Chain, FIPtr, MachinePointerInfo());
                InVals.push_back(Load);
            }
        } else if (sType.isVector()) {
            if (!Ins[i].Used) {
                InVals.push_back(DAG.getUNDEF(ObjectVT));
            } else {
                int FrameIdx = MF.getFrameInfo().CreateFixedObject(
                    sType.getStoreSize(), ArgOffset, true);
                SDValue FIPtr = DAG.getFrameIndex(FrameIdx, DEFAULT_TYPE);
                SDValue Load = DAG.getLoad(
                    sType, dl, Chain, FIPtr, MachinePointerInfo());
                InVals.push_back(Load);
            }
        } else {
            std::cerr << "Unhandled argument type: " 
                      << ObjectVT.getEVTString() << std::endl;
            std::cerr << "sType size in bits: " << sType.getSizeInBits()  << std::endl;
            std::cerr << "is a vector? " << sType.isVector() << std::endl;
            assert(false);
        }

        unsigned argumentByteSize = sType.getStoreSize();

        // Align parameter to stack correctly.
        if (argumentByteSize <= maxMemAlignment) {
            ArgOffset += maxMemAlignment;
        } else {
            unsigned alignBytes = maxMemAlignment - 1;
            ArgOffset += (argumentByteSize + alignBytes) & (~alignBytes);
        }
    }
    
    // inspired from ARM
    if (isVarArg) {
        /// @todo This probably doesn't work with vector arguments currently.
        // This will point to the next argument passed via stack.

        VarArgsFrameOffset = frameInfo.CreateFixedObject(
            DEFAULT_SIZE, ArgOffset, /*immutable=*/true);
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

    const unsigned maxMemAlignment = isVarArg? 4 : tm_.stackAlignment();
    int regParams = 0;
    unsigned ArgsSize = 0;

    // Count the size of the outgoing arguments.
    for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
        EVT ObjectVT = Outs[i].VT;
        MVT sType = Outs[i].VT.SimpleTy;
#ifndef TARGET64BIT
        if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 ||
            sType == MVT::i32 || sType == MVT::f16 || sType == MVT::f32) {
            if (regParams < argRegCount) {
                regParams++;
            } 
        } else if (sType == MVT::i64 || sType == MVT::f64) {
            // Nothing to do.
        } else if (sType.isVector()) {
            // Nothing to do.
        } else {
            std::cerr << "Unknown argument type: "
                      << ObjectVT.getEVTString() << std::endl;
            assert(false);
        }
#else
        if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 ||
            sType == MVT::i32 || sType == MVT::i64 || sType == MVT::f16 ||
            sType == MVT::f32 || sType == MVT::f64) {
            if (regParams < argRegCount) {
                regParams++;
            }
        } else if (sType.isVector()) {
            // Nothing to do.
        } else {
            std::cerr << "Unknown argument type: " 
                      << ObjectVT.getEVTString() << std::endl;
            assert(false);
        }
#endif

        unsigned argumentByteSize = sType.getStoreSize();

        // Align parameter to stack correctly.
        if (argumentByteSize <= maxMemAlignment) {
            ArgsSize += maxMemAlignment;
        } else {
            unsigned alignBytes = maxMemAlignment - 1;
            ArgsSize += (argumentByteSize + alignBytes) & (~alignBytes);
        }
    }
    Chain = DAG.getCALLSEQ_START(Chain, ArgsSize, 0, dl);
    SmallVector<SDValue, 8> MemOpChains;
   
    SmallVector<std::pair<unsigned, SDValue>, argRegCount> RegsToPass;

    unsigned ArgOffset = 0;

  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    SDValue Val = OutVals[i];
    EVT ObjectVT = Val.getValueType();
    MVT sType = ObjectVT.getSimpleVT().SimpleTy;
    SDValue ValToStore(0, 0);

#ifndef TARGET64BIT
    if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 ||
        sType == MVT::i32 || sType == MVT::f32 || sType == MVT::f16) {
        if (RegsToPass.size() >= argRegCount || isVarArg) {
            ValToStore = Val;
        }
        if (RegsToPass.size() < argRegCount) {
            RegsToPass.push_back(
                std::make_pair(ArgRegs[RegsToPass.size()], Val));
        }
    } else if (sType.isVector()) {
        ValToStore = Val;
    } else {
        std::cerr << "Unknown argument type: " 
                  << ObjectVT.getEVTString() << std::endl;
        assert(false);
    }
#else // is 64-bit
    if (sType == MVT::i1 || sType == MVT::i8 || sType == MVT::i16 ||
        sType == MVT::i32 || sType == MVT::i64 || sType == MVT::f32 ||
        sType == MVT::f64) {
        if (RegsToPass.size() >= argRegCount || isVarArg) {
            ValToStore = Val;
        }
        if (RegsToPass.size() < argRegCount) {
            RegsToPass.push_back(
                std::make_pair(ArgRegs[RegsToPass.size()], Val));
        }
    } else if (sType.isVector()) {
        ValToStore = Val;
    } else {
        std::cerr << "Unknown argument type: "
                      << ObjectVT.getEVTString() << std::endl;
        assert(false);
    }
#endif

    if (ValToStore.getNode()) {
        SDValue StackPtr = DAG.getCopyFromReg(
            Chain, dl, TCE::SP, getPointerTy(
                getTargetMachine().createDataLayout(), 0));
      SDValue PtrOff = DAG.getConstant(ArgOffset, dl, DEFAULT_TYPE);
      PtrOff = DAG.getNode(ISD::ADD, dl, DEFAULT_TYPE, StackPtr, PtrOff);

      MemOpChains.push_back(DAG.getStore(Chain, dl, ValToStore,
                                         PtrOff, MachinePointerInfo()));
    }

    unsigned argumentByteSize = sType.getStoreSize();

    // Align parameter to stack correctly.
    if (argumentByteSize <= maxMemAlignment) {
        ArgOffset += maxMemAlignment;
    } else {
        unsigned alignBytes = maxMemAlignment - 1;
        ArgOffset += (argumentByteSize + alignBytes) & (~alignBytes);
    }
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
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, DEFAULT_TYPE);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), DEFAULT_TYPE);
  std::vector<EVT> NodeTys;
  NodeTys.push_back(MVT::Other);   // Returns a chain
  NodeTys.push_back(MVT::Glue);    // Returns a flag for retval copy to use.
  SDValue Ops[] = { Chain, Callee, InFlag };

  Chain = DAG.getNode(
      TCEISD::CALL, dl, ArrayRef<EVT>(NodeTys), 
      ArrayRef<SDValue>(Ops, InFlag.getNode() ? 3 : 2));

  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, dl, true),
                             DAG.getIntPtrConstant(0, dl, true), InFlag, dl);
  InFlag = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RVInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 RVLocs, *DAG.getContext());

  RVInfo.AnalyzeCallResult(Ins, RetCC_TCE);

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
TCETargetLowering::TCETargetLowering(
    TargetMachine &TM, const TCESubtarget &subt)
    : TargetLowering(TM), tm_(static_cast<TCETargetMachine &>(TM)) {
    LLVMTCECmdLineOptions* opts = dynamic_cast<LLVMTCECmdLineOptions*>(
        Application::cmdLineOptions());

    if (opts != NULL && opts->conservativePreRAScheduler()) {
        setSchedulingPreference(llvm::Sched::RegPressure);
    }

    hasI1RC_ = hasI1RegisterClass();
    if (hasI1RC_)
        addRegisterClass(MVT::i1, &TCE::R1RegsRegClass);

#ifdef TARGET64BIT
    addRegisterClass(MVT::i64, &TCE::R64IRegsRegClass);
    addRegisterClass(MVT::f64, &TCE::R64DFPRegsRegClass);
#else
    addRegisterClass(MVT::i32, &TCE::R32IRegsRegClass);
#endif
    addRegisterClass(MVT::f32, &TCE::FPRegsRegClass);
    addRegisterClass(MVT::f16, &TCE::HFPRegsRegClass);

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

    setOperationAction(ISD::GlobalAddress, DEFAULT_TYPE, Custom);
    setOperationAction(ISD::BlockAddress, DEFAULT_TYPE, Custom);
    setOperationAction(ISD::ConstantPool , DEFAULT_TYPE, Custom);

    setOperationAction(ISD::TRAP, MVT::Other, Custom);

// TODO: define TCE instruction for leading/trailing zero count
    setOperationAction(ISD::CTLZ, DEFAULT_TYPE, Expand);
    setOperationAction(ISD::CTTZ, DEFAULT_TYPE, Expand);
    setOperationAction(ISD::CTPOP, DEFAULT_TYPE, Expand);
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


    // not needed when we uses xor for boolean comparison
//    setOperationAction(ISD::SETCC, MVT::i1, Promote);

    setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

    // Expand indirect branches.
    setOperationAction(ISD::BRIND, MVT::Other, Expand);
    // Expand jumptable branches.
    setOperationAction(ISD::BR_JT, MVT::Other, Expand);
    // Expand conditional branches.

    // only port-guarded jumps..
    if (!MachineInfo::supportsBoolRegisterGuardedJumps(tm_.ttaMachine())
        && MachineInfo::supportsPortGuardedJumps(tm_.ttaMachine())) {
        std::cerr << "Only port guarded jumps supported, not expanding bc_cc" << std::endl;

        setOperationAction(ISD::BRCOND, MVT::Other, Expand);
        setOperationAction(ISD::BRCOND, MVT::i1, Expand);
        setOperationAction(ISD::BRCOND, MVT::i32, Expand);
        setOperationAction(ISD::BRCOND, MVT::f16, Expand);
        setOperationAction(ISD::BRCOND, MVT::f32, Expand);
        setOperationAction(ISD::BRCOND, MVT::i64, Expand);
    } else {
        setOperationAction(ISD::BR_CC, MVT::Other, Expand);
        setOperationAction(ISD::BR_CC, MVT::i1, Expand);
        setOperationAction(ISD::BR_CC, MVT::i32, Expand);
        setOperationAction(ISD::BR_CC, MVT::f16, Expand);
        setOperationAction(ISD::BR_CC, MVT::f32, Expand);
        setOperationAction(ISD::BR_CC, MVT::i64, Expand);
    }

#ifdef TARGET64BIT
    setOperationAction(ISD::BR_CC, MVT::f64, Expand);
#endif
    setOperationAction(ISD::MULHU,  MVT::i32, Expand);
    setOperationAction(ISD::MULHS,  MVT::i32, Expand);

#ifdef TARGET64BIT
    setOperationAction(ISD::MULHU,  MVT::i64, Expand);
    setOperationAction(ISD::MULHS,  MVT::i64, Expand);
#endif
    setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
    setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

    setOperationAction(ISD::VASTART           , MVT::Other, Custom);

    setOperationAction(ISD::VAARG             , MVT::Other, Expand);
    setOperationAction(ISD::VACOPY            , MVT::Other, Expand);
    setOperationAction(ISD::VAEND             , MVT::Other, Expand);
    setOperationAction(ISD::STACKSAVE         , MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE      , MVT::Other, Expand);

    setOperationAction(ISD::DYNAMIC_STACKALLOC, DEFAULT_TYPE, Expand);

    setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
    setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);

    setOperationAction(ISD::ConstantFP, MVT::f64, Expand);

    setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
    setOperationAction(ISD::SMUL_LOHI, MVT::i64, Expand);
    setOperationAction(ISD::UMUL_LOHI, MVT::i64, Expand);

    setOperationAction(ISD::BSWAP, DEFAULT_TYPE, Expand);

    setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
    setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

#ifdef TARGET64BIT
    setOperationAction(ISD::SDIVREM, MVT::i64, Expand);
    setOperationAction(ISD::UDIVREM, MVT::i64, Expand);
    setTruncStoreAction(MVT::f64, MVT::f32, Expand);
#endif

    setTruncStoreAction(MVT::f32, MVT::f16, Expand);
    // 3.7 requires the types as target type second parameter,
    // mem type thid parameter
    setLoadExtAction(ISD::EXTLOAD, MVT::f32, MVT::f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v2f32, MVT::v2f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v4f32, MVT::v4f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v8f32, MVT::v8f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v16f32, MVT::v16f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v32f32, MVT::v32f16, Expand);

#ifdef TARGET64BIT
    setLoadExtAction(ISD::EXTLOAD, MVT::f64, MVT::f32, Expand);
#endif

#if LLVM_HAS_CUSTOM_VECTOR_EXTENSION == 2
    setLoadExtAction(ISD::EXTLOAD, MVT::v64f32, MVT::v64f16, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v128f32, MVT::v128f16, Expand);
#endif

    if (!tm_.has8bitLoads()) {
        if (Application::verboseLevel() > 0) {
            std::cout << "No 8-bit loads in the processor. "
                      << "Emulating 8-bit loads with wider loads. "
                      << "This may be very slow if the program performs "
                      << "lots of 8-bit loads." << std::endl;
        }

#ifdef TARGET64BIT
        setLoadExtAction(ISD::EXTLOAD, MVT::i64, MVT::i8, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MVT::i8, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MVT::i8, Custom);
        setOperationAction(ISD::LOAD, MVT::i8, Custom);
        setOperationAction(ISD::LOAD, MVT::i1, Custom);

        setLoadExtAction(ISD::EXTLOAD, MVT::i64, MVT::i1, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MVT::i1, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MVT::i1, Custom);
#else
        setLoadExtAction(ISD::EXTLOAD, MVT::i32, MVT::i8, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i32, MVT::i8, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i32, MVT::i8, Custom);
        setOperationAction(ISD::LOAD, MVT::i8, Custom);
        setOperationAction(ISD::LOAD, MVT::i1, Custom);

        setLoadExtAction(ISD::EXTLOAD, MVT::i32, MVT::i1, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i32, MVT::i1, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i32, MVT::i1, Custom);
#endif
    }

    if (!tm_.has16bitLoads()) {
        if (Application::verboseLevel() > 0) {
            std::cout << "No 16-bit loads in the processor. "
                      << "Emulating 16-bit loads with wider loads. "
                      << "This may be very slow if the program performs "
                      << "lots of 16-bit loads." << std::endl;
        }
#ifdef TARGET64BIT
        setLoadExtAction(ISD::EXTLOAD, MVT::i64, MVT::i16, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MVT::i16, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MVT::i16, Custom);
        setOperationAction(ISD::LOAD, MVT::i16, Custom);
#else
        setLoadExtAction(ISD::EXTLOAD, MVT::i32, MVT::i16, Custom);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i32, MVT::i16, Custom);
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i32, MVT::i16, Custom);
        setOperationAction(ISD::LOAD, MVT::i16, Custom);
#endif
    }

    setOperationAction(ISD::ADDE, MVT::i32, Expand);
    setOperationAction(ISD::ADDC, MVT::i32, Expand);
    setOperationAction(ISD::ADDE, MVT::i16, Expand);
    setOperationAction(ISD::ADDC, MVT::i16, Expand);
    setOperationAction(ISD::ADDE, MVT::i8, Expand);
    setOperationAction(ISD::ADDC, MVT::i8, Expand);
#ifdef TARGET64BIT
    setOperationAction(ISD::Constant, MVT::i64, Custom);
#else
    setOperationAction(ISD::Constant, MVT::i32, Custom);
#endif

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

    const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >* 
        promotedOps = tm_.promotedOperations();

    iter = promotedOps->begin();
    while (iter != promotedOps->end()) {
        unsigned nodetype = (*iter).first;
        llvm::MVT::SimpleValueType valuetype = (*iter).second;
        llvm::EVT evt(valuetype);
        setOperationAction(nodetype, valuetype, Promote);
        iter++;
    }

    if (Application::verboseLevel() > 0) {
        std::cerr << std::endl;
    }

    auto customLegalizedOps = tm_.customLegalizedOperations();
    for (auto i : *customLegalizedOps) {
        unsigned nodetype = i.first;
        llvm::MVT::SimpleValueType valuetype = i.second;
        llvm::EVT evt(valuetype);
        setOperationAction(nodetype, valuetype, Custom);
    }

    setJumpIsExpensive(true);

    //setShouldFoldAtomicFences(true);

    PredictableSelectIsExpensive = false;

    // Determine which of global addresses by address space id should be //
    // loaded from constant pool due to limited immediate support.       //
    // Reverse for default address space.
    loadGAFromConstantPool_[0] = false;
    for (const auto& as : tm_.ttaMachine().addressSpaceNavigator()) {
        if (as->numericalIds().empty()) {
            // No IDs specified, assume default address space ID (0)
            if (as->end() > tm_.largestImmValue()) {
                if (Application::verboseLevel() > 0) {
                    std::cerr << "Global addresses by "
                              << "address space id of 0"
                              << " (implicitly specified by AS: " << as->name()
                              << ") will be stored in constant pool."
                              << std::endl;
                }
                loadGAFromConstantPool_[0] = true;
            } else {
                loadGAFromConstantPool_[0] |= false;
            }
            continue;
        }

        for (unsigned id : as->numericalIds()) {
            if (as->end() > tm_.largestImmValue()) {
                if (Application::verboseLevel() > 0) {
                    std::cerr << "Global addresses belonging to "
                              << "address space id of " << id
                              << " (specified by AS: " << as->name()
                              << ") will be stored in constant pool."
                              << std::endl;
                }
                loadGAFromConstantPool_[id] = true;
            } else {
                loadGAFromConstantPool_[id] |= false;
            }
        }
    }

    setBooleanContents(ZeroOrOneBooleanContent);
    setBooleanVectorContents(ZeroOrNegativeOneBooleanContent);

    addVectorRegisterClasses();
    addVectorLowerings();
    computeRegisterProperties(subt.getRegisterInfo());
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
    CLI.setCallee(
        CallingConv::C,
        Type::getVoidTy(*DAG.getContext()),
        DAG.getExternalSymbol("_exit",
             getPointerTy(getTargetMachine().createDataLayout(), 0)),
        std::move(Args));
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


SDValue
TCETargetLowering::LowerGLOBALADDRESS(SDValue Op, SelectionDAG &DAG) const {
    const GlobalAddressSDNode* gn = cast<GlobalAddressSDNode>(Op);
    const GlobalValue* gv = gn->getGlobal();
    // FIXME there isn't really any debug info here
    SDLoc dl(Op);

#if 0
    std::cerr << "lowering GA: AS = " << gn->getAddressSpace() << ", ";
    gv->getValueType()->dump();
#endif

    if (shouldLoadFromConstantPool(gn->getAddressSpace())) {
        // Immediate support for the address space is limited. Therefore,
        // the address must be loaded from constant pool.
        auto vt = getPointerTy(DAG.getDataLayout(), gn->getAddressSpace());
        SDValue cpIdx = DAG.getConstantPool(
            gv, getPointerTy(DAG.getDataLayout()));
	llvm::Align Alignment = cast<ConstantPoolSDNode>(cpIdx)->getAlign();
        SDValue result = DAG.getLoad(vt, dl, DAG.getEntryNode(), cpIdx,
            MachinePointerInfo::getConstantPool(DAG.getMachineFunction())
            );

        if (Application::verboseLevel() > 0) {
            std::cerr << "Expanded Global Value to a load from "
                      << "the constant pool." << std::endl;
        }
        return result;
    }
    SDValue tga = DAG.getTargetGlobalAddress(gv, dl, DEFAULT_TYPE);
    return DAG.getNode(TCEISD::GLOBAL_ADDR, SDLoc(Op), DEFAULT_TYPE, tga);
}

SDValue
TCETargetLowering::LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const {
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();

  SDValue BA_SD =  DAG.getTargetBlockAddress(BA, DEFAULT_TYPE);
  SDLoc dl(Op);
  return DAG.getNode(
      TCEISD::BLOCK_ADDR, dl, 
      getPointerTy(getTargetMachine().createDataLayout(), 0), BA_SD);
}

static SDValue LowerCONSTANTPOOL(SDValue Op, SelectionDAG &DAG) {
    // TODO: Check this.
    llvm::MVT ptrVT = Op.getValueType().getSimpleVT();
    ConstantPoolSDNode* cp = cast<ConstantPoolSDNode>(Op);
    SDValue res;
    if (cp->isMachineConstantPoolEntry()) {
        res = DAG.getTargetConstantPool(
            cp->getMachineCPVal(), ptrVT,
            cp->getAlign());
    } else {
        res = DAG.getTargetConstantPool(
            cp->getConstVal(), ptrVT,
            cp->getAlign());

    }
    return DAG.getNode(TCEISD::CONST_POOL, SDLoc(Op), DEFAULT_TYPE, res);
}

SDValue
TCETargetLowering::LowerConstant(SDValue Op, SelectionDAG &DAG) const {
    ConstantSDNode* cn = cast<ConstantSDNode>(Op.getNode());
    assert(cn);

    if (canEncodeImmediate(*cn)) {
        return Op;
    } else {
        // The constant is not supported immediate, return empty SDValue, so
        // it gets converted to a load from a constant pool.
        if (Application::verboseLevel() > 0) {
            std::cerr << "Expand constant of " << cn->getSExtValue();
            std::cerr << " to a load from the constant pool." << std::endl;
        }
        // Since LLVM 3.8 LLVM's DAG Legalization does the expansion from 
        // constant to constant pool load.
        return SDValue(nullptr, 0);
    }
}

SDValue TCETargetLowering::LowerBuildBooleanVectorVector(
SDValue Op, MVT newElementVT, int elemCount, SelectionDAG &DAG) const {

    BuildVectorSDNode* node = cast<BuildVectorSDNode>(Op);
    MVT mvt = Op.getSimpleValueType();
    int laneWidth = newElementVT.getSizeInBits();

    std::vector<SDValue> packedConstants(elemCount/laneWidth);
    for (int i = 0; i < elemCount; i+=laneWidth) {
        unsigned int packedVal = 0;
        for (int j = 0; j < laneWidth; j++) {
            const SDValue& operand = node->getOperand(i+j);
            SDNode* opdNode = operand.getNode();
            if (isa<ConstantSDNode>(opdNode)) {
                ConstantSDNode* cn = cast<ConstantSDNode>(opdNode);
                if (cn->isOne()) {
                    packedVal += (1<< j);
                }
            }
        }
        packedConstants[i/laneWidth] = DAG.getConstant(packedVal, Op, newElementVT);
    }
    EVT wvt = EVT::getVectorVT(*DAG.getContext(), newElementVT, elemCount/laneWidth);
    SDValue intVectorBuild = DAG.getNode(ISD::BUILD_VECTOR, Op, wvt, packedConstants);
    SDValue retValue = DAG.getNode(ISD::BITCAST, Op, mvt, intVectorBuild);
    return retValue;
}

SDValue
TCETargetLowering::LowerBuildVector(SDValue Op, SelectionDAG &DAG) const {

    MVT elemVT = Op.getSimpleValueType().getScalarType();
    BuildVectorSDNode* node = cast<BuildVectorSDNode>(Op);
    int elemCount = node->getNumOperands();

    if (isConstantOrUndefBuild(*node)) {
        if (!isBroadcast(node)) {
            // Convert boolean vector into wider vector.
            // Use int here.

            auto vt = Op.getValueType();
            bool scalarizedPack = false;
            if (vt.isVector() && vt.getSizeInBits() == 32) {
                unsigned int packedVal = 0;
                unsigned int laneW = vt.getScalarSizeInBits();
#ifdef LLVM_OLDER_THAN_12
                for (int i = 0; i < vt.getVectorElementCount().Min; i++) {
#else
                for (int i = 0;
                     i < vt.getVectorElementCount().getKnownMinValue(); i++) {
#endif
                    auto oprd = node->getOperand(i);
                    ConstantSDNode* cn = cast<ConstantSDNode>(oprd);
                    unsigned int val = cn->getZExtValue();
                    val = val & (~0u >> (32 - laneW));
                    packedVal |= (val << (laneW*i));
                }
                if (tm_.canEncodeAsMOVI(MVT::i32, packedVal)) {
                    auto packedNode =
                        DAG.getConstant(packedVal, Op, MVT::i32);
                    return DAG.getNode(ISD::BITCAST, Op, vt, packedNode);
                }
            }

            if (elemVT == MVT::i1) {
                if (elemCount > 31) {
                    assert(elemCount % 32 == 0);
                    int intElemCount = elemCount/32;
                    TCEString wideOpName = "PACK32X"; wideOpName << intElemCount;
                    if (tm_.hasOperation(wideOpName)) {
                        return LowerBuildBooleanVectorVector(
                            Op, MVT::i32, elemCount, DAG);
                    }
                }
/* TODO: this does not work if u16 and i8 value types not legal.
                if (elemCount > 15 && elemCount < 4096) {
                    assert(elemCount % 16 == 0);
                    int shortElemCount = elemCount/16;
                    TCEString wideOpName = "PACK16X"; wideOpName << shortElemCount;
                    if (tm_.hasOperation(wideOpName)) {
                        return LowerBuildBooleanVectorVector(
                        Op, MVT::i16, elemCount, DAG);
                    }
                }
                if (elemCount > 7 && elemCount < 2048) {
                    assert(elemCount % 8 == 0);
                    int charElemCount = elemCount/8;
                    TCEString wideOpName = "PACK8X"; wideOpName << charElemCount;
                    if (tm_.hasOperation(wideOpName)) {
                        return LowerBuildBooleanVectorVector(
                        Op, MVT::i8, elemCount, DAG);
                    }
                }
*/
                if (elemCount > 255) {
                    std::cerr << "Warning: Lowering Boolean vector build with"
                              << " more than 255 elements. LLVM does not"
                              << " support instructions with more than"
                              << " 255 operands so this will probably fail."
                              << " Add a pack instruction using wider lane"
                              << " width, such as PACK32X" << (elemCount/32)
                              << " into your architecture."
                              << std::endl;
                }
            } else { // not boolean.
                // makes no sense to have zillion inputs to build_vector.
                // load from const pool instead.
                TCEString packName = "PACK";
                switch (elemVT.SimpleTy) {
                case MVT::i8: packName << "8"; break;
                case MVT::i16: packName << "16"; break;
                case MVT::i32: packName << "32"; break;
                }
                packName << "X" << elemCount;
                // pack op not found from the adf or too big
                if (elemCount > 4 || !tm_.hasOperation(packName)) {
                    return SDValue(nullptr, 0);
                }
            }
        }

        if (canEncodeConstantOperands(*node)) {
            return Op;
        }
    }

    // TODO: Check if there is enough register for the build_vector needed by
    // LLVM's register allocator.

    // There is issue with build_vector to be selected as all-register-operand
    // version of PACK (i.e. PACKtrrrrrrrr). LLVM's register allocator tries
    // allocate as many i32 registers as there is register operands. For
    // example with PACK8X64, the allocator tries to reserve 64 i32 register(!)
    // and likely runs out of them.

    if (Application::verboseLevel() > 1) {
        std::cerr << "Expanding build_vector of "
                  << Op->getValueType(0).getEVTString()
                  << " = { ";
        for (unsigned i = 0; i < node->getNumOperands(); i++) {
            auto opdNode = node->getOperand(i).getNode();
            if (isa<ConstantSDNode>(opdNode)) {
                ConstantSDNode* cn = cast<ConstantSDNode>(opdNode);
                std::cerr << cn->getSExtValue() << " ";
            } else {
                std::cerr << "Reg ";
            }
        }
        std::cerr << "}" << std::endl;
    }

    // TODO: Expand to insert_vector_elt chain rather than to expansion done by
    // LLVM

    // Expand to a load from constant pool or to an in-stack fabrication.
    return SDValue(nullptr, 0);
}

SDValue 
TCETargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {

    // ARM ripoff 

    // vastart just stores the address of the VarArgsFrameIndex slot into the
    // memory location argument.
    SDLoc dl(Op);
    EVT PtrVT = 
        DAG.getTargetLoweringInfo().getPointerTy(
            getTargetMachine().createDataLayout(), 0);
    SDValue FR = DAG.getFrameIndex(getVarArgsFrameOffset(), PtrVT);
    const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
    return DAG.getStore(
        Op.getOperand(0), dl, FR, Op.getOperand(1), MachinePointerInfo(SV));
}

/**
 * Returns the preferred result type of comparison operations.
 *
 * @param VT Result type of the comparison operation.
 * @return Preferred comparison result type.
 */
EVT
TCETargetLowering::getSetCCResultType(
    const DataLayout &DL, LLVMContext &context, llvm::EVT VT) const {
    if (VT.isVector()) {
        EVT resultVectorType = getSetCCResultVT(VT);
        if (resultVectorType != MVT::INVALID_SIMPLE_VALUE_TYPE) {
            return resultVectorType;
        }
    }
    if (!VT.isVector()) return hasI1RC_ ? llvm::MVT::i1 : llvm::MVT::i32;
    return VT.changeVectorElementTypeToInteger();
}

#ifdef OLD_VECTOR_CODE
static
SDValue LowerLOAD(SDValue Op, SelectionDAG &DAG) {
    EVT VT = Op.getValueType();
    DebugLoc dl = Op.getDebugLoc();
    SDValue Chain = Op.getOperand(0);

    // TODO: why is this here?
    if (VT == MVT::v4i32) {
	EVT ptrVT = Op.getOperand(1).getValueType();

	SDValue Ptr0, Ptr1, Ptr2, Ptr3;
	SDValue Imm0 = DAG.getConstant(0, ptrVT);
	SDValue Imm1 = DAG.getConstant(1, ptrVT);
	SDValue Imm2 = DAG.getConstant(2, ptrVT);
	SDValue Imm3 = DAG.getConstant(3, ptrVT);
	
	Ptr0 = Op.getOperand(1);
	Ptr1 = DAG.getNode(ISD::ADD, dl, ptrVT,
			   Op.getOperand(1), Imm1);
	Ptr2 = DAG.getNode(ISD::ADD, dl, ptrVT,
			   Op.getOperand(1), Imm2);
	Ptr3 = DAG.getNode(ISD::ADD, dl, ptrVT,
			   Op.getOperand(1), Imm3);
	SDValue Elt0 = DAG.getLoad(
	    MVT::i32, dl, Chain, Ptr0, MachinePointerInfo(), false, false, 0);
	SDValue Elt1 = DAG.getLoad(
	    MVT::i32, dl, Chain, Ptr1, MachinePointerInfo(), false, false, 0);
	SDValue Elt2 = DAG.getLoad(
	    MVT::i32, dl, Chain, Ptr2, MachinePointerInfo(), false, false, 0);
	SDValue Elt3 = DAG.getLoad(
	    MVT::i32, dl, Chain, Ptr3, MachinePointerInfo(), false, false, 0);
    // SDValue Result = DAG.getTargetInsertSubreg(0, dl, MVT::v4i32,
    //                  DAG.getTargetInsertSubreg(1, dl, MVT::v4i32,
    //                  DAG.getTargetInsertSubreg(2, dl, MVT::v4i32,
    //                  DAG.getTargetInsertSubreg(3, dl, MVT::v4i32,
    // 					       DAG.getUNDEF(MVT::v4i32),
    // 					       Elt3), Elt2), Elt1), Elt0);
	
	// SDValue Result = DAG.getNode(ISD::BUILD_VECTOR, dl, MVT::v4i32,
	// 			     Elt0, Elt1, Elt2, Elt3);

	SDValue Result = DAG.getNode(ISD::INSERT_VECTOR_ELT, dl, MVT::v4i32,
			 DAG.getNode(ISD::INSERT_VECTOR_ELT, dl, MVT::v4i32,
			 DAG.getNode(ISD::INSERT_VECTOR_ELT, dl, MVT::v4i32,
			 DAG.getNode(ISD::INSERT_VECTOR_ELT, dl, MVT::v4i32,
			 DAG.getNode(ISD::UNDEF, dl, MVT::v4i32),
				     Elt0, Imm0),
				     Elt1, Imm1),
				     Elt2, Imm2),
				     Elt3, Imm3);

	Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
			    Elt0.getValue(1), Elt1.getValue(1),
			    Elt2.getValue(1), Elt3.getValue(1));

	SDValue Ops[] = {Result, Chain};

	return DAG.getMergeValues(Ops, 2, dl);
    }

    llvm_unreachable("Invalid LOAD to lower!");
}
#endif

std::pair<int, TCEString> TCETargetLowering::getConstShiftNodeAndTCEOP(SDValue op) const {
    switch(op.getOpcode()) {
    case ISD::SRA:
        return std::make_pair(TCEISD::SRA_Const, TCEString("SHR"));
    case ISD::SRL:
        return std::make_pair(TCEISD::SRL_Const, TCEString("SHRU"));
    case ISD::SHL:
        return std::make_pair(TCEISD::SHL_Const, TCEString("SHL"));
    default:
        return std::make_pair(0, TCEString("unknown op"));
    }
}

SDValue
TCETargetLowering::LowerShift(SDValue op, SelectionDAG& dag) const {

    auto shiftOpcodes = getConstShiftNodeAndTCEOP(op);
    int shiftOpcode = shiftOpcodes.first;
    assert(shiftOpcode && "Shift opcide not supported, should not be here");

    SDValue R = op.getOperand(0);
    SDValue Amt = op.getOperand(1);
    const DebugLoc& dl = op.getDebugLoc();
    std::set<unsigned long> supportedShifts;


    // find all the constant shifts
    for (int i = 1; i < 32; i++) {
        TCEString opName = shiftOpcodes.second; opName << i << "_32";
        if (tm_.hasOperation(opName)) {
            supportedShifts.insert(i);
        }
    }

    // add also 1-bit shift for add
    // we should ALWAYS have an add but - lets check to be sure ;)
    if (tm_.hasOperation("ADD")) {
        supportedShifts.insert(1);
    }

    if (Amt.getOpcode() == ISD::Constant) {
        unsigned long amount = op.getConstantOperandVal(1);
        // if has no correct-width shift, need to break down into multiple.
        if (supportedShifts.find(amount) == supportedShifts.end()) {
            // find the biggest suitable shift.
            for (auto i = supportedShifts.rbegin();
                 i != supportedShifts.rend(); i++) {
                if (amount > *i) {
                    auto shiftVal =
                        dag.getConstant(*i, op, Amt.getValueType());
                    auto remVal =
                        dag.getConstant(amount - *i, op, Amt.getValueType());
                    SDValue remaining = dag.getNode(
                        op.getOpcode(), op, op.getValueType(), R, remVal);
                    SDValue lowered = LowerShift(remaining, dag);
                    SDValue shift = dag.getNode(
                        shiftOpcode, op, op.getValueType(), lowered, shiftVal);
                    return shift;
                }
            }
        }
        return op;

    } else {
        unsigned Opc = op.getOpcode();
        switch(Opc) {
        case ISD::SRA:
            return ExpandLibCall(RTLIB::SRA_I32, op.getNode(), true, dag);
        case ISD::SRL:
            return ExpandLibCall(RTLIB::SRL_I32, op.getNode(), false, dag);
        case ISD::SHL:
            return ExpandLibCall(RTLIB::SHL_I32, op.getNode(), false, dag);
        default:
            std::cerr << "Invalid dynamic shift opcode" << std::endl;
        }
    }
    return op;
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
    case ISD::Constant: return LowerConstant(op, dag);
    case ISD::BUILD_VECTOR: return LowerBuildVector(op, dag);
    case ISD::SHL:
    case ISD::SRA:
    case ISD::SRL: return LowerShift(op, dag);
    case ISD::LOAD: return lowerExtOrBoolLoad(op, dag);
    case ISD::DYNAMIC_STACKALLOC: {
        assert(false && "Dynamic stack allocation not yet implemented.");
    }
#ifdef OLD_VECTOR_CODE
    case ISD::LOAD: return LowerLOAD(op, dag);
#endif
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
TCETargetLowering::getConstraintType(StringRef Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:  break;
    case 'r': return C_RegisterClass;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

const TargetRegisterClass*
TCETargetLowering::getVectorRegClassForInlineAsmConstraint(
    const TargetRegisterInfo* TRI,
    MVT VT) const {

    if (!VT.isVector()) return nullptr;

    const TargetRegisterClass* bestVRC = nullptr;
    // Find smallest RF by using stack spill size as reg size indication.
    for (unsigned i = 0U; i < TRI->getNumRegClasses(); i++) {
        auto vrc = TRI->getRegClass(i);
        if (TRI->isTypeLegalForClass(*vrc, VT) &&
            (!bestVRC || vrc->MC->RegsSize < bestVRC->MC->RegsSize)) {
            bestVRC = vrc;
        }
    }
    return bestVRC;
}

/**
 * Returns proper register class for given value type.
 *
 * @param Constraint A constraint defined for an inline asm operation operand.
 * @return Proper register class for the operand value type.
 */
std::pair<unsigned, const TargetRegisterClass *>
TCETargetLowering::getRegForInlineAsmConstraint(
    const TargetRegisterInfo *TRI,
    StringRef Constraint, MVT VT) const {
    if (Constraint.size() == 1) {
        // check if value type is a vector and return associated reg class
        std::pair<unsigned, const TargetRegisterClass *> rcPair =
            associatedVectorRegClass(VT);

        switch (Constraint[0]) {
            case 'r':
                // if found associated vector reg class
                if (rcPair.second != NULL) {
                    return rcPair;
                }
        }
    }

    bool isPhysicalRegister = Constraint.size() > 3
        && Constraint.front() == '{' && Constraint.back() == '}';

    const TargetRegisterClass* vrc = nullptr;
    if (Constraint.size() == 1) {
        switch (Constraint[0]) {
            case 'r':
                // Prefer vector RFs for vector types and then try
                // scalar RFs.
                vrc = getVectorRegClassForInlineAsmConstraint(TRI, VT);
                if (vrc) return std::make_pair(0U, vrc);

                switch (VT.getSizeInBits()) {
                    case 8:
                    case 16:
                    case 32:
                    case 64:
                        return std::make_pair(0U, &DEFAULT_REG_CLASS);
                     default:
                         break;
                }
                return std::make_pair(0U, nullptr); // return error.
            // TODO: this should be some other char. But change it in devel64b
            case 's':
                return std::make_pair(0U, &TCE::R64RegsRegClass);
            case 'f':
                if (VT == MVT::f32) {
                    return std::make_pair(0U, &TCE::FPRegsRegClass);
                }
#ifdef TARGET64BIT
            case 'd':
                return std::make_pair(0U, &TCE::R64DFPRegsRegClass);
#endif
        }
    } else if (isPhysicalRegister) {
        // Constraint = {<RF-name>.<Register-index>}
        const std::string regName = Constraint.substr(1, Constraint.size()-2).str();
        unsigned regId = tm_.llvmRegisterId(regName);
        if (regId == TCE::NoRegister) {
            // No such register. Return error.
            return std::make_pair(0, nullptr);
        }

        // In case the reg is boolean register via local register
        // variable (ie. "register int foo asm("BOOL.1") = ...").
        if (TCE::R1RegsRegClass.contains(regId)) {
            return std::make_pair(regId, &TCE::R1RegsRegClass);
        }
        if (TCE::GuardRegsRegClass.contains(regId)) {
            return std::make_pair(regId, &TCE::GuardRegsRegClass);
        }

        return std::make_pair(regId, TRI->getMinimalPhysRegClass(regId, VT));
    }
    return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

// For invalid constraint, like unsupported immediates, add nothing into Ops.
void
TCETargetLowering::LowerAsmOperandForConstraint(
    SDValue Op,
    std::string& Constraint,
    std::vector<SDValue>& Ops,
    SelectionDAG& DAG) const {

    if (Constraint.length() == 1) {
        switch (Constraint[0]) {
            case 'i':
                if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                    if (!canEncodeImmediate(*C)) {
                        return;
                    }
                }
                break;
            default:
                break;
        }
    }

    TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
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



bool
    TCETargetLowering::allowsMisalignedMemoryAccesses(EVT, unsigned, unsigned, MachineMemOperand::Flags, bool*) const {
    /// @todo This commented area and the whole function is probably not
    /// needed anymore. The base class version returns false as default.
    return false;
}

/**
 * Returns true if all operands of the SDNode are constants or undefined.
 */
bool
TCETargetLowering::isConstantOrUndefBuild(const SDNode& node) const {
    for (unsigned i = 0; i < node.getNumOperands(); i++) {
        auto opc = node.getOperand(i)->getOpcode();
        if (opc != ISD::Constant && opc != ISD::UNDEF) {
            return false;
        }
    }
    return true;
}

/**
 * Check if constant operands used by the SDNode can be encoded as immediate
 * on the target machine.
 */
bool
TCETargetLowering::canEncodeConstantOperands(const SDNode& node) const {
    for (unsigned i = 0; i < node.getNumOperands(); i++) {
        if (node.getOperand(i)->getOpcode() != ISD::Constant) continue;
        ConstantSDNode* cn =
            cast<ConstantSDNode>(node.getOperand(i).getNode());
        if (!canEncodeImmediate(*cn)) return false;
    }
    return true;
}

/**
 * Check if the constant can be generally encoded as immediate
 * on the target machine.
 */
bool
TCETargetLowering::canEncodeImmediate(const ConstantSDNode& node) const {
    int64_t val = node.getSExtValue();
    MVT vt = node.getSimpleValueType(0);

    // We accept here only constant that can be materialized in instruction
    // selection in some way and this must be done by the lowest common
    // denominator.

    // can encode as MOVI?
    // Assuming here, that the immediate can be transported to any target
    // machine operation.
    if (tm_.canEncodeAsMOVI(vt, val))
        return true;

    // can encode as immediate to operation
    // TODO?

    // can encode as immToOp for user that is exactly known to be selected
    // to certain target instruction?

    // can encode through ISEL transformation?
    if (tm_.canMaterializeConstant(*node.getConstantIntValue()))
        return true;

    return false;
}

/**
 * Returns true if the address values should be loaded from constant pool due
 * to limited immediate support.
 *
 */
bool
TCETargetLowering::shouldLoadFromConstantPool(unsigned addressSpace) const {
    if (loadGAFromConstantPool_.count(addressSpace) == 0) {
        // Default behavior for unspecified address spaces.
        assert(loadGAFromConstantPool_.count(0));
        return loadGAFromConstantPool_.at(0);
    }

    return loadGAFromConstantPool_.at(addressSpace);
}

/**
 * Returns true if the target machine has register class for i1 types.
 */
bool
TCETargetLowering::hasI1RegisterClass() const {
    if (TCE::R1RegsRegClass.getNumRegs() == 0) return false;

    // TDGen generates dummy register class for the machines without boolean
    // RFs.
    if (TCE::R1RegsRegClass.getNumRegs() == 1) {
        std::string regName = tm_.rfName(TCE::R1RegsRegClass.getRegister(0));
        if (regName.find("dummy") != std::string::npos) return false;
    }

    return true;
}

/**
 * Check the FP in bits can be fit in machine's immediates.
 */
bool
    TCETargetLowering::isFPImmLegal(
        const APFloat& apf, EVT VT, bool forCodeSize) const {
    if (VT==MVT::f32 || VT==MVT::f16) {
        return tm_.canEncodeAsMOVF(apf);
    }
    return false;
}

bool
TCETargetLowering::isBroadcast(SDNode *n) {
    if (n->getOpcode() != ISD::BUILD_VECTOR) {
        return false;
    }
    SDValue val = n->getOperand(0);
    int operandCount = n->getNumOperands();
    for (unsigned i = 1; i <operandCount; i++) {
        SDValue val2 = n->getOperand(i);
        SDNode* node2 = dyn_cast<SDNode>(val2);
        if (node2->getOpcode() != ISD::UNDEF) {
            if (val2 != val)
                return false;
        }
    }
    return true;
}


// TODO: This is copypaste from legalizeDAG. Because the
// routine in legalizeDAG is not public
SDValue
    TCETargetLowering::ExpandLibCall(RTLIB::Libcall LC, SDNode *Node,
                                     bool isSigned, SelectionDAG &DAG) const {

  TargetLowering::ArgListTy Args;
  TargetLowering::ArgListEntry Entry;
  for (const SDValue &Op : Node->op_values()) {
    EVT ArgVT = Op.getValueType();
    Type *ArgTy = ArgVT.getTypeForEVT(*DAG.getContext());
    Entry.Node = Op;
    Entry.Ty = ArgTy;
    Entry.IsSExt = shouldSignExtendTypeInLibCall(ArgVT, isSigned);
    Entry.IsZExt = !shouldSignExtendTypeInLibCall(ArgVT, isSigned);
    Args.push_back(Entry);
  }
  SDValue Callee = DAG.getExternalSymbol(getLibcallName(LC),
                                         getPointerTy(DAG.getDataLayout(),0));

  EVT RetVT = Node->getValueType(0);
  Type *RetTy = RetVT.getTypeForEVT(*DAG.getContext());

  // By default, the input chain to this libcall is the entry node of the
  // function. If the libcall is going to be emitted as a tail call then
  // TLI.isUsedByReturnOnly will change it to the right chain if the return
  // node which is being folded has a non-entry input chain.
  SDValue InChain = DAG.getEntryNode();

  // isTailCall may be true since the callee does not reference caller stack
  // frame. Check if it's in the right position and that the return types match.
  SDValue TCChain = InChain;
  const Function &F = DAG.getMachineFunction().getFunction();
  bool isTailCall =
      isInTailCallPosition(DAG, Node, TCChain) &&
      (RetTy == F.getReturnType() || F.getReturnType()->isVoidTy());
  if (isTailCall)
    InChain = TCChain;

  TargetLowering::CallLoweringInfo CLI(DAG);
  bool signExtend = shouldSignExtendTypeInLibCall(RetVT, isSigned);
  CLI.setDebugLoc(SDLoc(Node))
      .setChain(InChain)
      .setLibCallee(getLibcallCallingConv(LC), RetTy, Callee,
                    std::move(Args))
      .setTailCall(isTailCall)
      .setSExtResult(signExtend)
      .setZExtResult(!signExtend)
      .setIsPostTypeLegalization(true);

  std::pair<SDValue, SDValue> CallInfo = LowerCallTo(CLI);

  if (!CallInfo.second.getNode()) {
    // It's a tailcall, return the chain (which is the DAG root).
    return DAG.getRoot();
  }

  return CallInfo.first;
}

void TCETargetLowering::ReplaceNodeResults(
    SDNode* node, SmallVectorImpl<SDValue>& Results,
    SelectionDAG& DAG) const {
    auto fnName = DAG.getMachineFunction().getName().str();

    SDValue shiftedVal;
    SDValue truncAnd;
    if (node->getOpcode() == ISD::LOAD) {
        auto lsdn = dyn_cast<LoadSDNode>(node);
        if (lsdn == nullptr) {
            std::cerr << "Error: null loadsdnde!" << std::endl;
            return;
        }

        if (lsdn->getAlignment() < 2 &&
            lsdn->getMemoryVT() != MVT::i8 && lsdn->getMemoryVT() != MVT::i1) {
            assert(0 && "Cannot lower 16-bit memory op with only one byte alignment");
        }

        auto chain = node->getOperand(0);

        SDValue load;
        SDValue lowBits;
        if (lsdn->getAlignment() >= 4) {
            load = DAG.getLoad(
            MVT::i32, node, chain, lsdn->getBasePtr(), MachinePointerInfo());
        } else {
            auto alignedAddr =
                DAG.getNode(
                    ISD::AND, node, MVT::i32, lsdn->getBasePtr(),
                    DAG.getConstant(-4l, node, MVT::i32));

            auto lowBytes = DAG.getNode(
                ISD::AND, node, MVT::i32, lsdn->getBasePtr(),
                DAG.getConstant(3l, node, MVT::i32));

            lowBits = DAG.getNode(
                ISD::SHL, node, MVT::i32, lowBytes,
                DAG.getConstant(3l, node, MVT::i32));

            load = DAG.getLoad(
                MVT::i32, node, chain, alignedAddr, MachinePointerInfo());
        }

        // TODO: breaks with 64 bits!
        // TODO: also breaks with 16-bit floats?
        MVT vt = node->getSimpleValueType(0);
        if (vt == MVT::i32) {
            assert(0 && "Result i32? this should be extload?");
            Results.push_back(SDValue(load));
            Results.push_back(SDValue(load.getNode(),1));
            return;
        }

        SDValue finalVal;
        if (lsdn->getExtensionType() == ISD::ZEXTLOAD) {
            shiftedVal = lsdn->getAlignment() < 4 ?
                DAG.getNode(ISD::SRA, node, MVT::i32, load, lowBits):
                load;

            if (lsdn->getMemoryVT() == MVT::i1) {
                finalVal = DAG.getNode(
                    ISD::AND, node, MVT::i32, shiftedVal,
                    DAG.getConstant(1l, node, MVT::i32));
            } else if (lsdn->getMemoryVT() == MVT::i8) {
                finalVal = DAG.getNode(
                    ISD::AND, node, MVT::i32, shiftedVal,
                    DAG.getConstant(255l, node, MVT::i32));
            } else {
                // TODO: 64-bit port needs to add option for 32-bit here.
                assert(0 && "Wrong memory vt in zextload!");
            }
        } else if (lsdn->getExtensionType() == ISD::SEXTLOAD) {
            if (lsdn->getMemoryVT() == MVT::i1) {
                auto shiftsLeft =
                    DAG.getNode(ISD::SUB, node, MVT::i32,
                                DAG.getConstant(31l, node, MVT::i32),lowBits);
                auto shiftUp = DAG.getNode(
                    ISD::SHL, node, MVT::i32, load, shiftsLeft);
                finalVal = DAG.getNode(
                    ISD::SRA, node, MVT::i32, shiftUp,
                    DAG.getConstant(31l, node, MVT::i32));
            } else if (lsdn->getMemoryVT() == MVT::i8) {
                auto shiftsLeft =
                    DAG.getNode(ISD::SUB, node, MVT::i32,
                                DAG.getConstant(24l, node, MVT::i32),lowBits);
                auto shiftUp = DAG.getNode(
                    ISD::SHL, node, MVT::i32, load, shiftsLeft);
                finalVal = DAG.getNode(
                    ISD::SRA, node, MVT::i32, shiftUp,
                    DAG.getConstant(24l, node, MVT::i32));
            } else {
                // TODO: 64-bit port needs to add option for 32-bit here.
                assert(0 && "Wrong memory vt in sextload!");
            }
        } else { // anyext/noext.
            finalVal = lsdn->getAlignment() < 4 ?
                DAG.getNode(ISD::SRA, node, MVT::i32, load, lowBits):
                load;
        }

        SDValue rv;
        if (vt == MVT::i16) {
            rv = DAG.getAnyExtOrTrunc(finalVal, node, MVT::i16);
        } else if (vt == MVT::i8) {
            rv = DAG.getAnyExtOrTrunc(finalVal, node, MVT::i8);
        } else if (vt == MVT::i1) {
            rv = DAG.getAnyExtOrTrunc(finalVal, node, MVT::i1);
        } else {
            assert(0 && "Wrong vt in load lowering!");
        }

        Results.push_back(rv);
        Results.push_back(SDValue(load.getNode(),1));
    } else {
        assert(false && "ReplaceNodeResults not load!");
    }
}

/**
 * Lowers extension load of 8- or 16-bit load to 32-bit little-endian load.
 */
SDValue TCETargetLowering::lowerExtOrBoolLoad(
    SDValue op,
    SelectionDAG& DAG) const {

    auto lsdn = dyn_cast<LoadSDNode>(op.getNode());
    if (lsdn == nullptr) {
        assert(false && "Not a lodsdnode on LowerExtLoad!");
    }

    auto chain = op.getOperand(0);
    SDValue alignedAddr;
    SDValue lowBits;

    if (lsdn->getAlignment() >= 4) {
        alignedAddr = lsdn->getBasePtr();
        lowBits = DAG.getConstant(0l, op, MVT::i32);
    } else {
        alignedAddr = DAG.getNode(
            ISD::AND, op, MVT::i32, lsdn->getBasePtr(),
            DAG.getConstant(-4l, op, MVT::i32));

        auto lowBytes = DAG.getNode(
            ISD::AND, op, MVT::i32, lsdn->getBasePtr(),
            DAG.getConstant(3l, op, MVT::i32));

        lowBits = DAG.getNode(
            ISD::SHL, op, MVT::i32, lowBytes,
            DAG.getConstant(3l, op, MVT::i32));
    }

    auto load = DAG.getLoad(
        MVT::i32, op, chain, alignedAddr, MachinePointerInfo());

    // this is little-endian code. big endian needs different.
    if (lsdn->getExtensionType() == ISD::ZEXTLOAD) {
        auto shiftedValue = lsdn->getAlignment() < 4 ?
            DAG.getNode(ISD::SRA, op, MVT::i32, load, lowBits) :
            load;
        if (lsdn->getMemoryVT() == MVT::i16) {
            assert(lsdn->getAlignment() >= 2 &&
                   "Cannot (yet?) emulate a 16-bit load which has 1-byte alignment. "
                   " 16-bit memory operations needed to compile this code." );
            std::cerr << "\t\tSource is 16 bits." << std::endl;
            auto zext = DAG.getNode(
                ISD::AND, op, MVT::i32, shiftedValue,
                DAG.getConstant(65535l, op, MVT::i32));
            return zext;
        } else if (lsdn->getMemoryVT() == MVT::i8) {
            auto zext = DAG.getNode(
                ISD::AND, op, MVT::i32, shiftedValue,
                DAG.getConstant(255l, op, MVT::i32));
            return zext;
        } else if (lsdn->getMemoryVT() == MVT::i1) {
            auto zext = DAG.getNode(
                ISD::AND, op, MVT::i32, shiftedValue,
                DAG.getConstant(1l, op, MVT::i32));
            return zext;
        } else {
            assert(false && "Unknown data type on LowerSExtLoad!");
        }
    }
    if (lsdn->getExtensionType() == ISD::SEXTLOAD) {

        // shift left to get it to upper bits, then arithmetic right.
        if (lsdn->getMemoryVT() == MVT::i16) {
            auto shiftsLeft = lsdn->getAlignment() < 4 ?
                DAG.getNode(ISD::SUB, op, MVT::i32,
                            DAG.getConstant(16l, op, MVT::i32),
                            lowBits) :
                DAG.getConstant(16l, op, MVT::i32);
            auto shiftUp = DAG.getNode(
                ISD::SHL, op, MVT::i32, load, shiftsLeft);
            auto shiftDown = DAG.getNode(
                ISD::SRA, op, MVT::i32, shiftUp,
                DAG.getConstant(16l, op, MVT::i32));
            return shiftDown;
        } else if (lsdn->getMemoryVT() == MVT::i8) {
            auto shiftsLeft = lsdn->getAlignment() < 4 ?
                DAG.getNode(ISD::SUB, op, MVT::i32,
                            DAG.getConstant(24l, op, MVT::i32),
                            lowBits) :
                DAG.getConstant(24l, op, MVT::i32);
            auto shiftUp =
                DAG.getNode(ISD::SHL, op, MVT::i32, load, shiftsLeft);
            auto shiftDown = DAG.getNode(
                ISD::SRA, op, MVT::i32, shiftUp,
                DAG.getConstant(24l, op, MVT::i32));
            return shiftDown;
        } else if (lsdn->getMemoryVT() == MVT::i1) {
            auto shiftsLeft = lsdn->getAlignment() < 4 ?
                DAG.getNode(ISD::SUB, op, MVT::i32,
                            DAG.getConstant(31l, op, MVT::i32),
                            lowBits) :
                DAG.getConstant(31l, op, MVT::i32);

            auto shiftUp =
                DAG.getNode(ISD::SHL, op, MVT::i32, load, shiftsLeft);
            auto shiftDown = DAG.getNode(
                ISD::SRA, op, MVT::i32, shiftUp,
                DAG.getConstant(31l, op, MVT::i32));
            return shiftDown;
        } else {
            assert(false && "Unknown data type on Lower(Z)ExtLoad!");
        }
    }

    // anyext?
    if (lsdn->getExtensionType() == ISD::EXTLOAD) {
        auto shiftedValue = lsdn->getAlignment() < 4 ?
            DAG.getNode(ISD::SRA, op, MVT::i32, load, lowBits) :
            load;
        auto shiftDown = DAG.getNode(ISD::SRA, op, MVT::i32, load, lowBits);
        return shiftDown;
    } else {
        // normal, not-extload.
        MVT vt = op->getSimpleValueType(0);
        if  (vt == MVT::i1 && lsdn->getMemoryVT() == MVT::i1) {
            SDValue trunc = DAG.getAnyExtOrTrunc(load, op, MVT::i1);
            return trunc;
        }

        assert(false && "Should not be here, non-ext-load");
    }
    return SDValue();
}
