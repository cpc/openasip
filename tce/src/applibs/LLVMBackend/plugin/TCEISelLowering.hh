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
 * @file TCETargetLowering.h
 *
 * Declaration of TCETargetLowering class.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef TCE_TARGET_LOWERING_H
#define TCE_TARGET_LOWERING_H

#include <llvm/Target/TargetLowering.h>
#include "TCEPlugin.hh"
#include "tce_config.h"

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

        Hi, Lo,      // Hi/Lo operations, typically on a global address.

        FTOI,        // FP to Int within a FP register.
        ITOF,        // Int to FP within a FP register.

        CALL,        // A call instruction.
        RET_FLAG ,    // Return with a flag operand.
        RET_FLAG_old
   };
}

namespace llvm {

    class TCETargetMachine;

    /**
     *  Lowers LLVM code to SelectionDAG for the TCE backend.
     */
    class TCETargetLowering : public llvm::TargetLowering {
        mutable int VarArgsFrameOffset;   // Frame offset to start of varargs area.
    public:
        TCETargetLowering(TargetMachine& TM);
        virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const;

        int getVarArgsFrameOffset() const { return VarArgsFrameOffset; }

       virtual const char* getTargetNodeName(unsigned opcode) const;
        
        ConstraintType getConstraintType(const std::string &Constraint) const;

#if (defined(LLVM_3_2) || defined(LLVM_3_3))
        std::pair<unsigned, const TargetRegisterClass*>
        getRegForInlineAsmConstraint(const std::string &Constraint, EVT VT) const;
#else
        std::pair<unsigned, const TargetRegisterClass*>
        getRegForInlineAsmConstraint(const std::string &Constraint, MVT VT) const;
#endif
        std::vector<unsigned>
        getRegClassForInlineAsmConstraint(const std::string &Constraint,
                                          EVT VT) const;
        virtual bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const;
        
        /// getFunctionAlignment - Return the Log2 alignment of this function.
        virtual unsigned getFunctionAlignment(const Function *F) const;

#if (defined(LLVM_3_2) || defined(LLVM_3_3))        
        virtual SDValue
        LowerFormalArguments(SDValue Chain,
                             CallingConv::ID CallConv,
                             bool isVarArg,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const;
#else
        virtual SDValue
        LowerFormalArguments(SDValue Chain,
                             CallingConv::ID CallConv,
                             bool isVarArg,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             SDLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const;
#endif        

        SDValue LowerTRAP(SDValue Op, SelectionDAG &DAG) const;


        virtual SDValue
        LowerCall(TargetLowering::CallLoweringInfo &CLI,
                  SmallVectorImpl<SDValue> &InVals) const;
        
#if (defined(LLVM_3_2) || defined(LLVM_3_3))        
        virtual SDValue
        LowerReturn(SDValue Chain,
                    CallingConv::ID CallConv, bool isVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals,
                    DebugLoc dl, SelectionDAG &DAG) const;
#else
        virtual SDValue
        LowerReturn(SDValue Chain,
                    CallingConv::ID CallConv, bool isVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals,
                    SDLoc dl, SelectionDAG &DAG) const;
#endif

        virtual bool allowsUnalignedMemoryAccesses(EVT VT) const;

        // We can ignore the bitwidth differences between the pointers
        // for now. It's the programmer's responsibility to ensure they
        // fit.
        virtual bool isNoopAddrSpaceCast(unsigned SrcAS, unsigned DestAS) const {
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
#if (defined(LLVM_3_2) || defined(LLVM_3_3))
        virtual llvm::EVT getSetCCResultType(llvm::EVT VT) const;
#else
        virtual llvm::EVT getSetCCResultType(LLVMContext&,llvm::EVT VT) const;
#endif        
        virtual bool isFPImmLegal(const APFloat& apf, EVT VT) const {
            if (VT==MVT::f32 || VT==MVT::f16) {
                return true;
            }
            return false;
        }
    };
}

#endif
