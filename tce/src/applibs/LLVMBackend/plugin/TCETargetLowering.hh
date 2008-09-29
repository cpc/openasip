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

        virtual llvm::SDOperand LowerOperation(
            llvm::SDOperand op, llvm::SelectionDAG& dag);

        virtual std::vector<llvm::SDOperand>
            LowerArguments(llvm::Function& f, llvm::SelectionDAG& dag);

        virtual std::pair<llvm::SDOperand, llvm::SDOperand>
            LowerCallTo(
                llvm::SDOperand chain,
                const llvm::Type* retTy,
                bool retTyIsSigned,
                bool,
                bool isVarArg,
                unsigned cc,
                bool isTailCall,
                llvm::SDOperand callee,
                llvm::TargetLowering::ArgListTy& args,
                llvm::SelectionDAG& dag);

        virtual const char* getTargetNodeName(unsigned opcode) const;

        llvm::SDOperand LowerRET(llvm::SDOperand op, llvm::SelectionDAG& dag);

        virtual std::pair<unsigned, const TargetRegisterClass*>
            getRegForInlineAsmConstraint(
                const std::string& constraint,
                MVT::ValueType vt) const;

        SDOperand lowerSELECT(SDOperand op, SelectionDAG& dag);
                
        virtual MVT::ValueType getSetCCResultType(const SDOperand &) const {
            return MVT::i1;
        }

    private:
        // Frame index to the the start of variadic parameter list.
        int varArgsFrameIndex_;

        TCETargetMachine& tm_;
    };
}

#endif
