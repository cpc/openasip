#include "BFCopyRegWithOp.hh"
#include "ProgramOperation.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "Move.hh"
#include "CodeGenerator.hh"
#include "TerminalFUPort.hh"
#include "FunctionUnit.hh"
#include "DataDependenceGraph.hh"

#include <iostream>

using TTAProgram::CodeGenerator;

bool BFCopyRegWithOp::splitMove(BasicBlockNode&) {
    CodeGenerator cGen(targetMachine());
    OperationPool opPool;
    auto& op = opPool.operation("COPY");
    pop_ = std::make_shared<ProgramOperation>(op);

    mn_.move().setSource(cGen.createTerminalFUPort("COPY",2));
    regCopy_->move().setDestination(cGen.createTerminalFUPort("COPY",1));
    pop_->addInputNode(*regCopy_);
    pop_->addOutputNode(mn_);
    mn_.setSourceOperationPtr(pop_);
    regCopy_->addDestinationOperationPtr(pop_);

    for (auto fu : candidateFUs_) {
        TTAProgram::ProgramAnnotation srcCandidate(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
            fu->name());
        mn_.move().addAnnotation(srcCandidate);

        TTAProgram::ProgramAnnotation dstCandidate(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST,
            fu->name());
        regCopy_->move().addAnnotation(dstCandidate);
    }

    // move RAW in edges...
    for (int i = ddg().rootGraph()->inDegree(mn_)-1; i >= 0; i--) {
        auto& e = ddg().rootGraph()->inEdge(mn_,i);
        if (e.isRAW() && !e.headPseudo()) {
            ddg().rootGraph()->moveInEdge(mn_, *regCopy_, e);
        }
    }

    // move antidep edges.
    for (int i = ddg().rootGraph()->outDegree(mn_)-1; i >= 0; i--) {
        auto& e = ddg().rootGraph()->outEdge(mn_,i);
        if (e.isRegisterOrRA() && !e.isRAW() && !e.tailPseudo()) {
            ddg().rootGraph()->moveOutEdge(mn_, *regCopy_, e);
        }
    }


    ddg().connectNodes(*regCopy_, mn_,
                       *(new DataDependenceEdge(
                                              DataDependenceEdge::EDGE_OPERATION,
                                              DataDependenceEdge::DEP_UNKNOWN,
                                              TCEString("COPY"))));
    return true;
}

void BFCopyRegWithOp::undoSplit() {

    mn_.move().removeAnnotations(
        TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC);

    // restore antidep edges.
    for (int i = ddg().rootGraph()->outDegree(*regCopy_)-1; i >= 0; i--) {
        auto& e = ddg().rootGraph()->outEdge(*regCopy_,i);
        if (e.isRegisterOrRA() && !e.isRAW() && !e.tailPseudo()) {
            ddg().rootGraph()->moveOutEdge(*regCopy_, mn_,  e);
        }
    }

    // restore raw edges
    for (int i = ddg().rootGraph()->inDegree(*regCopy_)-1; i >= 0; i--) {
        auto& e = ddg().rootGraph()->inEdge(*regCopy_,i);
        if (e.isRAW() && !e.headPseudo()) {
            ddg().rootGraph()->moveInEdge(*regCopy_, mn_, e);
        }
    }


    pop_->removeOutputNode(mn_);
    pop_->removeInputNode(*regCopy_);
    mn_.move().setSource(regCopy_->move().source().copy());
    mn_.unsetSourceOperation();
    regCopy_->removeDestinationOperation(pop_.get());
    pop_ = ProgramOperationPtr(); // this should delete the obj.

    ddg().disconnectNodes(*regCopy_, mn_);
}
