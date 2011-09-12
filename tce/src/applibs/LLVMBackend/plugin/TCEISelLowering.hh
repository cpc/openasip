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

namespace TCEISD {
    enum {
        FIRST_NUMBER = llvm::ISD::BUILTIN_OP_END,
        SELECT_I1,
        SELECT_I8,
        SELECT_I16,
        SELECT_I32,
        SELECT_I64,
        SELECT_F32,
        SELECT_F64,

        CONST_POOL,
        GLOBAL_ADDR,

        Hi, Lo,      // Hi/Lo operations, typically on a global address.

        FTOI,        // FP to Int within a FP register.
        ITOF,        // Int to FP within a FP register.

        CALL,        // A call instruction.
        RET_FLAG     // Return with a flag operand.
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
        std::pair<unsigned, const TargetRegisterClass*>
        getRegForInlineAsmConstraint(const std::string &Constraint, EVT VT) const;
        std::vector<unsigned>
        getRegClassForInlineAsmConstraint(const std::string &Constraint,
                                          EVT VT) const;
        
        virtual bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const;
        
        /// getFunctionAlignment - Return the Log2 alignment of this function.
        virtual unsigned getFunctionAlignment(const Function *F) const;
        
        virtual SDValue
        LowerFormalArguments(SDValue Chain,
                             CallingConv::ID CallConv,
                             bool isVarArg,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const;
        
        SDValue LowerTRAP(SDValue Op, SelectionDAG &DAG) const;

        virtual SDValue
        LowerCall(SDValue Chain, SDValue Callee,
                  CallingConv::ID CallConv, bool isVarArg,
                  bool& isTailCall,
                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                  const SmallVectorImpl<SDValue> &OutVals,
                  const SmallVectorImpl<ISD::InputArg> &Ins,
                  DebugLoc dl, SelectionDAG &DAG,
                  SmallVectorImpl<SDValue> &InVals) const;
        
        virtual SDValue
        LowerReturn(SDValue Chain,
                    CallingConv::ID CallConv, bool isVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals,
                    DebugLoc dl, SelectionDAG &DAG) const;

        // ----------------------------------------------------
        //
        //  Below here TCE specific stuff is added, which is not copied from Sparc
        //
        // ----------------------------------------------------
    private:
        TCETargetMachine& tm_;
        
    public:        
#ifdef LLVM_2_9
        virtual llvm::MVT::SimpleValueType 
#else
        virtual llvm::EVT
#endif
        getSetCCResultType(llvm::EVT VT) const;


    };
}

#endif
