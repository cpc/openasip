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
 * @file TCEDAGToDAGISel.cpp
 *
 * TCE DAG to DAG instruction selector implementation.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <assert.h>
#include <string>
#include <llvm/Intrinsics.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/Support/Debug.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/Compiler.h>

#include "TCEDAGToDAGISel.hh"
#include "TCETargetMachine.hh"
#include "TCESubtarget.hh"
#include "TCETargetLowering.hh"

#include <iostream>

using namespace llvm;
class TCEDAGToDAGISel : public llvm::SelectionDAGISel {
public:
    TCEDAGToDAGISel(llvm::TCETargetMachine& tm);
    virtual ~TCEDAGToDAGISel();
    bool SelectADDRrr(
        llvm::SDValue op, llvm::SDValue addr,
        llvm::SDValue& r1, llvm::SDValue& r2);

    bool SelectADDRri(
        llvm::SDValue op, llvm::SDValue addr,
        llvm::SDValue& base, llvm::SDValue& offset);

    llvm::SDNode* Select(llvm::SDValue op);
#ifdef LLVM_2_3
    virtual void InstructionSelectBasicBlock(llvm::SelectionDAG& dag);
#else
    virtual void InstructionSelect();
#endif
    virtual const char* getPassName() const {
        return "TCE DAG->DAG Pattern Instruction Selection";
    }

private:
    llvm::TCETargetLowering lowering_;
    const llvm::TCESubtarget& subtarget_;

    #include "TCEGenDAGISel.inc"
};

/**
 * Constructor.
 */
TCEDAGToDAGISel::TCEDAGToDAGISel(TCETargetMachine& tm):
    SelectionDAGISel(lowering_), lowering_(tm),
    subtarget_(tm.getSubtarget<TCESubtarget>()) {
}

/**
 * Destructor.
 */
TCEDAGToDAGISel::~TCEDAGToDAGISel() {
}

#ifdef LLVM_2_3
/** 
 * This callback is invoked by SelectionDAGISel when it has created
 * a SelectionDAG for us to codegen.
 */
void
TCEDAGToDAGISel::InstructionSelectBasicBlock(SelectionDAG& dag) {

    DEBUG(BB->dump());
    
    // Select target instructions for the DAG.
    dag.setRoot(SelectRoot(dag.getRoot()));
    dag.RemoveDeadNodes();

    // Emit machine code to BB.
    ScheduleAndEmitDAG(dag);
}
#else
// from Sparc backend:
void TCEDAGToDAGISel::InstructionSelect() {
  DEBUG(BB->dump());
  
  // Select target instructions for the DAG.
  SelectRoot();
  CurDAG->RemoveDeadNodes();
}
#endif

/**
 * Handles custom instruction selections.
 *
 * @param op Operation to select.
 */
SDNode*
TCEDAGToDAGISel::Select(SDValue op) {

    SDNode* n = op.getNode();

    if (n->getOpcode() >= ISD::BUILTIN_OP_END &&
        n->getOpcode() < TCEISD::FIRST_NUMBER) {
        // Already selected.
        return NULL;
    } else if (n->getOpcode() == ISD::BRCOND) {

        // TODO: Check this. Following IA64 example..
        SDValue chain = n->getOperand(0);
        SDValue cc = n->getOperand(1);
        AddToISelQueue(chain);
        AddToISelQueue(cc);
        MachineBasicBlock* dest =
            cast<BasicBlockSDNode>(n->getOperand(2))->getBasicBlock();

        // FIXME? - this creates long branches all the time
        return CurDAG->SelectNodeTo(
            n, TCE::TCEBRCOND, MVT::Other, cc,
            CurDAG->getBasicBlock(dest), chain);
        //} else if (n->getOpcode() == ISD::SETCC) {
        //SDValue op1 = n->getOperand(0);
        //SDValue op1 = n->getOperand(1);
        //ISD::CondCode cc = cast<CondCodeSDNode>(op.getOperand(2))->get();
        //switch (cc) {
        //default: assert(false && "Unhandled CC");
        //case ISD::SETEQ: {
        //    return dag.getNode(TCE::EQ
        //}
        //}
    } else if (n->getOpcode() == ISD::BR) {
        SDValue chain = n->getOperand(0);
        AddToISelQueue(chain);
        MachineBasicBlock* dest =
            cast<BasicBlockSDNode>(n->getOperand(1))->getBasicBlock();
        return CurDAG->SelectNodeTo(
	    n, TCE::TCEBR, MVT::Other, CurDAG->getBasicBlock(dest), chain);
    } else if (n->getOpcode() == ISD::FrameIndex) {
        int fi = cast<FrameIndexSDNode>(n)->getIndex();
        if (n->hasOneUse()) {
            return CurDAG->SelectNodeTo(
                n, TCE::MOVI32ri, MVT::i32,
                CurDAG->getTargetFrameIndex(fi, MVT::i32));
        } else {
            return CurDAG->getTargetNode(
                TCE::MOVI32ri, MVT::i32,
                CurDAG->getTargetFrameIndex(fi, MVT::i32));
        }
    }

   
    SDNode* res =  SelectCode(op);
    return res;
}

/**
 * Handles ADDRri operands.
 */
bool
TCEDAGToDAGISel::SelectADDRri(
    SDValue op, SDValue addr, SDValue& base, SDValue& offset) {

    if (FrameIndexSDNode* fin = dyn_cast<FrameIndexSDNode>(addr)) {
        base = CurDAG->getTargetFrameIndex(fin->getIndex(), MVT::i32);
        offset = CurDAG->getTargetConstant(0, MVT::i32);
        return true;
    } else if (addr.getOpcode() == ISD::TargetExternalSymbol ||
               addr.getOpcode() == ISD::TargetGlobalAddress) {

        return false;  // direct calls.
        /*
    } else if (addr.getOpcode() == ISD::ADD) {

        if (ConstantSDNode* cn =
            dyn_cast<ConstantSDNode>(addr.getOperand(1))) {

            if (FrameIndexSDNode* fin = 
                dyn_cast<FrameIndexSDNode>(addr.getOperand(0))) {

                base = CurDAG->getTargetFrameIndex(fin->getIndex(), MVT::i32);
                offset = CurDAG->getTargetConstant(cn->getValue(), MVT::i32);
                return true;
            }
        }
        */
    }
    base = addr;
    offset = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
}

/**
 * Handles ADDRrr operands.
 */
bool
TCEDAGToDAGISel::SelectADDRrr(
    SDValue op, SDValue addr, SDValue& r1, SDValue& r2) {

    if (addr.getOpcode() == ISD::TargetGlobalAddress) {
        //r1 = addr.getOperand(0);
        //r2 = CurDAG->getTargetConstant(cn->getValue(), MVT::i32);
        //return true:
        return false;
    } else if (addr.getOpcode() == ISD::TargetExternalSymbol) {
        return false;
    } else if (addr.getOpcode() == ISD::FrameIndex) {
        return false;
    }

    r1 = addr;
    r2 = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
}

/**
 * Returns an instance of the instruction selector.
 *
 * @param tm Target machine description.
 */
FunctionPass*
llvm::createTCEISelDag(TCETargetMachine& tm) {
    return new TCEDAGToDAGISel(tm);
}
