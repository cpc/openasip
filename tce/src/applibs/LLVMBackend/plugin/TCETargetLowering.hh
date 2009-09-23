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
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TCE_TARGET_LOWERING_H
#define TCE_TARGET_LOWERING_H

#include <llvm/Target/TargetLowering.h>

#include "TCEPlugin.hh"

namespace TCEISD {
    enum {
        FIRST_NUMBER = llvm::ISD::BUILTIN_OP_END + llvm::TCE::INSTRUCTION_LIST_END,
        CALL,
        RET_FLAG,
        CONST_POOL,
        GLOBAL_ADDR,
        SELECT_I1,
        SELECT_I8,
        SELECT_I16,
        SELECT_I32,
        SELECT_I64,
        SELECT_F32,
        SELECT_F64
    };
}

namespace llvm {

    class TCETargetMachine;

    /**
     *  Lowers LLVM code to SelectionDAG for the TCE backend.
     */
    class TCETargetLowering : public llvm::TargetLowering {
    public:
        TCETargetLowering(llvm::TCETargetMachine& m);
        virtual ~TCETargetLowering();

        virtual llvm::SDValue LowerOperation(
            llvm::SDValue op, llvm::SelectionDAG& dag);
/*
        virtual void
        LowerArguments(
            llvm::Function& f, 
            llvm::SelectionDAG& dag,
            SmallVectorImpl<SDValue>& argValues,
            DebugLoc dl);

        virtual std::pair<llvm::SDValue, llvm::SDValue>
            LowerCallTo(
                llvm::SDValue chain,
                const llvm::Type* retTy,
                bool retTyIsSigned,
                bool,
                bool isVarArg,
                bool,
                unsigned cc,
                bool isTailCall,
                llvm::SDValue callee,
                llvm::TargetLowering::ArgListTy& args,
                llvm::SelectionDAG& dag,
                llvm::DebugLoc dl);
*/

        virtual const char* getTargetNodeName(unsigned opcode) const;

        virtual unsigned int getFunctionAlignment(const Function*) const {
            return 4;
        }

        virtual SDValue
        LowerFormalArguments(SDValue Chain,
                             unsigned CallConv,
                             bool isVarArg,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals);

        virtual SDValue
        LowerCall(SDValue Chain, SDValue Callee,
                  unsigned CallConv, bool isVarArg,
                  bool isTailCall,
                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                  const SmallVectorImpl<ISD::InputArg> &Ins,
                  DebugLoc dl, SelectionDAG &DAG,
                  SmallVectorImpl<SDValue> &InVals);
        
        virtual SDValue
        LowerReturn(SDValue Chain,
                    unsigned CallConv, bool isVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    DebugLoc dl, SelectionDAG &DAG);

        llvm::SDValue LowerRET(llvm::SDValue op, llvm::SelectionDAG& dag);

        virtual std::pair<unsigned, const TargetRegisterClass*>
            getRegForInlineAsmConstraint(
                const std::string& constraint,
                llvm::MVT vt) const;

        SDValue lowerSELECT(SDValue op, SelectionDAG& dag);
                
        virtual llvm::MVT getSetCCResultType(const SDValue &) const {
            return MVT::i1;
        }

        virtual llvm::MVT getSetCCResultType(llvm::MVT VT) const;

    private:
        // Frame index to the the start of variadic parameter list.
        int varArgsFrameIndex_;

        TCETargetMachine& tm_;
    };
}

#endif
