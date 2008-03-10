/**
 * @file TCETargetLowering.h
 *
 * Declaration of TCETargetLowering class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
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
                bool isVarArg,
                unsigned cc,
                bool isTailCall,
                llvm::SDOperand callee,
                llvm::TargetLowering::ArgListTy& args,
                llvm::SelectionDAG& dag);

        virtual llvm::MachineBasicBlock* InsertAtEndOfBasicBlock(
            llvm::MachineInstr* mi,
            llvm::MachineBasicBlock* mbb);

        virtual const char* getTargetNodeName(unsigned opcode) const;

        llvm::SDOperand LowerRET(llvm::SDOperand op, llvm::SelectionDAG& dag);

        //virtual ConstraintType getConstraintType(
        //    char ConstraintLetter) const;

        virtual std::pair<unsigned, const TargetRegisterClass*>
            getRegForInlineAsmConstraint(
                const std::string& constraint,
                MVT::ValueType vt) const;

        /*
        virtual std::vector<unsigned>
            getRegClassForInlineAsmConstraint(
                const std::string& constraint,
                MVT::ValueType vt) const;
        */
        SDOperand lowerSELECT(SDOperand op, SelectionDAG& dag);
                
    private:
        // Frame index to the the start of variadic parameter list.
        int varArgsFrameIndex_;

        TCETargetMachine& tm_;
    };
}

#endif
