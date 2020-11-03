/**
 * @file Peel2BBLoops.cc
 *
 * This optimizer optimizes some 2-bb loops into
 * 1-BB loop be peeling out the 1st iteration which jumps into the middle
 * of the jump. The resulting 1-BB loop can then be loop-scheduled.
 *
 * @author Heikki Kultala 2016 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include "SimpleIfConverter.hh"
#include "Peel2BBLoops.hh"
#include "ProgramOperation.hh"
#include "MoveNode.hh"
#include "BasicBlock.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "ControlFlowGraph.hh"
#include "Move.hh"
#include "CodeGenerator.hh"
#include "TerminalBasicBlockReference.hh"
#include "TerminalFUPort.hh"
#include "Operation.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "OperationPool.hh"
#include "MachineInfo.hh"

Peel2BBLoops::Peel2BBLoops(
    InterPassData& data, const TTAMachine::Machine& targetMachine) :
    ControlFlowGraphPass(data),
    codeGenerator_(new TTAProgram::CodeGenerator(targetMachine)),
    irm_(NULL), mach_(targetMachine) {}

Peel2BBLoops::BBNodes Peel2BBLoops::testIf2BBLoop(ControlFlowGraph& cfg, BasicBlockNode& bbn) {

    // entry/exit node or wrong number of succs/preds
    if (!bbn.isNormalBB() || cfg.outDegree(bbn) != 2 ||
        cfg.inDegree(bbn) != 2) {
        return false;
    }

    // find possible loop begin node and pre-loop node.
    auto iEdges = cfg.inEdges(bbn);
    BasicBlockNode *loopBegin = NULL;
    BasicBlockNode* preLoop = NULL;
    for (auto e: iEdges) {
        if (e->isFallThroughEdge()) {
            loopBegin = &cfg.tailNode(*e);
        } else {
            preLoop = &cfg.tailNode(*e);
        }
    }
    if (loopBegin == NULL || preLoop == NULL) {
        return false;
    }

    auto oEdges = cfg.outEdges(bbn);
    BasicBlockNode *afterLoop = NULL;
    for (auto e: oEdges) {
        if (e->isFallThroughEdge()) {
            afterLoop = &cfg.headNode(*e);
        } else { // jump to loop begin
            if (&cfg.headNode(*e) != loopBegin) {
                return false;
            }
        }
    }

    if (afterLoop == NULL) {
        return false;
    }

    if (cfg.inDegree(*loopBegin) != 1 || cfg.outDegree(*loopBegin) != 1) {
        return false;
    }

    if (!cfg.outEdge(*loopBegin, 0).isFallThroughEdge() ||
        cfg.inEdge(*loopBegin, 0).isFallThroughEdge()) {
        return false;
    }

    // incoming must not be fall-through.
    if (cfg.outDegree(*preLoop) != 1) {
        return false;
    }

    return BBNodes(preLoop, loopBegin, &bbn, afterLoop);
}



/**
 * Handles a cfg.
 *
 * @param cfg cfg to be optimized.
 */
void
Peel2BBLoops::handleControlFlowGraph(
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine) {
    if (codeGenerator_ == NULL) {
        codeGenerator_ = new TTAProgram::CodeGenerator(targetMachine);
    }

    irm_ = &cfg.instructionReferenceManager();

    for (int i = 0; i < cfg.nodeCount(); i++) {

        // first find the block containing the main body(end) of the loop
        BasicBlockNode& bbn = cfg.node(i);

        if (auto loopBlocks = testIf2BBLoop(cfg, bbn)) {
            peel2BBLoop(cfg, loopBlocks);

            // performing this may change other basic blocks so lets start
            // from beginning(may it?)
            i = 0;
        }
    }
}

void Peel2BBLoops::peel2BBLoop(ControlFlowGraph& cfg, BBNodes& bbns) {
    performCodeMotion(bbns);
    updateCFG(cfg, bbns);
}


void Peel2BBLoops::updateCFG(ControlFlowGraph& cfg, BBNodes& bbns) {
    // pre to loop-end needs to be converted from jump to fall-through,
    // with correct predicate(which is got from the loop node, reversing it
    cfg.disconnectNodes(*bbns.preLoop, *bbns.endLoop);
    auto exitEdge = *cfg.connectingEdges(*bbns.endLoop, *bbns.postLoop).begin();
    auto loopEdge = *cfg.connectingEdges(*bbns.endLoop, *bbns.beginLoop).begin();

    ControlFlowEdge* intoLoop = new ControlFlowEdge(
        loopEdge->edgePredicate(),
        ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);

    ControlFlowEdge* overLoop = new ControlFlowEdge(
        exitEdge->edgePredicate(),
        ControlFlowEdge::CFLOW_EDGE_JUMP);

    cfg.moveOutEdge(*bbns.endLoop, *bbns.beginLoop,  *loopEdge);
    loopEdge->setBackEdge();
    cfg.moveOutEdge(*bbns.endLoop, *bbns.beginLoop,  *exitEdge);
    cfg.connectNodes(*bbns.preLoop, *bbns.beginLoop, *intoLoop);
    cfg.connectNodes(*bbns.preLoop, *bbns.postLoop, *overLoop);

    // old loop end bb
    cfg.deleteNodeAndRefs(*bbns.endLoop);
}


void Peel2BBLoops::performCodeMotion(BBNodes& bbns) {
    // the nasty jump to middle of bb, removed because copying the code
    assert(SimpleIfConverter::removeJump(bbns.preLoop->basicBlock()));

    // then append the end of the loop into the pre-loop BB,
    // and inverse the guard and update the jump destination the post-loop
    appendBB(bbns.endLoop->basicBlock(),
             bbns.preLoop->basicBlock(),
             bbns.postLoop);

    // append end of loop to begin of the loop. no need to update jump.
    appendBB(bbns.endLoop->basicBlock(),
             bbns.beginLoop->basicBlock(),
             nullptr);
}

void Peel2BBLoops::appendBB(
    const TTAProgram::BasicBlock& src, TTAProgram::BasicBlock& dest, BasicBlockNode* newJumpDest) {

    std::map<ProgramOperationPtr,ProgramOperationPtr> poMapping;

    ProgramOperationPtr prevPO = nullptr;
    for (int j = 0; j < src.instructionCount(); j++) {
        const TTAProgram::Instruction& ins = src.instructionAtIndex(j);
        TTAProgram::Instruction* newIns = new TTAProgram::Instruction();
        dest.add(newIns);
        for (int i = 0; i < ins.moveCount(); i++) {
            const TTAProgram::Move& move = ins.move(i);
            auto moveCopy = move.copy();
            MoveNode* newMN = nullptr;

            if (moveCopy->source().isFUPort()) {
                ProgramOperationPtr newPO =
                    SimpleIfConverter::fixTerminalPO(
                        static_cast<TTAProgram::TerminalFUPort&>(
                                moveCopy->source()),
                        poMapping);
                if (newPO != NULL) {
                    prevPO = newPO;
                    newMN = new MoveNode(moveCopy);
                    newPO->addOutputNode(*newMN);
                    newMN->setSourceOperationPtr(newPO);
                }
            }
            if (moveCopy->destination().isFUPort()) {
                ProgramOperationPtr newPO = SimpleIfConverter::fixTerminalPO(
                        static_cast<TTAProgram::TerminalFUPort&>(
                            moveCopy->destination()),
                        poMapping);
                if (newPO != NULL) {
                    prevPO = newPO;
                    if (newMN == nullptr)
                        newMN = new MoveNode(moveCopy);
                    newPO->addInputNode(*newMN);
                    newMN->addDestinationOperationPtr(newPO);
                }
            }

            // for jumps, need to update target and inverse guard.
            if (moveCopy->isJump() && newJumpDest != nullptr) {
                if (!moveCopy->isUnconditional()) {

                    // if fake guard, first try to inverse the op that
                    // generated it, but if it fails negate the guard.
                    if (!(moveCopy->guard().guard().parentBus() == nullptr
                          && negateOp(prevPO))) {

                        moveCopy->setGuard(
                            codeGenerator_->createInverseGuard(
                                moveCopy->guard()));
                    }

                } else { // bz / bnz
                    // TODO: put these into a map?
                    std::map<std::string, std::string> branchNegations =
                        {{"BZ", "BNZ"}, {"BNZ", "BZ"},
                         {"BZ1", "BNZ1"}, {"BNZ1", "BZ1"},
                         {"BEQ", "BNE"}, {"BNE", "BEQ"},
                         {"BGT", "BLE"}, {"BLE", "BGT"},
                         {"BGTU", "BLEU"}, {"BLEU", "BGTU"},
                         {"BLT", "BGE"}, {"BGE", "BLT"},
                         {"BLTU", "BGEU"}, {"BGEU", "BLTU"}
                        };
                    auto *dest = dynamic_cast<TTAProgram::TerminalFUPort*>(
                        &moveCopy->destination());
                    assert(dest);
                    TCEString jumpName = dest->hintOperation().name();
                    if (branchNegations.count(jumpName) > 0) {
                        OperationPool pool;
                        std::string negatedOpName = branchNegations[jumpName];
                        const Operation& negatedOp =
                            pool.operation(negatedOpName.c_str());
                        newMN->destinationOperation().setOperation(negatedOp);
                    } else {
                        assert (false &&
                                "Cannot neg unknown conditional jump instr");
                    }
                }
                moveCopy->setSource(
                    new TTAProgram::TerminalBasicBlockReference(
                        newJumpDest->basicBlock()));
            }
            newIns->addMove(moveCopy);
        }
    }
}

/**
 * Negates operation into operation which generates opposite
 * predicate value.
 */
bool Peel2BBLoops::negateOp(ProgramOperationPtr po) {

    // TODO: Shouldn't these be CAPSed? does this really work?
    std::map<TCEString, TCEString> negateOps;
    negateOps["eq"] = "ne";
    negateOps["ne"] = "eq";
    negateOps["le"] = "gt";
    negateOps["gt"] = "le";
    negateOps["lt"] = "ge";
    negateOps["ge"] = "lt";
    negateOps["leu"] = "gtu";
    negateOps["gtu"] = "leu";
    negateOps["ltu"] = "geu";
    negateOps["geu"] = "ltu";

    const Operation& op = po->operation();
    std::cerr << "negating op: " << op.name() << std::endl;
    auto i = negateOps.find(op.name());
    if (i == negateOps.end()) {
        std::cerr << "negated op for: " << op.name() << " not found!" << std::endl;
        return false;
    }
    std::cerr << "negated opname: " << i->second << std::endl;

    if (!MachineInfo::supportsOperation(mach_, i->second)) {
        return false;
    }
    OperationPool pool;
    const Operation& negatedOp = pool.operation(i->second.c_str());
    po->setOperation(negatedOp);
    return true;
}
