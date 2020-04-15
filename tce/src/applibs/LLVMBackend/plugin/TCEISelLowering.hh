/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TCETargetLowering.h
 *
 * Declaration of TCETargetLowering class.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef TCE_TARGET_LOWERING_H
#define TCE_TARGET_LOWERING_H

#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetLowering.h>
#else
#include <llvm/CodeGen/TargetLowering.h>
#endif
#include "TCEPlugin.hh"
#include "TCESubtarget.hh"
#include "tce_config.h"
#include "TCEString.hh"
#include <map>
#include <iostream>

namespace TCEISD {
    enum {
        FIRST_NUMBER = llvm::ISD::BUILTIN_OP_END,
        SELECT_I1,
        SELECT_I8,
        SELECT_I16,
        SELECT_I32,
        SELECT_I64,
        SELECT_F16,
        SELECT_F32,
        SELECT_F64,

        CONST_POOL,
        GLOBAL_ADDR,
        BLOCK_ADDR,

        Hi, Lo,      // Hi/Lo operations, typically on a global address.

        FTOI,        // FP to Int within a FP register.
        ITOF,        // Int to FP within a FP register.

        CALL,        // A call instruction.
        RET_FLAG ,    // Return with a flag operand.
        RET_FLAG_old,
        SRA_Const,
        SRL_Const,
        SHL_Const
   };
}

namespace llvm {

#ifdef LLVM_OLDER_THAN_3_9
#define SDLOC_PARAM_TYPE SDLoc
#else
#define SDLOC_PARAM_TYPE const SDLoc&
#endif

    class TCETargetMachine;

    /**
     *  Lowers LLVM code to SelectionDAG for the TCE backend.
     */
    class TCETargetLowering : public llvm::TargetLowering {
        mutable int VarArgsFrameOffset;   // Frame offset to start of varargs area.
    public:
        TCETargetLowering(TargetMachine& TM, const TCESubtarget &subt);
        virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

        int getVarArgsFrameOffset() const /* override */ { return VarArgsFrameOffset; }

       virtual const char* getTargetNodeName(unsigned opcode) const override;

        ConstraintType getConstraintType(StringRef Constraint) const override;
        std::pair<unsigned, const TargetRegisterClass *>
        getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                     StringRef Constraint, MVT VT) const override;

        //TODO: this is from some old version - which?
        std::vector<unsigned>
        getRegClassForInlineAsmConstraint(const std::string &Constraint,
                                          EVT VT) const;
        virtual bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;
        
        /// getFunctionAlignment - Return the Log2 alignment of this function.

        virtual SDValue
        LowerFormalArguments(SDValue Chain,
                             CallingConv::ID CallConv,
                             bool isVarArg,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             SDLOC_PARAM_TYPE dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const override;

        SDValue LowerTRAP(SDValue Op, SelectionDAG &DAG) const;
        SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG) const;
        SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;
        SDValue LowerShift(SDValue op, SelectionDAG& dag) const;
        SDValue lowerExtOrBoolLoad(SDValue op, SelectionDAG& DAG) const;

        std::pair<int, TCEString> getConstShiftNodeAndTCEOP(SDValue op) const;

        virtual SDValue
        LowerCall(TargetLowering::CallLoweringInfo &CLI,
                  SmallVectorImpl<SDValue> &InVals) const override;
        
        virtual SDValue
        LowerReturn(SDValue Chain,
                    CallingConv::ID CallConv, bool isVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals,
                    SDLOC_PARAM_TYPE dl, SelectionDAG &DAG) const override;

#ifdef LLVM_OLDER_THAN_9
        virtual bool allowsMisalignedMemoryAccesses(EVT VT, unsigned as, unsigned align, bool* ) const override;
#else // LLVM 9+
        virtual bool allowsMisalignedMemoryAccesses(EVT VT, unsigned as, unsigned align, MachineMemOperand::Flags flags, bool* ) const override;
#endif
        // We can ignore the bitwidth differences between the pointers
        // for now. It's the programmer's responsibility to ensure they
        // fit.
        virtual bool isNoopAddrSpaceCast(unsigned SrcAS, unsigned DestAS) const override {
            return true;
        }
        // ----------------------------------------------------
        //
        //  Below here TCE specific stuff is added, which is not copied from Sparc
        //
        // ----------------------------------------------------
    private:
        TCETargetMachine& tm_;
        
    public:        
        virtual llvm::EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                                       EVT VT) const override;

#ifdef LLVM_OLDER_THAN_9
        virtual bool isFPImmLegal(const APFloat& apf, EVT VT) const override {
            if (VT==MVT::f32 || VT==MVT::f16) {
                return true;
            }
            return false;
        }
#else
        virtual bool isFPImmLegal(
            const APFloat& apf, EVT VT, bool forCodeSize) const override {
            if (VT==MVT::f32 || VT==MVT::f16) {
                return true;
            }
            return false;
        }
#endif

    SDValue ExpandLibCall(
        RTLIB::Libcall LC, SDNode *Node, bool isSigned, SelectionDAG &DAG)
        const;

        void ReplaceNodeResults(SDNode * node,
                                SmallVectorImpl< SDValue > &,
                                SelectionDAG &) const override;

    };
}

#endif
