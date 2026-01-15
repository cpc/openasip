/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * @author Heikki Kultala 2012 (heikki.kultala-no.spam-tut.fi)
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
#include "Conversion.hh"
#include "MathTools.hh"

#ifdef TARGET64BIT
#define DEFAULT_TYPE MVT::i64
#define MOVE_IMM TCE::MOVI64sa
#else
#define DEFAULT_TYPE MVT::i32
#define MOVE_IMM TCE::MOVI32ri
#endif

using namespace llvm;

class TCEDAGToDAGISel : public llvm::SelectionDAGISel {
public:
    static char ID;

    TCEDAGToDAGISel() = delete;

    explicit TCEDAGToDAGISel(llvm::TCETargetMachine& tm);
    virtual ~TCEDAGToDAGISel();

    bool SelectADDRrr(SDValue N, SDValue &R1, SDValue &R2);
    bool SelectADDRri(SDValue N, SDValue &Base, SDValue &Offset);

    void Select(llvm::SDNode* op) override;

#if LLVM_MAJOR_VERSION < 21
    virtual StringRef getPassName() const override {
        return "TCE DAG->DAG Pattern Instruction Selection";
    }
#endif

private:

    static bool isBroadcast(SDNode *n);
    static bool isConstantBuild(SDNode* n);
    static bool isConstantFPBuild(SDNode* n);
    void SelectOptimizedBuildVector(SDNode* n);
    // returns size of cancat

    EVT getIntegerVectorVT(EVT vt);

    MachineSDNode* createPackNode(SDNode*n, const EVT& vt, int laneCount, SDValue* vals);

    llvm::TCETargetLowering& lowering_;
    const llvm::TCESubtarget& subtarget_;
    llvm::TCETargetMachine* tm_;

#include "TCEGenDAGISel.inc"
};

#if LLVM_MAJOR_VERSION >= 21
class TCEDAGToDAGISelLegacy : public llvm::SelectionDAGISelLegacy {
public:
  static char ID;
  explicit TCEDAGToDAGISelLegacy(llvm::TCETargetMachine &Target);
};

#else
char TCEDAGToDAGISel::ID = 0;
#endif

/**
 * Constructor.
 */
TCEDAGToDAGISel::TCEDAGToDAGISel(TCETargetMachine& tm):
#if LLVM_MAJOR_VERSION < 21
    SelectionDAGISel(ID, tm),
#else
    SelectionDAGISel(tm),
#endif
    lowering_(*static_cast<TCETargetLowering*>(tm.getTargetLowering())),
    subtarget_(*tm.getSubtargetImpl()), tm_(&tm) {
}

/**
 * Destructor.
 */
TCEDAGToDAGISel::~TCEDAGToDAGISel() {
}

// in LLVM 3.9 select() returns void
#define SELECT_NODE_AND_RETURN(args...) \
    CurDAG->SelectNodeTo(args); \
    return
#define RETURN_SELECTED_NODE(node) \
    (void)(node); \
    return


/**
 * Handles custom instruction selections.
 *
 * @param op Operation to select.
 */
void
TCEDAGToDAGISel::Select(SDNode* n) {
    // Custom nodes are already selected
    if (n->isMachineOpcode()) {
        n->setNodeId(-1);
        return;
    }

    SDLoc dl(n);
    if (n->getOpcode() >= ISD::BUILTIN_OP_END &&
        n->getOpcode() < TCEISD::FIRST_NUMBER) {
        // Already selected.
        return;
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
                n, MOVE_IMM, DEFAULT_TYPE,
                CurDAG->getTargetFrameIndex(fi, DEFAULT_TYPE));
        } else {
            auto fiN = CurDAG->getMachineNode(
                MOVE_IMM, dl, DEFAULT_TYPE,
                CurDAG->getTargetFrameIndex(fi, DEFAULT_TYPE));
            ReplaceNode(n, fiN);
            return;
        }
    } else if (n->getOpcode() == ISD::VSELECT ||
               (n->getOpcode() == ISD::SELECT &&
                !n->getOperand(1).getValueType().isVector())) {
        SDNode* node2 = dyn_cast<SDNode>(n->getOperand(0));
        if (node2->getOpcode() == ISD::SETCC) {
            SDValue val1 = n->getOperand(1);
            SDValue val2 = n->getOperand(2);

            SDValue n2val1 = node2->getOperand(0);
            SDValue n2val2 = node2->getOperand(1);

            if (val1 == n2val1 && val2 == n2val2 && node2->hasOneUse()) {
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
                            n,opc, n->getSimpleValueType(0), val1, val2);
                    }
                    break;
                case ISD::SETGT:
                case ISD::SETGE:
                case ISD::SETOGT:
                case ISD::SETOGE: // todo: what is ordered here? nan handling?
                    opc = tm_->getMaxOpcode(n);
                    if (opc != -1) {
                        SELECT_NODE_AND_RETURN(
                            n, opc, n->getSimpleValueType(0), val1, val2);
                    }
                    break;
                case ISD::SETULT:
                case ISD::SETULE:
                    opc = tm_->getMinuOpcode(n);
                    if (opc != -1) {
                        SELECT_NODE_AND_RETURN(
                            n, opc, n->getSimpleValueType(0), val1, val2);
                    }
                    break;
                case ISD::SETUGT:
                case ISD::SETUGE:
                    opc = tm_->getMaxuOpcode(n);
                    if (opc != -1) {
                        SELECT_NODE_AND_RETURN(
                            n, opc, n->getSimpleValueType(0), val1, val2);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    } else if (n->getOpcode() == ISD::SHL ||
               n->getOpcode() == ISD::SRA ||
               n->getOpcode() == ISD::SRL) {
        SDValue shifted = n->getOperand(0);
        SDValue shifter = n->getOperand(1);
        EVT shiftedVt = shifted.getValueType();
        EVT shifterVt = shifter.getValueType();

    } else if (n->getOpcode() == ISD::AND ||
               n->getOpcode() == ISD::OR ||
               n->getOpcode() == ISD::XOR) {
        SDValue lhs = n->getOperand(0);
        SDValue rhs = n->getOperand(1);
        EVT lhsVt = lhs.getValueType();
        EVT rhsVt = rhs.getValueType();
    }

    SelectCode(n);
}
#undef SELECT_NODE_AND_RETURN
#undef RETURN_SELECTED_NODE

/**
 * Handles ADDRri operands.
 */
bool
TCEDAGToDAGISel::SelectADDRri(
    SDValue addr, SDValue& base, SDValue& offset) {

    if (FrameIndexSDNode* fin = dyn_cast<FrameIndexSDNode>(addr)) {
        base = CurDAG->getTargetFrameIndex(fin->getIndex(), DEFAULT_TYPE);
        offset = CurDAG->getTargetConstant(0, SDLoc(), DEFAULT_TYPE);
        return true;
    } else if (addr.getOpcode() == ISD::TargetExternalSymbol ||
               addr.getOpcode() == ISD::TargetGlobalAddress) {

        return false;  // direct calls.
    }
    base = addr;
    offset = CurDAG->getTargetConstant(0, SDLoc(), DEFAULT_TYPE);
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

    r2 = CurDAG->getTargetConstant(0, SDLoc(), DEFAULT_TYPE);
    return true;
}

/**
 * Returns an instance of the instruction selector.
 *
 * @param tm Target machine description.
 */

FunctionPass*
llvm::createTCEISelDag(TCETargetMachine& tm) {
#if LLVM_MAJOR_VERSION < 21
    return new TCEDAGToDAGISel(tm);
#else
    return new TCEDAGToDAGISelLegacy(tm);
#endif
}


#if LLVM_MAJOR_VERSION >= 21

char TCEDAGToDAGISelLegacy::ID = 0;

TCEDAGToDAGISelLegacy::TCEDAGToDAGISelLegacy(TCETargetMachine &tm) :
    SelectionDAGISelLegacy(ID, std::make_unique<TCEDAGToDAGISel>(tm)) {
}

#endif


bool
TCEDAGToDAGISel::isBroadcast(SDNode *n) {
    return TCETargetLowering::isBroadcast(n);
}

bool
TCEDAGToDAGISel::isConstantBuild(SDNode* n) {
    int operandCount = n->getNumOperands();
    for (unsigned i = 1; i <operandCount; i++) {
        SDValue val2 = n->getOperand(i);
        SDNode *n2 = val2.getNode();
        if (n2->getOpcode() != ISD::Constant) {
            return false;
        }
    }
    return true;
}

bool
TCEDAGToDAGISel::isConstantFPBuild(SDNode* n) {
    int operandCount = n->getNumOperands();
    for (unsigned i = 1; i <operandCount; i++) {
        SDValue val2 = n->getOperand(i);
        SDNode *n2 = val2.getNode();
        if (n2->getOpcode() != ISD::ConstantFP) {
            return false;
        }
    }
    return true;
}
