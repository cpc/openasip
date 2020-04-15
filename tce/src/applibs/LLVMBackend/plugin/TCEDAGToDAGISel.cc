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
 * @file TCEDAGToDAGISel.cpp
 *
 * TCE DAG to DAG instruction selector implementation.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#include "tce_config.h"
#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/Compiler.h>
#include <llvm/Support/raw_ostream.h>

#include "TCEDAGToDAGISel.hh"
#include "TCETargetMachine.hh"
#include "TCESubtarget.hh"
#include "TCEISelLowering.hh"

using namespace llvm;

class TCEDAGToDAGISel : public llvm::SelectionDAGISel {
public:
    explicit TCEDAGToDAGISel(llvm::TCETargetMachine& tm);
    virtual ~TCEDAGToDAGISel();

    bool SelectADDRrr(SDValue N, SDValue &R1, SDValue &R2);
    bool SelectADDRri(SDValue N, SDValue &Base, SDValue &Offset);

#ifdef LLVM_OLDER_THAN_3_9
    llvm::SDNode* Select(llvm::SDNode* op) override;
#else
    void Select(llvm::SDNode* op) override;
#endif

#if LLVM_OLDER_THAN_4_0
    virtual const char* getPassName() const override {
#else
    virtual StringRef getPassName() const override {
#endif
        return "TCE DAG->DAG Pattern Instruction Selection";
    }

private:
    llvm::TCETargetLowering& lowering_;
    const llvm::TCESubtarget& subtarget_;
    llvm::TCETargetMachine* tm_;

    #include "TCEGenDAGISel.inc"
};

/**
 * Constructor.
 */
TCEDAGToDAGISel::TCEDAGToDAGISel(TCETargetMachine& tm):
    SelectionDAGISel(tm), 
    lowering_(*static_cast<TCETargetLowering*>(tm.getTargetLowering())), 
    subtarget_(*tm.getSubtargetImpl()), tm_(&tm) {
}

/**
 * Destructor.
 */
TCEDAGToDAGISel::~TCEDAGToDAGISel() {
}

// from Sparc backend:
//void TCEDAGToDAGISel::InstructionSelect() {
    //DEBUG(BB->dump());
//  SelectRoot(*CurDAG);
//  CurDAG->RemoveDeadNodes();
//}


#ifdef LLVM_OLDER_THAN_3_9
#define SELECT_NODE_AND_RETURN(args...) return CurDAG->SelectNodeTo(args)
#else
// in LLVM 3.9 select() returns void
#define SELECT_NODE_AND_RETURN(args...) \
    CurDAG->SelectNodeTo(args); \
    return
#endif

/**
 * Handles custom instruction selections.
 *
 * @param op Operation to select.
 */
#ifdef LLVM_OLDER_THAN_3_9
SDNode*
#else
void
#endif
TCEDAGToDAGISel::Select(SDNode* n) {
    SDLoc dl(n);
    if (n->getOpcode() >= ISD::BUILTIN_OP_END &&
        n->getOpcode() < TCEISD::FIRST_NUMBER) {
        // Already selected.
#ifdef LLVM_OLDER_THAN_3_9
        return NULL;
#else
        return;
#endif
    } else if (n->getOpcode() == ISD::BRCOND) {

        // TODO: Check this. Following IA64 example..
        SDValue chain = n->getOperand(0);
        SDValue cc = n->getOperand(1);

        MachineBasicBlock* dest =
            cast<BasicBlockSDNode>(n->getOperand(2))->getBasicBlock();

        // FIXME? - this creates long branches all the time
        SELECT_NODE_AND_RETURN(
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

        MachineBasicBlock* dest =
            cast<BasicBlockSDNode>(n->getOperand(1))->getBasicBlock();
        SELECT_NODE_AND_RETURN(
	    n, TCE::TCEBR, MVT::Other, CurDAG->getBasicBlock(dest), chain);
    } else if (n->getOpcode() == ISD::FrameIndex) {
        int fi = cast<FrameIndexSDNode>(n)->getIndex();
        if (n->hasOneUse()) {
            SELECT_NODE_AND_RETURN(
                n, TCE::MOVI32ri, MVT::i32,
                CurDAG->getTargetFrameIndex(fi, MVT::i32));
        } else {
#ifdef LLVM_OLDER_THAN_3_9
            return CurDAG->getMachineNode(
                TCE::MOVI32ri, dl, MVT::i32,
                CurDAG->getTargetFrameIndex(fi, MVT::i32));
#else
            auto fiN = CurDAG->getMachineNode(
                TCE::MOVI32ri, dl, MVT::i32,
                CurDAG->getTargetFrameIndex(fi, MVT::i32));
            ReplaceNode(n, fiN);
            return;
#endif
        }
    } else if (n->getOpcode() == ISD::SELECT) {
        SDValue cond = n->getOperand(0);
        if (cond.getValueType() == MVT::i1) {
            SDNode* node2 = dyn_cast<SDNode>(n->getOperand(0));
            if (node2->getOpcode() == ISD::SETCC) {
                SDValue val1 = n->getOperand(1);
                SDValue val2 = n->getOperand(2);
                
                SDValue n2val1 = node2->getOperand(0);
                SDValue n2val2 = node2->getOperand(1);

                if (val1 == n2val1 && val2 == n2val2) {
                    if (node2->hasOneUse()) {
                        int opc;
                        ISD::CondCode cc = cast<CondCodeSDNode>(
                            node2->getOperand(2))->get();
                        
                        switch (cc) {
                        case ISD::SETLT:
                        case ISD::SETLE:
                        case ISD::SETOLT:
                        case ISD::SETOLE:
                            opc = tm_->getMinOpcode(n);
                            if (opc != -1) {
                                SELECT_NODE_AND_RETURN(
                                    n,opc, MVT::i32, val1, val2);
                            }
                            break;
                        case ISD::SETGT:
                        case ISD::SETGE:
                        case ISD::SETOGT:
                        case ISD::SETOGE: // todo: what is ordered here? nan handling?
                            opc = tm_->getMaxOpcode(n);
                            if (opc != -1) {
                                SELECT_NODE_AND_RETURN(
                                    n, opc, MVT::i32, val1, val2);
                            }
                            break;
                        case ISD::SETULT:
                        case ISD::SETULE:
                            opc = tm_->getMinuOpcode(n);
                            if (opc != -1) {
                                SELECT_NODE_AND_RETURN(
                                    n, opc, MVT::i32, val1, val2);
                            }
                            break;
                        case ISD::SETUGT:
                        case ISD::SETUGE:
                            opc = tm_->getMaxuOpcode(n);
                            if (opc != -1) {
                                SELECT_NODE_AND_RETURN(
                                    n, opc, MVT::i32, val1, val2);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
#ifdef LLVM_OLDER_THAN_3_9
    SDNode* res =  SelectCode(n);
    return res;
#else
    SelectCode(n);
#endif
}
#undef SELECT_NODE_AND_RETURN


/**
 * Handles ADDRri operands.
 */
bool
TCEDAGToDAGISel::SelectADDRri(
    SDValue addr, SDValue& base, SDValue& offset) {

    if (FrameIndexSDNode* fin = dyn_cast<FrameIndexSDNode>(addr)) {
        base = CurDAG->getTargetFrameIndex(fin->getIndex(), MVT::i32);
        offset = CurDAG->getTargetConstant(0, SDLoc(), MVT::i32);
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
    offset = CurDAG->getTargetConstant(0, SDLoc(), MVT::i32);
    return true;
}

/**
 * Handles ADDRrr operands.
 */
bool
TCEDAGToDAGISel::SelectADDRrr(
    SDValue addr, SDValue& r1, SDValue& r2) {

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
    r2 = CurDAG->getTargetConstant(0, SDLoc(), MVT::i32);
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
