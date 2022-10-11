/**
 * @file SimpleIfConverter.cc
 *
 * Implementation of if converter optimizer class.
 *
 * This does if conversion for simple control structures
 * where only one predicate is used.
 *
 * @author Heikki Kultala 2008 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include <cmath>
#include <iostream>

#include "SimpleIfConverter.hh"

#include "Move.hh"
#include "Instruction.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "ControlFlowGraph.hh"
#include "CodeGenerator.hh"
#include "NullInstruction.hh"
#include "Procedure.hh"
#include "InstructionReference.hh"
#include "UniversalMachine.hh"
#include "SchedulerCmdLineOptions.hh"
#include "MachineAnalysis.hh"
#include "ControlUnit.hh"
#include "TerminalFUPort.hh"
#include "MoveNode.hh"
#include "InstructionReferenceManager.hh"
#include "Program.hh"
#include "BasicBlock.hh"

using TTAProgram::Move;
using TTAProgram::MoveGuard;
using TTAProgram::Instruction;
using TTAProgram::Terminal;
using TTAProgram::TerminalRegister;
using TTAProgram::CodeGenerator;
using TTAMachine::RegisterGuard;
using TTAMachine::Guard;
using TTAMachine::RegisterFile;

class UniversalMachine;
class InterPassData;

/**
 * Constructor
 *
 * @param InterPassData InterPassData. Not used.
 * @param dsLimit diamond-shape size limit
 * @param tsLimit1 triangle-shape size limit(convert fall-thru)
 * @param tsLimit2 triangle-shape size limit(convert jump)
 */
SimpleIfConverter::SimpleIfConverter(
    InterPassData& data, const TTAMachine::Machine& targetMachine) :
    ControlFlowGraphPass(data), ProcedurePass(data),
    ProgramPass(data), codeGenerator_(NULL), irm_(NULL),
    diamonds_(0), diamonds2_(0), triangles1_(0), triangles2_(0),
    grAborts_(0), grDefAborts_(0), grUseAborts_(0),
    loopAborts_(0), uncondAborts_(0) , sizeAborts_(0), succAborts_(0),
    diamondSizeLimit_(-1), triangleSizeLimit1_(-1), 
    triangleSizeLimit2_(-1) {

    // allow overriding the thresholds using a scheduler command line switch
    SchedulerCmdLineOptions* opts = 
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    if (opts != NULL && opts->ifConversionThreshold() > -1) {
        diamondSizeLimit_ = opts->ifConversionThreshold();
        triangleSizeLimit1_ = opts->ifConversionThreshold();
        triangleSizeLimit2_ = opts->ifConversionThreshold();
    } else if (diamondSizeLimit_ == -1 ||
               triangleSizeLimit1_ == -1 ||
               triangleSizeLimit2_ == -1) {

        // fall back to an heuristics to determine proper limits for
        // if-conversion according to the available ILP in the target
        // machine

        MachineAnalysis ma(targetMachine);

        // rf ports can be shared by ops with neg guards, so 
        // rfilp * 2 on this heuristic.
        float diamondOperations = 
            pow(pow(ma.busILP(), -2) + 
                pow((ma.fuILP()*2), -2) +
                pow((ma.bypassedRfILP()*2), -2), -0.5)
            * (1+targetMachine.controlUnit()->delaySlots());
        
        // on triangle structures there is no moves with opposite guards,
        // use the default averageilp
        float triangleOperations = ma.averageILP()
            * (1+targetMachine.controlUnit()->delaySlots());

        if (diamondSizeLimit_ == -1) {
            diamondSizeLimit_ = int(diamondOperations*2.25);
        }
        if (triangleSizeLimit1_ == -1) {
            triangleSizeLimit1_ = int(triangleOperations*1.5);
        }
        if (triangleSizeLimit2_ == -1) {
            triangleSizeLimit2_ = int(triangleOperations*1.5);
        }
    }

    if (Application::verboseLevel() > 2) {
        Application::logStream()
            << "if-conversion thresholds:" << std::endl
            << "  diamond: " << diamondSizeLimit_ << std::endl
            << "triangle1: " << triangleSizeLimit1_ << std::endl
            << "triangle2: " << triangleSizeLimit2_ << std::endl;
    }

}

/**
 * Tells what this scheduler pass does
 */
std::string
SimpleIfConverter::shortDescription() const {
    return std::string("Simple if converter");
}

/**
 * Handles a cfg. Does if conversion for the cfg.
 *
 * @param cfg cfg to be if-converted.
 */
void
SimpleIfConverter::handleControlFlowGraph(
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine) {
    SchedulerCmdLineOptions* opts = 
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    const bool printCFGs = opts != NULL && opts->dumpIfConversionCFGs();

    if (printCFGs) 
        cfg.writeToDotFile(cfg.name() + ".cfg.before_ifc.dot");

    if (codeGenerator_ == NULL) {
        codeGenerator_ = 
            new CodeGenerator(targetMachine);
    }

    irm_ = &cfg.instructionReferenceManager();

    while (true) {
    // do thru cfg..
        CandidateBlocks* bblocks = searchCandidate(cfg);
        if (bblocks == NULL) {
            break;
        } else {
            convert(*bblocks, cfg);
            delete bblocks;
        }
    }

    if (Application::verboseLevel() > 2) {
        Application::logStream() <<
            "Converted: " << std::endl <<
            "\tDiamonds: " << diamonds_ << std::endl <<
            "\tDiamonds(2): " << diamonds2_ << std::endl <<
            "\tTriangles(1): " << triangles1_ << std::endl <<
            "\tTriangles(2): " << triangles2_ << std::endl <<

            "Aborts: " << std::endl <<
            "\tGrDef: " << grDefAborts_ << std::endl <<
            "\tAlready guarded: " << grUseAborts_ << std::endl <<
            "\tLoop Aborts: " << loopAborts_ << std::endl <<
            "\tUncond jump: " << uncondAborts_ << std::endl <<
            "\tSize: " << sizeAborts_ << std::endl <<
            "\tSucc unknown aborts: " << succAborts_ << std::endl;
    }

    if (printCFGs)
        cfg.writeToDotFile(cfg.name() + ".cfg.after_ifc.dot");
}

/**
 * Handles a procedure. Does if conversion for the procedure.
 *
 * @param procedure procedure to be if-converted.
 * @param targetMachine machine for which to be compiled.
 */
void
SimpleIfConverter::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine) {
    irm_ = &procedure.parent().instructionReferenceManager();
    ControlFlowGraph cfg(procedure);

    cfg.updateReferencesFromProcToCfg();

    handleControlFlowGraph(cfg, targetMachine);

    cfg.copyToProcedure(procedure);
}

/**
 * Handles a program. Does if conversion for the whole program.
 *
 * @param program program to be if-converted.
 * @param targetMachine machine for which to be compiled.
 */
void
SimpleIfConverter::handleProgram(
    TTAProgram::Program& program, const TTAMachine::Machine& targetMachine) {
    if (codeGenerator_ == NULL) {
        codeGenerator_ = new CodeGenerator(targetMachine);
    }

    ProgramPass::executeProcedurePass(program, targetMachine, *this);

    delete codeGenerator_; codeGenerator_ = NULL;
}

/**
 * Searches a single area of basic blocs to be converted at once.
 *
 * Conversion should always be possible for blocks this returns,
 * All checks are done here.
 *
 * This always returnsa the first area of blocks that can be
 * converted. If actual conversion is not done/cfg is not changed,
 * returns the same are again and again.
 *
 * @param cfg ControlFlowGraph.
 */
SimpleIfConverter::CandidateBlocks*
SimpleIfConverter::searchCandidate(ControlFlowGraph& cfg) {
    for (int i = 0; i < cfg.nodeCount(); i++) {

        BasicBlockNode& bbn = cfg.node(i);

        // entry/exit node or too many successors
        if (!bbn.isNormalBB() || cfg.outDegree(bbn) != 2) {
            continue;
        }

        std::pair<BasicBlockNode*,BasicBlockNode*> nodes =
	    successors(bbn, cfg);

        BasicBlockNode* fallThruNode = nodes.first;
        BasicBlockNode* jumpDestNode = nodes.second;

        if (fallThruNode == NULL || jumpDestNode == NULL ||
            !fallThruNode->isNormalBB() || !jumpDestNode->isNormalBB()) {
            continue;
        }

        CandidateBlocks* cb =
            detectDiamond(bbn, *fallThruNode, *jumpDestNode, cfg);
        if (canConvert(cb, cfg)) {
            diamonds_++;
            return cb;
        }
        if (cb == NULL) {
            cb = detectTriangleViaJump(bbn, *fallThruNode, *jumpDestNode, cfg);
            if (canConvert(cb, cfg)) {
                triangles2_++;
                return cb;
            }
        }
        if (cb == NULL) {
            cb = detectTriangleViaFt(bbn, *fallThruNode, *jumpDestNode, cfg);
            if (canConvert(cb, cfg)) {
                triangles1_++;
                return cb;
            }
        }
        // some canconvert failed.
        if (cb != NULL) {
            delete cb;
        }
    }
    return NULL;
}

/**
 * Checks whether if conversion is possible for given basic blocks.
 *
 * @param candidates Data about area being converted.
*/
bool
SimpleIfConverter::canConvert(
    CandidateBlocks* candidates, ControlFlowGraph& cfg) {
    if (candidates == NULL) {
        return false;
    }
    assert(candidates->firstBB_.instructionCount() > 0);
    TTAProgram::Instruction* jumpIns =
        &candidates->firstBB_.lastInstruction();
    assert(jumpIns->moveCount()==1);
    TTAProgram::Move* jumpMove = &jumpIns->move(0);
    assert(jumpMove->source().isInstructionAddress() ||
           jumpMove->source().isBasicBlockReference());
    if (jumpMove->isUnconditional()) {
        jumpIns = &candidates->firstBB_.previousInstruction(*jumpIns);
        jumpMove = &jumpIns->move(0);
        assert(jumpMove->source().isInstructionAddress() ||
               jumpMove->source().isBasicBlockReference());
    }

    // check that the jump is conditional.
    // broken cfg may lead it to be unconditional
    if(jumpMove->isUnconditional()) {
        uncondAborts_++;
        return false;
    }

    // Cannot convert if there are not register guards.
    if (!jumpMove->guard().guard().parentBus()) {
        return false;
    }
    candidates->guard_ = jumpMove->guard().copy();

    // find the guard reg
    const Guard &g = candidates->guard_->guard();
    const RegisterGuard* rg = dynamic_cast<const RegisterGuard*>(&g);

    candidates->invg_ =
        codeGenerator_->createInverseGuard(*candidates->guard_);

    // if something fails on guad generation or extraction
    // caused for example by port guards or missing inverse guard.
    if (rg == NULL || candidates->invg_ == NULL) {
        grAborts_++;
        return false;
    }
    candidates->grIndex_ = rg->registerIndex();
    candidates->grFile_ = rg->registerFile();

    // if converting fall-thru-node
    if (candidates->joinNode_ != &candidates->fallThruNode_ &&
        candidates->succNode1_ != &candidates->fallThruNode_) {
        bool lastToConvert = &candidates->fallThruBB_ == candidates->joinBB_;
        if (writesRegister(
                candidates->fallThruBB_, candidates->grIndex_,
                *candidates->grFile_, lastToConvert)) {
            grDefAborts_++;
#if 0
            PRINT_VAR(candidates->fallThruNode_.nodeID());
            PRINT_VAR(candidates->grIndex_);
            PRINT_VAR(candidates->grFile_);
            
            Application::logStream()
                << candidates->fallThruBB_.disassembly()
                << std::endl;
#endif            
            return false;
        }
        // cannot have double guards
        if (hasConditionals(candidates->fallThruBB_)) {
            grUseAborts_++;
            return false;
        }
    }

    // if converting jump node
    if (candidates->joinNode_ != &candidates->jumpDestNode_ &&
        candidates->succNode1_ != &candidates->jumpDestNode_) {

        bool lastToConvert = &candidates->fallThruBB_ != candidates->joinBB_;
        if (writesRegister(
                candidates->jumpDestBB_, candidates->grIndex_,
                *candidates->grFile_, lastToConvert)) {
            grDefAborts_++;
            return false;
        }
        // cannot have double guards
        if (hasConditionals(candidates->jumpDestBB_)) {
            grUseAborts_++;
            return false;
        }
    }

    // Do not allow backwards fall throughs from the last BB to the first!
    // TODO: some day convert these to jumps. For now, just give up when
    // encountering these.
    ControlFlowGraph::EdgeSet outEdges = cfg.outEdges(candidates->lastNode_);
    for (auto o: outEdges) {
        if (!o->isJumpEdge() &&
            &cfg.headNode(*o) == &candidates->firstNode_) {
            return false;
        }
    }

    // no reason why not
    return true;

}

/**
 * Do the if conversion for given area.
 * All checks are already done, this must not fail.
 *
 * @param bblocks The data about blocks being merged
 * @param cfg ControlFlowGraph.
 */
void
SimpleIfConverter::convert(CandidateBlocks& bblocks, ControlFlowGraph& cfg) {

    // combines blocks
    combineBlocks(bblocks);

    // update cfg about the changes
    updateCfg(bblocks, cfg);
}

/**
 * Updates CFG after if conversion
 *
 * @param bblocks The data about blocks being merged
 * @param cfg The cfg to update
 */
void
SimpleIfConverter::updateCfg(CandidateBlocks& bblocks, ControlFlowGraph& cfg) {

    // these should not be needed but just for sure.
    cfg.disconnectNodes(bblocks.firstNode_, bblocks.jumpDestNode_);
    cfg.disconnectNodes(bblocks.firstNode_, bblocks.fallThruNode_);

//    assert(cfg.outDegree(bblocks.firstNode_) == 0);
    if (bblocks.joinNode_ != NULL) {
        if (bblocks.joinNode_ != &bblocks.jumpDestNode_ && bblocks.removeJd_) {
            cfg.disconnectNodes(bblocks.jumpDestNode_, *bblocks.joinNode_);
        }
        if (bblocks.joinNode_ != &bblocks.fallThruNode_ && bblocks.removeFt_) {
            cfg.disconnectNodes(bblocks.fallThruNode_, *bblocks.joinNode_);
        }
    }

    // copy out edges
    ControlFlowGraph::EdgeSet outEdges = cfg.outEdges(bblocks.lastNode_);
    for (ControlFlowGraph::EdgeSet::iterator i = outEdges.begin();
         i != outEdges.end(); i++) {
        BasicBlockNode& bbn = cfg.headNode(**i);
        ControlFlowEdge oldEdge = **i;

        // some fall-thru's may become jumps.
        ControlFlowEdge::CFGEdgeType eType = bblocks.createJump_ ?
            ControlFlowEdge::CFLOW_EDGE_JUMP : (*i)->edgeType();
        
        ControlFlowEdge* cfe = new ControlFlowEdge(
            (*i)->edgePredicate(), eType);

        if (oldEdge.isBackEdge()) {
            cfe->setBackEdge();
        }

        cfg.connectNodes(bblocks.firstNode_, bbn, *cfe);

        // if the original join node is being deleted, remove edges
        if (bblocks.removeJoin_) {
            cfg.removeEdge(oldEdge);
        }
    }

    if (bblocks.joinNode_ != &bblocks.fallThruNode_ &&
        bblocks.succNode1_ != &bblocks.fallThruNode_ && bblocks.removeFt_) {
        //assert(cfg.inDegree(bblocks.fallThruNode_) == 0);
        // remove nodes from CFG.
        cfg.deleteNodeAndRefs(bblocks.fallThruNode_);
    }

    if (bblocks.joinNode_ != &bblocks.jumpDestNode_ &&
        bblocks.succNode1_ != &bblocks.jumpDestNode_ && bblocks.removeJd_) {
        //assert(cfg.inDegree(bblocks.jumpDestNode_) == 0);
        cfg.deleteNodeAndRefs(bblocks.jumpDestNode_);
    }

    if (bblocks.joinNode_ != NULL) {
        if (bblocks.removeJoin_) {
            //assert(cfg.inDegree(*bblocks.joinNode_) == 0);
            cfg.deleteNodeAndRefs(*bblocks.joinNode_);
        }
    }
}


/**
 * Combines many basic blocks into one and sets the guards
 * accordingly.
 *
 * @param bblocks All the data needed for the operation
 */
void
SimpleIfConverter::combineBlocks(CandidateBlocks& bblocks) {

    // TODO: should not set guard of jump of lastbb.

    // remove the branch jump. 
    assert(removeJump(bblocks.firstBB_));

    // fall thru node handling.
    if (&bblocks.fallThruNode_ != bblocks.succNode1_) {

        if (&bblocks.fallThruBB_ != bblocks.joinBB_) {
            appendBB(
                bblocks.fallThruBB_, bblocks.firstBB_, bblocks.invg_,
                &bblocks.fallThruBB_ != &bblocks.lastBB_);
        }
    }

    // jump dest node handling
    if (&bblocks.jumpDestNode_ != bblocks.succNode1_) {

        if (&bblocks.jumpDestBB_ != bblocks.joinBB_) {
            appendBB(bblocks.jumpDestBB_, bblocks.firstBB_, bblocks.guard_,
                     &bblocks.jumpDestBB_ != &bblocks.lastBB_ ||
                     (bblocks.succNode2_ == NULL &&
                      bblocks.succNode1_->isNormalBB()));
        }
    }

    if (bblocks.joinBB_ != NULL) {
        appendBB(*bblocks.joinBB_, bblocks.firstBB_, NULL, false);
    }

    bool isLastUncondJump = false;
    if (bblocks.firstBB_.instructionCount() != 0) {
        TTAProgram::Instruction& last = bblocks.firstBB_.lastInstruction();
        for (int i = 0; i < last.moveCount(); i++) {
            TTAProgram::Move& move = last.move(i);
            if (move.isUnconditional() && move.isJump()) {
                isLastUncondJump = true;
            }
        }
    }

    // handle the last jump which may need to be added.
    if  (bblocks.succNode1_->isNormalBB() && 
         !isLastUncondJump && !bblocks.removeJoin_) {
        bblocks.createJump_ = true;
        addJump(bblocks.firstBB_, *bblocks.succNode1_);
    }
}

/**
 *  Checks whether there exists any conditional moves in given BB.
 *
 * @param bb BasicBlock where to check for conditional moves.
 * @return true if some move is conditional
*/
bool
SimpleIfConverter::hasConditionals(TTAProgram::BasicBlock& bb) {
    for (int j = 0; j < bb.instructionCount(); j++) {
        Instruction& ins = bb.instructionAtIndex(j);
        for (int i = 0; i < ins.moveCount(); i++) {
            if (!ins.move(i).isUnconditional()) {
                return true;
            }
        }
    }
    return false;
}

ProgramOperationPtr
SimpleIfConverter::fixTerminalPO(TTAProgram::TerminalFUPort& terminal, 
              std::map<ProgramOperationPtr,ProgramOperationPtr>& poMapping) {
    
    ProgramOperationPtr po = terminal.programOperation();
    if (po == NULL) {
        return ProgramOperationPtr();
    }
    std::map<ProgramOperationPtr,ProgramOperationPtr>::iterator i = 
        poMapping.find(po);
    
    if (i == poMapping.end()) {
        // create new programOperation
        ProgramOperationPtr newPO(
            new ProgramOperation(terminal.hintOperation(), po->machineInstr()));
        poMapping[po] = newPO;
        terminal.setProgramOperation(newPO);
        return newPO;
    } else {
        terminal.setProgramOperation(i->second);
        return i->second;
    }
}

    
void SimpleIfConverter::appendBB(
    const TTAProgram::BasicBlock& src, TTAProgram::BasicBlock& dest,
    MoveGuard* mg, bool removeJumps) {

    std::map<ProgramOperationPtr,ProgramOperationPtr> poMapping;
        
    for (int j = 0; j < src.instructionCount(); j++) {
        const Instruction& ins = src.instructionAtIndex(j);
        Instruction* newIns = new Instruction();
        dest.add(newIns);
        for (int i = 0; i < ins.moveCount(); i++) {
            const TTAProgram::Move& move = ins.move(i);
            if (!(move.isJump() && removeJumps)) {
                auto moveCopy = move.copy();
                if (moveCopy->source().isFUPort()) {
                    ProgramOperationPtr newPO =
                        fixTerminalPO(
                            static_cast<TTAProgram::TerminalFUPort&>(
                                moveCopy->source()),
                            poMapping);
                    if (newPO != NULL) {
                        MoveNode* mn = new MoveNode(moveCopy);
                        newPO->addOutputNode(*mn);
                        mn->setSourceOperationPtr(newPO);
                    }
                }
                if (moveCopy->destination().isFUPort()) {
                    ProgramOperationPtr newPO =  fixTerminalPO(
                        static_cast<TTAProgram::TerminalFUPort&>(
                            moveCopy->destination()),
                        poMapping);
                    if (newPO != NULL) {
                        MoveNode* mn = new MoveNode(moveCopy);
                        newPO->addInputNode(*mn);
                        mn->addDestinationOperationPtr(newPO);
                    }
                }
                
                if (!moveCopy->isReturn() && mg != NULL) {
                    moveCopy->setGuard(mg->copy());
                }
                newIns->addMove(moveCopy);
            }
        }
    }
}


/**
 * Checks whether a given register is written in given BB.
 *
 * @param bb BasicBlock where to check for register writes
 * @param index index of the register in a registerfile
 * @param rf register file of the register
 * @return true if there exists a write to given register
 */
bool
SimpleIfConverter::writesRegister(
    const TTAProgram::BasicBlock& bb, int index, const RegisterFile& rf,
    bool ignoreLastInstruction) {
    // check that jump does not mess the guard reg.
    // TODO: if could do register renaming to counter this?
    int iCount = ignoreLastInstruction ? 
        bb.instructionCount() -1 :
        bb.instructionCount();
    for (int i = 0; i < iCount; i++) {
        TTAProgram::Instruction& ins = bb.instructionAtIndex(i);
        // should be only 1 move / ins. but loop to be sure.
        for (int j = 0; j < ins.moveCount(); j++) {
            TTAProgram::Move& move = ins.move(j);
            Terminal& dest = move.destination();
            // if writes to the guard reg?
            if (dest.isGPR()) {
                TerminalRegister& tr = dynamic_cast<TerminalRegister&>(dest);
                if (tr.index() == index && &tr.registerFile() == &rf) {
                    // simple solution is to disallow this.
                    // moderate solution is o allow only for last
                    // best solution is to allow this for one that can be last
                    return true;
                }
            }
        }
    }
    return false;
}
/**
 * Tries to remove a jump from the end of a basic block.
 *
 * @param bb basic block where to remove the jump from
 * @return true if removed, false if not removed
 */
bool
SimpleIfConverter::removeJump(TTAProgram::BasicBlock& bb) {

    for (int i = bb.instructionCount() -1 ; i >= 0; i--) {
        Instruction* jumpIns = &bb.lastInstruction();
        if (jumpIns->moveCount() != 0) {
            if (jumpIns->hasJump()) {
                Move *move = &jumpIns->move(0);
                jumpIns->removeMove(*move);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

/**
 * Adds a jump at the end of a basic block.
 *
 * This may create an dangling instr reference?
 *
 * @param bb basicBlock add the jump at end of this basic block.
 * @param dest destination of the jump.
 */
void SimpleIfConverter::addJump(
    TTAProgram::BasicBlock& bb, BasicBlockNode& dest) {
    Instruction* ins = new Instruction;
    TTAProgram::InstructionReference ir =
        irm_->createReference(dest.basicBlock().firstInstruction());

    ins->addMove(
        codeGenerator_->createJump(ir));

    bb.add(ins);
}

/**
 * Returns 2 successors blocks of given block in specified order.
 *
 * First contains the unconditional successor.
 * Second contains the jumo target of conditional jump if exists, or null.
 * If cannot analyze, returns both nulls.
 *
 * @param node Node whose successors to check
 * @param cfg cfg where the successors are searched.

 */
std::pair<BasicBlockNode*,BasicBlockNode*>
SimpleIfConverter::successors(
    BasicBlockNode& node, ControlFlowGraph& cfg) {
    ControlFlowGraph::NodeSet succs = cfg.successors(node);
    if (succs.size() == 1) {
        return std::pair<BasicBlockNode*,BasicBlockNode*>(*succs.begin(),NULL);
    }
    if (succs.size() != 2) {
	Application::logStream()
	    << "Warning: Successor cound of node: " << node.toString()
	    << " is: " << succs.size() << std::endl;
        cfg.writeToDotFile("IfConverterInvalidNumberOfSuccessors_cfg.dot");
    }

    if (node.basicBlock().instructionCount() == 0) {
        return std::pair<BasicBlockNode*,BasicBlockNode*>(NULL,NULL);
    }

    assert(succs.size() == 2);
    ControlFlowEdge& edge = cfg.outEdge(node,0);

    TTAProgram::Instruction* jumpIns =
	&node.basicBlock().lastInstruction();
    TTAProgram::Move* jumpMove = &jumpIns->move(0);
    if (!jumpMove->isJump()) {
        return std::pair<BasicBlockNode*,BasicBlockNode*>(NULL,NULL);
    }

    if (jumpMove->isUnconditional()) {
        jumpIns = &node.basicBlock().previousInstruction(*jumpIns);
        if (jumpIns == NULL ||
            jumpIns == &TTAProgram::NullInstruction::instance()) {
            uncondAborts_++;
            return std::pair<BasicBlockNode*,BasicBlockNode*>(NULL,NULL);
        }
        jumpMove = &jumpIns->move(0);
        if (jumpMove->isUnconditional() || !jumpMove->isJump()) {
            uncondAborts_++;
            return std::pair<BasicBlockNode*,BasicBlockNode*>(NULL,NULL);
        }
    }

    if (edge.isTrueEdge() == jumpMove->guard().guard().isInverted()) {
        return std::pair<BasicBlockNode*,BasicBlockNode*>(
            &cfg.headNode(edge),
            &cfg.headNode(cfg.outEdge(node,1)));
    } else {
        return std::pair<BasicBlockNode*,BasicBlockNode*>(
	    &cfg.headNode(cfg.outEdge(node,1)),
	    &cfg.headNode(edge));
    }
}

SimpleIfConverter::CandidateBlocks*
SimpleIfConverter::detectDiamond(BasicBlockNode& bbn,
                                 BasicBlockNode& fallThruNode,
                                 BasicBlockNode& jumpDestNode,
                                 ControlFlowGraph& cfg) {

    if (cfg.outDegree(fallThruNode) != 1 ||
        cfg.outDegree(jumpDestNode) != 1) {
        return NULL;
    }

    ControlFlowEdge& jdse = cfg.outEdge(jumpDestNode,0);
    ControlFlowEdge& ftse = cfg.outEdge(fallThruNode,0);

    BasicBlockNode& jdSucc = cfg.headNode(jdse);
    BasicBlockNode& ftSucc = cfg.headNode(ftse);

    if (jdse.isCallPassEdge() || 
        ftse.isCallPassEdge() || 
        &jdSucc != &ftSucc ||
        jdse.isBackEdge() ||
        ftse.isBackEdge()) {
        return NULL;
    }

    if (&jdSucc == &bbn) {
        loopAborts_++;
        return NULL;
    }
    //not desirable if nodes are too big.
    if (fallThruNode.basicBlock().instructionCount() >
        diamondSizeLimit_ ||
        jumpDestNode.basicBlock().instructionCount() >
        diamondSizeLimit_) {
        sizeAborts_++;
        return NULL;
    }

    bool keepFt = (cfg.inDegree(fallThruNode) != 1);
    bool keepJd = (cfg.inDegree(jumpDestNode) != 1);

    // temporary fix as these are hard for cfg updates
    if (keepFt || keepJd) {
        return NULL;
    }

    if (!jdSucc.isNormalBB()) {
        // last BB of diamond is exit node?
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, 
            jumpDestNode, NULL, &jdSucc, NULL, 
            false, !keepFt, !keepJd);
    }
    
    // find successors.
    std::pair<BasicBlockNode*,BasicBlockNode*> succ =
        successors(jdSucc, cfg);

    if (succ.first == NULL) {
        succAborts_++;
        return NULL;
    }

    bool keepJoin = (keepJd | keepFt) || cfg.inDegree(jdSucc) > 2 ||
        !jdSucc.isNormalBB();

    // Cannot if-convert code where possibility of fall-through loop.
    if (jdSucc.nodeID() < bbn.nodeID()) {
        keepJoin = true;
    }

    if (keepJoin) {
        ControlFlowGraph::EdgeSet joinOutEdges = cfg.outEdges(jdSucc);
        for (ControlFlowGraph::EdgeSet::iterator i = joinOutEdges.begin();
             i != joinOutEdges.end(); i++) {
           ControlFlowEdge& e = **i;
           if (e.edgePredicate() != ControlFlowEdge::CFLOW_EDGE_NORMAL ||
               e.edgeType() != ControlFlowEdge::CFLOW_EDGE_JUMP) {
               // then do not contain last BB in the diamond
               return new CandidateBlocks(
                   bbn, fallThruNode, jumpDestNode, jumpDestNode,
                   NULL, &jdSucc, NULL, false,  !keepFt, !keepJd);
           }
        }
    }

    return new CandidateBlocks(
        bbn, fallThruNode, jumpDestNode, jdSucc, &jdSucc,
        succ.first, succ.second, !keepJoin, !keepFt, !keepJd);
}


SimpleIfConverter::CandidateBlocks*
SimpleIfConverter::detectTriangleViaJump(
    BasicBlockNode& bbn,
    BasicBlockNode& fallThruNode,
    BasicBlockNode& jumpDestNode,
    ControlFlowGraph& cfg) {

    if (cfg.outDegree(jumpDestNode) != 1) {
        return NULL;
    }
    ControlFlowEdge& jdse = cfg.outEdge(jumpDestNode,0);
    BasicBlockNode& jdSucc = cfg.headNode(jdse);
    if (&jdSucc != &fallThruNode || jdse.isBackEdge()) {
        return NULL;
    }

    // find successors.
    std::pair<BasicBlockNode*,BasicBlockNode*> succ =
        successors(fallThruNode, cfg);

    bool keepJd = (cfg.inDegree(jumpDestNode) != 1);
    bool keepFt = keepJd|(cfg.inDegree(fallThruNode) > 2);
    //assert(cfg.inDegree(fallThruNode) > 1);

    // temporary fix as these are hard for cfg updates
    if (keepFt || keepJd) {
        return NULL;
    }

    // Cannot if-convert code where possibility of fall-through loop.
    if ((succ.first != NULL && succ.first->nodeID() < bbn.nodeID()) ||
        (succ.second != NULL && succ.second->nodeID() < bbn.nodeID())) {
        return NULL;
    }

    // we have a triangle first->jump->ft(branch)

    //not desirable if jump dest node is big.
    if (jumpDestNode.basicBlock().instructionCount() >
        triangleSizeLimit2_) {
        sizeAborts_++;
        return NULL;
    }

    if (succ.first == NULL) {
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, fallThruNode,
            NULL, succ.first, succ.second, !keepFt,
            !keepFt, !keepJd);
    } else {
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, fallThruNode,
            &fallThruNode, succ.first, succ.second, !keepFt,
            !keepFt, !keepJd);
    }
    return NULL;
}


SimpleIfConverter::CandidateBlocks*
SimpleIfConverter::detectTriangleViaFt(
    BasicBlockNode& bbn,
    BasicBlockNode& fallThruNode,
    BasicBlockNode& jumpDestNode,
    ControlFlowGraph& cfg) {

    if (cfg.outDegree(fallThruNode) != 1) {
        return NULL;
    }
    ControlFlowEdge& e = cfg.outEdge(fallThruNode,0);
    if (e.isCallPassEdge() || e.isBackEdge() ||
        &cfg.headNode(e) != &jumpDestNode) {
        return NULL;
    }

    // find successors.
    std::pair<BasicBlockNode*,BasicBlockNode*> succ =
        successors(jumpDestNode, cfg);

    bool keepFt = (cfg.inDegree(fallThruNode) != 1);
    bool keepJd = keepFt|(cfg.inDegree(jumpDestNode) > 2);
    //assert(cfg.inDegree(jumpDestNode) > 1);

    // temporary fix as these are hard for cfg updates
    if (keepFt) {
        return NULL;
    }

    //not desirable if fallThruNnode is big.
    if (fallThruNode.basicBlock().instructionCount() >
        triangleSizeLimit1_) {
        sizeAborts_++;
        return NULL;
    }

    if (keepJd) {
        if (cfg.jumpSuccessor(fallThruNode) != NULL) {
            return NULL;
        }
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, fallThruNode, NULL,
            &jumpDestNode, NULL, false, true, false);
    }
    // Cannot if-convert code where possibility of fall-through loop.
    if ((succ.first != NULL && succ.first->nodeID() < bbn.nodeID()) ||
        (succ.second != NULL && succ.second->nodeID() < bbn.nodeID())) {
        return NULL;
    }

    if (succ.first == NULL) {
        // we have a triangle first->jump->ft(branch)
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, jumpDestNode,
            NULL, succ.first, succ.second, !keepJd,
            !keepFt, !keepJd);
    } else {
        // we have a triangle first->jump->ft(branch)
        return new CandidateBlocks(
            bbn, fallThruNode, jumpDestNode, jumpDestNode,
            &jumpDestNode, succ.first, succ.second, !keepJd,
            !keepFt, !keepJd);
    }
    return NULL;
}


/**
 * Sub class CandidateBlocks
 */

/**
 * Constructor
 *
 * @param firstNode node where the branch was
 * @param fallThruNode node where fell thru if branch not taken
 * @param jumpDestNode node where the jump jumped
 * @param lastNode last node in the area being converter.
          May be fallThruNode, jumpDestNode or joinNode.
 * @param joinNode node where the paths join. Can be jumpNode, fallThruNode,
          lastNode or NULL if paths join outside converted area(at succNode1)
 * @param succNode1 1st successor node(fall-thru of original lastNode)
 * @param succNode2 2nd succossor node(if lastNode has branch, branch target)
 */
SimpleIfConverter::CandidateBlocks::CandidateBlocks(
    BasicBlockNode& firstNode, BasicBlockNode& fallThruNode,
    BasicBlockNode& jumpDestNode, BasicBlockNode& lastNode,
    BasicBlockNode* joinNode, BasicBlockNode* succNode1,
    BasicBlockNode* succNode2,
    bool removeJoin, bool removeFT, bool removeJd) :
    firstNode_(firstNode), fallThruNode_(fallThruNode),
    jumpDestNode_(jumpDestNode), lastNode_(lastNode),
    joinNode_(joinNode), succNode1_(succNode1),
    succNode2_(succNode2),
    firstBB_(firstNode.basicBlock()), fallThruBB_(fallThruNode.basicBlock()),
    jumpDestBB_(jumpDestNode.basicBlock()),
    lastBB_(lastNode.basicBlock()),
    joinBB_(joinNode == NULL ? NULL : &joinNode->basicBlock()),
    removeJoin_(removeJoin), removeFt_(removeFT), removeJd_(removeJd),
    createJump_(false),
    guard_(NULL), invg_(NULL), grIndex_(-1), grFile_(NULL) {}


/**
 * Destructor
 */
SimpleIfConverter::CandidateBlocks::~CandidateBlocks() {
    if (guard_ != NULL) {
        delete guard_;
    }
    if (invg_ != NULL) {
        delete invg_;
    }
}


