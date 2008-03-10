/**
 * @file DataDependenceGraphBuilder.cc
 *
 * Implementation of data dependence graph builder
 *
 * @author Heikki Kultala 2006-2008 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include <list>

#include "AssocTools.hh"
#include "ContainerTools.hh"

#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Operation.hh"
#include "SpecialRegisterPort.hh"
#include "Move.hh"
#include "ProgramOperation.hh"
#include "RegisterFile.hh"
#include "Machine.hh"
#include "UniversalMachine.hh"
#include "Exception.hh"
#include "UnboundedRegisterFile.hh"

#include "ControlFlowGraph.hh"
#include "ControlFlowEdge.hh"
#include "BasicBlockNode.hh"

#include "DataDependenceGraphBuilder.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "MemoryAliasAnalyzer.hh"
#include "POMDisassembler.hh"

#include "ConstantAliasAnalyzer.hh"
#include "FalseAliasAnalyzer.hh"

using namespace TTAProgram;
using namespace TTAMachine;

using std::list;

static const int REG_SP = 1;
static const int REG_RV = 0;
static const int REG_IPARAM = 2;

class DataDependenceGraph;
class BasicBlockNode;

/**
 * Constructor of Data Dependence graph builder
 */
DataDependenceGraphBuilder::DataDependenceGraphBuilder() :
    processOrder_(0), entryNode_(NULL), entryData_(NULL) {
    addAliasAnalyzer(new ConstantAliasAnalyzer);
//    addAliasAnalyzer(new FalseAliasAnalyzer); 
}

/**
 * Destructor of DataDependenceGraphBuilder
 */
DataDependenceGraphBuilder::~DataDependenceGraphBuilder() {
    AssocTools::deleteAllValues(specialRegisters_);
    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        delete(aliasAnalyzers_.at(i));
    }
}

/**
 * Adds a memory alias analyzer to the DDG builder.
 * 
 * Currently not yet used.
 *
 * @param analyzer object which will analyze memory accesses.
 */
void
DataDependenceGraphBuilder::addAliasAnalyzer(MemoryAliasAnalyzer* analyzer) {
    aliasAnalyzers_.push_back(analyzer);
}

/**
 * Changes state of a basic block in processing.
 * Move BBData into a diffefent list and changes the state data in BBData.
 * 
 * @param bbd BBData of basic block whose state is being changed
 * @param newState the new state of the basic block.
*/
void
DataDependenceGraphBuilder::changeState(
    list<BBData*> (&blocksByState)[BB_STATES], BBData* bbd, BBState newState) {

    BBState oldState = bbd->state_;
    if (newState != oldState) {
        ContainerTools::removeValueIfExists(blocksByState[oldState], bbd);
        bbd->state_ = newState;
        blocksByState[newState].push_back(bbd);
    }
}

/**
 * Creates new Data Dependence Graph for the given basic block.
 *
 * Client has to delete the graph when it is not anymore used.
 *
 * @param bb BasicBlockNode whose data dependence graph to build.
 * @return new DataDependence Graph.
 *
 */
DataDependenceGraph*
DataDependenceGraphBuilder::build(
    BasicBlock& bb, const UniversalMachine* um) 
    throw (IllegalProgram) {

    singleBBMode_ = true;
    currentDDG_ = new DataDependenceGraph();
    // GRR, start and end addresses are lost..
    currentBB_ = new BasicBlockNode(bb); 
    currentData_ = new BBData(*currentBB_);

    if (um != NULL) {
        getStaticRegisters(*um, specialRegisters_);
    } else {
        getStaticRegisters(bb,specialRegisters_);
    }

    constructIndividualBB();
    delete currentData_;
    currentData_ = NULL;
    return currentDDG_;
}

/**
 * Creates new data dependence graph of a control flow graph.
 *
 * Client has to delete the graph when it is not anymore used.
 *
 * @param cGraph controlFlowGraph of procedure whose DDG is built.
 * @return new DataDependence Graph.
 *
 */
DataDependenceGraph*
DataDependenceGraphBuilder::build(
    ControlFlowGraph& cGraph, const UniversalMachine* um) {

    singleBBMode_ = false;

    // @TODO: when CFG subgraphs are in use, 2nd param not always true
    DataDependenceGraph* ddg = new DataDependenceGraph(
        cGraph.procedureName(), true);
    
    if (um != NULL) {
        getStaticRegisters(*um, specialRegisters_);
    } else {
        getStaticRegisters(cGraph, specialRegisters_);
    }

    currentDDG_ = ddg;
    list<BBData*> blocksByState[BB_STATES];

    entryNode_ = new MoveNode();
    entryData_ = new MNData(entryNode_, specialRegisters_, true);

    currentDDG_->addNode(*entryNode_, cGraph.entryNode());

    for (int bbi = 0; bbi < cGraph.nodeCount(); bbi++) {
        currentBB_ = &(cGraph.node(bbi));
        BBData* bbd = new BBData(*currentBB_);
        bbData_[currentBB_] = bbd;
        if (currentBB_->isNormalBB()) {
            blocksByState[BB_UNREACHABLE].push_back(bbd);
        }
    }

    BBNodeSet firstBBs = cGraph.successors(cGraph.entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBB = *firstBBs.begin();

    changeState(blocksByState, bbData_[firstBB], BB_QUEUED);
    bbData_[firstBB]->loopDepth_ = 0;
    bbData_[firstBB]->ownRegWrites_.insert(*entryData_);

    // Goes thru all basic blocks in the procedure.
    // Loops as long as there are basic blocks on states waiting to be 
    // processed.
    while (!blocksByState[BB_QUEUED].empty() ||
           !blocksByState[BB_REACHABLE].empty() || 
           !blocksByState[BB_UNREACHABLE].empty()) {

        if (blocksByState[BB_QUEUED].empty()) {
            // If no queued BB's available, we have to jump
            // into a loop by selecting a reachable BB.

            // but if none reachable, we have some unreachable BB. 
            // select one with no inputs
            if (blocksByState[BB_REACHABLE].empty()) {
                std::list<BBData*>& unreachables = 
                    blocksByState[BB_UNREACHABLE];
                bool orphanBBfound = false;
                for (std::list<BBData*>::iterator iter = 
                         unreachables.begin(); iter != unreachables.end();
                     iter++) {
                    BBData* bbd = *iter;
                    if (cGraph.inDegree(*bbd->bblock_) == 0) {
                        currentData_ = bbd;
                        unreachables.erase(iter);
                        orphanBBfound = true;
                        break;
                    }
                }
                assert(orphanBBfound);
            } else {
                currentData_ = *(blocksByState[BB_REACHABLE].begin());
            }
        } else {
            // Goes thru list of QUEUED basic blocks, 
            // searching for the one with smallest loop depth.
            // If there are many with same loop depth, selects the first.
            int smallestLD = INT_MAX;
            list<BBData*>::iterator si;
            for (list<BBData*>::iterator it = blocksByState[BB_QUEUED].begin();
                 it != blocksByState[BB_QUEUED].end(); it++) {
                if ((*it)->loopDepth_ < smallestLD) {
                    si = it;
                    smallestLD = (*it)->loopDepth_ ;
                } 
                assert((*it)->loopDepth_ != INT_MAX);
            }
            // reachable is unreachable, internal error?
            assert(smallestLD != INT_MAX);
            currentData_ = *si;
        }
        currentBB_ = currentData_->bblock_;

        // preceeding blocks
        BBNodeSet inputBBs = cGraph.predecessors(*currentBB_);

        if (currentBB_->isNormalBB()) {
            if (currentData_->processedCount_ > 0) {
                // DDG for this BB Already constructed, 
                // so do not construct again, only add dependencies to input
                // BB's that were constructed later.
                updateBB(inputBBs); 
            } else {
                // First time processing this BB so construct the DDG.
                constructBB(inputBBs); 
            }
        } else {
            // skip entry and exit nodes
        }

        if (currentData_->loopDepth_ == 0) {
            // This BB is ready.
            changeState(blocksByState, currentData_, BBState(BB_PROCESSED));
        } else {
            // This BB is not fully processed because it is in a loop.
            // Make it wait for it's missing predecessor.
            changeState(blocksByState, currentData_, BBState(BB_NOT_READY));
        }

        // search all successors of the processed bb.
        for (int i = 0; i < cGraph.outDegree(*currentBB_); i++) {
            ControlFlowEdge& e = cGraph.outEdge(*currentBB_, i);
            BBData* outputBB = bbData_[&cGraph.headNode(e)];

            // only BB's in states under BB_PROCESSED 
            // may still need some processing
            if (outputBB->state_ < BB_PROCESSED) {
                bool ready = true;
                outputBB->predCount_ = cGraph.inDegree(*outputBB->bblock_);

                for (int j = 0; j < outputBB->predCount_; j++) {

                    ControlFlowEdge& e = cGraph.inEdge(*outputBB->bblock_, j);
                    BBData* inputBB = bbData_[&cGraph.tailNode(e)];
                    if (inputBB->processedCount_ == 0)
                        ready = false;

                    // following BB of handled BB are also in loop
                    if (inputBB->loopDepth_ < outputBB->loopDepth_)
                        outputBB->loopDepth_ = inputBB->loopDepth_;
                }

                if (ready) {
                    if (outputBB->maxLoopDepth_ < outputBB->loopDepth_)
                        outputBB->maxLoopDepth_ = outputBB->loopDepth_;

                    if (outputBB->processedCount_ + outputBB->loopDepth_
                            <= outputBB->maxLoopDepth_) {
                        changeState(blocksByState, outputBB, BB_QUEUED);
                    } 

                } else {
                    outputBB->loopDepth_++;

                    if (outputBB->maxLoopDepth_ < outputBB->loopDepth_)
                        outputBB->maxLoopDepth_ = outputBB->loopDepth_;

                    if (outputBB->processedCount_ < outputBB->maxLoopDepth_)

                        changeState(blocksByState, outputBB, BB_REACHABLE);
                }
            }
        }


        // check that "sister BB's" are procedded. if they are, data related to
        // input BB's can be deleted as it's no longer needed.
        for (BBNodeSet::iterator i = inputBBs.begin(); i != inputBBs.end(); i++) {
            bool allReady = true;
            BBData* inputBB = bbData_[*i];
            if (inputBB->state_ == BB_PROCESSED) {
                for (int j = 0;j < cGraph.outDegree(*inputBB->bblock_); j++) {
                    ControlFlowEdge& e2 = cGraph.outEdge(*inputBB->bblock_, j);
                    BBData* outputBB = bbData_[&cGraph.headNode(e2)];
                    if (outputBB->state_ < BB_PROCESSED) {
                        allReady = false;
                        break;
                    }
                }
                if (allReady) {
                    inputBB->clear();
                }
            }
        }

    }

    // free bb data
    AssocTools::deleteAllValues(bbData_);

    return ddg;
}

/** 
 * Inserts contest of one MoveNodeData list to another
 *
 * @param dst Destination list. Appends to this list.
 * @param src Source. Inset contents of this list to another
 */
void
DataDependenceGraphBuilder::copyMNDList(MNDList& dst, MNDList& src) {
    for (MNDIter mni = src.begin(); mni != src.end(); mni++) {
        dst.insert(MNData(*mni));
    }
}


/**
 * Initializes dependencies between blocks by copying live registers,
 * mem writes etc. from preceeding blocks to block being processed
 *
 * @param inputBlocks Set containing the preceeding blocks where to copy
 * the incoming data.
 */
void
DataDependenceGraphBuilder::setPreDependencies(
    BBNodeSet& inputBlocks) {

    for (BBNodeSet::iterator biter = inputBlocks.begin();
            biter != inputBlocks.end(); biter++) {
        if (*biter != currentBB_) {
            if (currentData_->predCount_ > 1) {

                // reg writes of prev blocks
                copyMNDList(currentData_->preRegWrites_,
                            (*bbData_[*biter]).ownRegWrites_);

                // mem writes of prev blocks
                copyMNDList(currentData_->preMemWrites_,
                            (*bbData_[*biter]).memWrites_);

                // mem reads of prec blocks for antidep
                copyMNDList(currentData_->preMemReads_,
                            (*bbData_[*biter]).memReads_);

                copyMNDList(currentData_->preFuStates_,
                            (*bbData_[*biter]).fuStateWrites_);

            } else { // pred as owns

                // reg writes of prev blocks
                copyMNDList(currentData_->ownRegWrites_,
                            (*bbData_[*biter]).ownRegWrites_);

                // mem writes of prev blocks
                copyMNDList(currentData_->memWrites_,
                            (*bbData_[*biter]).memWrites_);

                // mem reads of prev blocks for antidep
                copyMNDList(currentData_->memReads_,
                            (*bbData_[*biter]).memReads_);

                copyMNDList(currentData_->fuStateWrites_,
                            (*bbData_[*biter]).fuStateWrites_);

            }

            // reg writes of prev blocks of prev blocks
            copyMNDList(currentData_->preRegWrites_,
                        (*bbData_[*biter]).preRegWrites_);

            // mem reads of prev blocks of prev blocks ( for antidep )
            copyMNDList(currentData_->preMemReads_,
                        (*bbData_[*biter]).preMemReads_);

            // mem writes ow prev blocks of prev blocks
            copyMNDList(currentData_->preMemWrites_,
                        (*bbData_[*biter]).preMemWrites_);

            copyMNDList(currentData_->preFuStates_,
                        (*bbData_[*biter]).preFuStates_);

            if ((*bbData_[*biter]).raWrite_ != 0)
                currentData_->raWrite_ =
                    new MNData(*((*bbData_[*biter]).raWrite_));
        }
    }
}

/**
 * Does a second pass on a basic block in order to get dependencies from
 * Basic blocks which are processed later( due loops in program )
 *
 * @param inputBlocks set containing the preceeding blocks.
 */
void
DataDependenceGraphBuilder::updateBB(BBNodeSet& inputBlocks) {

    MNDList oldOwnRegWrites;
    MNDList oldOwnMemWrites;
    MNDList oldFuStateWrites;

    // block itself not found from predecessors
    if (inputBlocks.find(currentBB_) == inputBlocks.end()) {
        // clear them temprorarily but restore at end of fn
        oldOwnRegWrites = currentData_->ownRegWrites_;
        oldOwnMemWrites = currentData_->memWrites_;
        oldFuStateWrites = currentData_->fuStateWrites_;
        currentData_->ownRegWrites_.clear();
        currentData_->memWrites_.clear();
        currentData_->fuStateWrites_.clear();
    }

    setPreDependencies(inputBlocks);

    // this is heavy.
    // create copies of these lists
    MNDList oldExtRegDeps = currentData_->extDepRegList_;
    MNDList oldExtMemDepReads = currentData_->extDepMemReads_;
    MNDList oldExtMemDepWrites = currentData_->extDepMemWrites_;

    // process everything on those copies.
    // can not loop thru originals as the originals may change during
    // this processing.
    /*
        cout << "\town reg wr size at begin:"
             << currentData_->ownRegWrites_.size()<<endl;
        cout << "\tpre reg wr size at begin:"
             << currentData_->preRegWrites_.size()<<endl;

        cout << "mem wr size at begin:"
             << currentData_->memWrites_.size() << endl;

        cout << "mem reads size at begin:"
             << currentData_->memReads_.size() << endl;

        cout << "\town reg rd size at begin:"
             << currentData_->ownRegReads_.size()<<endl;

        cout << "\tpre reg rd size at begin:"
             << currentData_->preRegReads_.size()<<endl;

        cout << "\tpre mem rd size at begin:"
             << currentData_->preMemReads_.size()<<endl;

        cout << "\tpre mem wr size at begin:"
             << currentData_->preMemWrites_.size()<<endl;

        cout << "extdepsRegs size at b:" << oldExtRegDeps.size() << endl;
        cout << "extdepsMems size at b:" << oldExtMemDepReads.size() << endl;
        cout << "extdepsMemWr size at b:" << oldExtMemDepWrites.size() << endl;

        cout << "\t\t\t---" << endl;
    */
    for (MNDIter iter = oldExtRegDeps.begin();
            iter != oldExtRegDeps.end(); iter = oldExtRegDeps.begin()) {
        processRegRead(*iter);
        oldExtRegDeps.erase(iter);
    }

    for (MNDIter iter = oldExtMemDepReads.begin();
         iter != oldExtMemDepReads.end() ; iter = oldExtMemDepReads.begin()) {
        processMemRead(*iter);
        oldExtMemDepReads.erase(iter);
    }

    for (MNDIter iter = oldExtMemDepWrites.begin();
         iter != oldExtMemDepWrites.end();iter = oldExtMemDepWrites.begin()) {
        processMemWrite(*iter);
        oldExtMemDepWrites.erase(iter);
    }

    // restore these
    copyMNDList(currentData_->ownRegWrites_, oldOwnRegWrites);
    copyMNDList(currentData_->memWrites_, oldOwnMemWrites);
    copyMNDList(currentData_->fuStateWrites_, oldFuStateWrites);

    currentData_->processedCount_++;
    /*
        cout << "\town reg wr size at end:"
             << currentData_->ownRegWrites_.size()<<endl;
        cout << "\tpre reg wr size at end:"
             << currentData_->preRegWrites_.size()<<endl;
        cout << "mem wr size at end:"
             << currentData_->memWrites_.size() << endl;

        cout << "mem reads size at end:"
             << currentData_->memWrites_.size() << endl;
    */

}


/**
 * Adds a basic block into multi-BB Data Dependence Graph.
 *
 * Analyzes all moves inside the basic block,
 * creates their ProgramOperations, MoveNodes and Edges,
 * and adds the nodes and edges to the graph.
 * Implementation justs sets the preceeding basic blocks and calls
 * constructIndividualBB() to do the actual work.
 * 
 * @param inputBlocks set containing the preceeding blocks.
 */
/**
 * @todo remove commented out code
 */
void
DataDependenceGraphBuilder::constructBB(BBNodeSet& inputBlocks) 
    throw (IllegalProgram) {

    currentData_->processOrder_ = processOrder_++;

    setPreDependencies(inputBlocks);

    /*
        cout << "\town reg wr size at begin:"
             << currentData_->ownRegWrites_.size()<<endl;
        cout << "\tpre reg wr size at begin:"
             << currentData_->preRegWrites_.size()<<endl;

        cout << "\tmem wr size at begin:"
             << currentData_->memWrites_.size() << endl;

        cout << "\tmem reads size at begin:"
             << currentData_->memReads_.size() << endl;

        cout << "\town reg rd size at begin:"
             << currentData_->ownRegReads_.size()<<endl;

        cout << "\tpre reg rd size at begin:"
             << currentData_->preRegReads_.size()<<endl;

        cout << "\tpre mem rd size at begin:"
             << currentData_->preMemReads_.size()<<endl;

        cout << "\tpre mem wr size at begin:"
             << currentData_->preMemWrites_.size()<<endl;

        cout << "\t\t\t---" << endl;
    */

    constructIndividualBB();

    currentData_->processedCount_ = 1;

    /*
        cout << "\town reg wr size at end:"
             << currentData_->ownRegWrites_.size()<<endl;
        cout << "\tpre reg wr size at end:"
             << currentData_->preRegWrites_.size()<<endl;

        cout << "\tmem wr size at end:"
             << currentData_->memWrites_.size() << endl;

        cout << "\tmem reads size at end:"
             << currentData_->memWrites_.size() << endl;
    */

}

/**
 * Constructs a Data Dependence Graph for a single basic block.
 *
 * Goes thru all moves in the basic block and analyzes their dependencies,
 * creates their ProgramOperations, MoveNodes and Edges,
 * and adds the nodes and edges to the graph.
 * Also used inside implementation of multi-BB-DDG-code.
 * BB being analyzed has to be already set in member variable currentBB_,
 * and the graph created and set into member variable currentBB_.
 */
void
DataDependenceGraphBuilder::constructIndividualBB() 
    throw (IllegalProgram) {

    for (int ia = 0; ia < currentBB_->basicBlock().instructionCount(); ia++) {
        Instruction& ins = currentBB_->basicBlock().instructionAtIndex(ia);
        for (int i = 0; i < ins.moveCount(); i++) {
            Move& move = ins.move(i);

            MoveNode* moveNode = new MoveNode(move);
            MNData mnData(moveNode, specialRegisters_, false);
            currentDDG_->addNode(*moveNode, *currentBB_);

            if (!(move.isUnconditional()))
                processGuard(mnData);

            processSource(mnData);
            processDestination(mnData);
        }
    }

    // Checks if we have some unready program operations at the end
    // of a basic block.
    if (currentData_->destPending_.size() > 0 ||
        currentData_->readPending_.size() > 0) {
        
        std::string msg = std::string("Basic block ") 
            + Conversion::toString(currentBB_->originalStartAddress()) 
            + std::string(" - ")
            + Conversion::toString(currentBB_->originalEndAddress()) 
            + std::string(", size : ") 
            + Conversion::toString(
                currentBB_->basicBlock().instructionCount())
            + std::string(" handled but we have ")
            + Conversion::toString(currentData_->destPending_.size())
            + std::string(" ")
            + Conversion::toString(currentData_->readPending_.size())
            + std::string(" unready PO's at: ")
            + currentDDG_->name() 
            + std::string(", probably an operation without result move?");

        for (unsigned int i = 0; i < currentData_->readPending_.size(); i++) {
            msg += "\n\tmissing read: " + currentData_->readPending_[i]->operation().name();
        }

        for (unsigned int i = 0; i < currentData_->destPending_.size(); i++) {
            msg += "\n\tmissing dest: " + currentData_->destPending_[i]->operation().name();
        }

        throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
    }

}

/**
 * Creates operand edges between intput and output moves of a
 * programoperation.
 * 
 * @param po ProgramOperation whose egdes we are creating.
 */
void DataDependenceGraphBuilder::createOperationEdges(
    ProgramOperation& po) {

    Operation& op = po.operation();
    // create operation edges
    // from all input nodes to all
    // output nodes, loops follow
    
    // loop thru all input parameters
    for (int inps = 1;
         inps <= op.numberOfInputs();
         inps++) {
        MoveNodeSet& mnsi = po.inputNode(inps);
        
        // exactly once on unscheduled code,
        // but still nice to not assume anything.
        for (int inp = 0; inp < mnsi.count(); inp++) {
            
            // Loop thru all output parameters
            for( int outps = op.numberOfInputs()+1;
                 outps <=
                     op.numberOfInputs()+op.numberOfOutputs();
                 outps++ ) {
                if( po.hasOutputNode(outps)) {
                    MoveNodeSet& mnso = po.outputNode(outps);
                    // exactly once on unscheduled code
                    // but still nice to not assume anything.
                    for (int outp = 0; outp<mnso.count(); outp++)
                    {
                        // and create operation edges
                        // from all input nodes to all
                        // output nodes.
                        DataDependenceEdge* dde = 
                            new DataDependenceEdge(
                                DataDependenceEdge::EDGE_OPERATION,
                                DataDependenceEdge::DEP_UNKNOWN);
                        
                        currentDDG_->connectOrDeleteEdge(
                            mnsi.at(inp), mnso.at(outp), dde);
                    }
                }
            }
        }
    }
}

/**
 * Compares a guard use against one previous reg write and
 * creates dependence if same reg
 *
 * @param prev Previous Memory write movenode
 * @param mn Current memory read movenode
 * @param pseudo create pseudo flag in the DD edge.
 * @return true if true edge created.
 */
bool
DataDependenceGraphBuilder::checkAndCreateGuardRAW(
    const MNData& prev, const MNData& current) {
    if (current.isRawToGuard(prev)) {

        bool tailPseudo = prev.node().move().isCall();
        
        // create dependency edge
        DataDependenceEdge* dde =
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER,
                DataDependenceEdge::DEP_RAW, true, false, tailPseudo);

        // add edge to the graph
        currentDDG_->connectOrDeleteEdge(
            prev.node(), current.node(), dde);
        return true;
    }
    return false;
}


/**
 * Analyzes dependencies related to guard usage.
 *
 * Finds the guard register used for the guard and the move
 * Which writes the guard register, and creates a guard egde 
 * between them.
 * 
 * @param moveNode MNData of move containing guarded move.
 */
void
DataDependenceGraphBuilder::processGuard(const MNData& moveNode) {

    bool writeFound = false;
    for (MNDIter rwi = currentData_->ownRegWrites_.begin();
         rwi != currentData_->ownRegWrites_.end(); rwi++) {
  
        if (checkAndCreateGuardRAW(*rwi, moveNode)) {
            writeFound = true;
        }
    }

    if (!writeFound) {
        for (MNDIter rwi = currentData_->preRegWrites_.begin();
             rwi != currentData_->preRegWrites_.end(); rwi++) {
            checkAndCreateGuardRAW(*rwi, moveNode);
        }
        currentData_->extDepRegList_.insert(MNData(moveNode));
    }
    currentData_->ownRegReads_.insert(MNData(moveNode));
}

/**
 * Analysis a source of a move and processes it's dependencies, 
 * and if it's a result read then also participates in ProgramOperation
 * creation.
 * 
 * @param moveNode MNData of the Move being analyzed.
 */
void
DataDependenceGraphBuilder::processSource(const MNData& moveNode) {
    Terminal& source = moveNode.node().move().source();

    // is this result move of an operation?
    if (source.isFUPort()) {
        if (!(dynamic_cast<const SpecialRegisterPort*>(&source.port()))) {
            processResultRead(moveNode);
        } else {
            processRARead(moveNode);
        }
    } else {
        if (source.isGPR()) {
            processRegRead(moveNode);
        }
    }
}

/**
 *
 * Analyzes a source of a result read. Handles program operation creation 
 * and operation dependence creation.
 *
 * @param moveNode MNData related to move being analyzed.
 */
void
DataDependenceGraphBuilder::processResultRead(const MNData& moveNode) {

    Terminal& source = moveNode.node().move().source();

    Operation* sop = NULL;
    TerminalFUPort& tfps = dynamic_cast<TerminalFUPort&>(source);
    sop = &tfps.hintOperation();

    for (POLIter poli = currentData_->readPending_.begin();
            poli != currentData_->readPending_.end(); poli++) {
        ProgramOperation* po = *poli;
        if (sop == &po->operation()) {
            po->addOutputNode(moveNode.node());
            moveNode.node().setSourceOperation(*po);

            // if this PO is ready, remove from list of uncomplete ones
            if (po->isComplete())   {

                createOperationEdges(*po);

                currentData_->readPending_.erase(poli);
                currentDDG_->addProgramOperation(po);
            } 
            return;
        }
    }
    throw IllegalProgram(
        __FILE__, __LINE__, __func__, "Result move without operands");
}

void 
DataDependenceGraphBuilder::createRegRaw(
    MoveNode& source, MoveNode& current, bool tailPseudo, bool headPseudo) {

    DataDependenceEdge* dde =
        new DataDependenceEdge(
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_RAW, false, false, 
            tailPseudo, headPseudo);

    currentDDG_->connectOrDeleteEdge(source, current, dde);
}

void 
DataDependenceGraphBuilder::createRegWar(
    MoveNode& source, MoveNode& current, bool tailPseudo, bool headPseudo) {

    DataDependenceEdge* dde =
        new DataDependenceEdge(
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_WAR, false, false, 
            tailPseudo, headPseudo);

    currentDDG_->connectOrDeleteEdge(source, current, dde);
}

void 
DataDependenceGraphBuilder::createRegWaw(
    MoveNode& source, MoveNode& current, bool tailPseudo, bool headPseudo) {
    if (source.isMove()) {
        DataDependenceEdge* dde =
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER,
                DataDependenceEdge::DEP_WAW, false, false, 
                tailPseudo, headPseudo);
        
        // create dependency edge
        currentDDG_->connectOrDeleteEdge(source, current, dde);
    }
}

/**
 * Analyzes dependencies for register read.

 * Finds the move or moves 
 * which have written the data into the register and creates dependence
 * edges between them.
 *
 * @param moveNode MNData related to move being analyzed.
 */
void
DataDependenceGraphBuilder::processRegRead(const MNData& moveNode) {

    MNData mnDataCopy(moveNode);
    int regsLeft = 1; 

    for (MNDIter wri = currentData_->ownRegWrites_.begin();
         wri != currentData_->ownRegWrites_.end(); wri++) {
        if (moveNode.isRaw(*wri)) {
            createRegRaw(wri->node(), moveNode.node());
            regsLeft = mnDataCopy.removeReadsOtherWrites(*wri);
        }
        if(moveNode.isRawToPseudo(*wri)) {
            createRegRaw(wri->node(), moveNode.node(), false, true);
            regsLeft = mnDataCopy.removeReadsOtherWrites(*wri);
        }
    }

    if (regsLeft) {
        for (MNDIter wri = currentData_->preRegWrites_.begin();
             wri != currentData_->preRegWrites_.end(); wri++) {
            if (mnDataCopy.isRaw(*wri)) {
                createRegRaw(wri->node(), moveNode.node());
            }
            
            if (mnDataCopy.isRawToPseudo(*wri)) {
                createRegRaw(wri->node(), moveNode.node(), false, true);
            }
        }
        // create external data dependency
        currentData_->extDepRegList_.insert(mnDataCopy);
    }
    // for later WAR's
    currentData_->ownRegReads_.insert(MNData(moveNode));
}

/**
 * Analyzes destination of a move. 
 * Updates bookkeeping and handles WaW and WaR dependencies of the move.
 * 
 * Checks whether destination is operation or register and calls other
 * functions to do the actual dependence checks etc.
 *
 * @param moveNode MNData related to move whose destination 
 * is being processed.
 */
void
DataDependenceGraphBuilder::processDestination(const MNData& moveNode) {

    Terminal& dest = moveNode.node().move().destination();

    // is this a operand to an operation?
    if (dest.isFUPort()) {
        if (!(dynamic_cast<const SpecialRegisterPort*>(&dest.port()))) {
            TerminalFUPort& tfpd = dynamic_cast<TerminalFUPort&>(dest);
            Operation &dop = tfpd.hintOperation();


            // is this a trigger?
            if (tfpd.isOpcodeSetting()) {
                processTrigger(moveNode, dop);
            } else { // not trigger
                processOperand(moveNode, dop);
            }
        } else {  // RA write
            processRAWrite(moveNode);
        }
    } else {
        if (dest.isGPR()) {
            processRegWrite(moveNode);
        } else { // somwthing else

        }
    }
}

/**
 * Compares a memory write against one previous memory write and
 * creates dependence if may alias.
 *
 * @param prev Previous Memory write movenode
 * @param mn Current memory write movenode
 * @param pseudo create pseudo flag in the DD edge.
 * @return true if true edge created.
 */
bool
DataDependenceGraphBuilder::checkAndCreateMemWAW(MoveNode& prev, MoveNode& mn, bool pseudo) {
    
    
    MemoryAliasAnalyzer::AliasingResult aliasResult =
        analyzeMemoryAlias(prev, mn);
    
    if (aliasResult != MemoryAliasAnalyzer::ALIAS_FALSE) {
        bool certainAlias = 
            (aliasResult == MemoryAliasAnalyzer::ALIAS_TRUE);

        bool tailPseudo = prev.move().isCall();
        
        ProgramOperation& po = prev.destinationOperation();
        for (int i = 0; i < po.inputMoveCount(); i++) {
            
            DataDependenceEdge* dde2 =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_MEMORY,
                    DataDependenceEdge::DEP_WAW, false, certainAlias, 
                    tailPseudo, pseudo);
            currentDDG_->connectOrDeleteEdge(
                po.inputMove(i), mn, dde2);
        }
        return certainAlias;
    }
    return false;
}

/**
 * Updates memory operation bookkeeping and creates WaR and WaW 
 * memory dependencies.
 *
 * @param moveNode MNData related to Move whose memory write to are
 * processing.
 */
void
DataDependenceGraphBuilder::processMemWrite(
    const MNData& moveNode, bool pseudo) throw (Exception) {

    // write which cannot be analyzed. no edges over it
    bool hasBarrierWrite = false;
    bool aliasWriteFound = false;
    for (MNDIter mwi = currentData_->memWrites_.begin();
         mwi != currentData_->memWrites_.end(); mwi++) {
        if (checkAndCreateMemWAW(mwi->node(), moveNode.node(), pseudo)) {
            aliasWriteFound = true;
        }
        // if non-analyzable write found, no deps over it to prev BB's
        if (!addressTraceable(mwi->node())) {
            hasBarrierWrite = true;
        }
    }

    // Nothing overwrites in this BB, search for WaW sources from previous
    // BB's.
    if (!hasBarrierWrite && !aliasWriteFound) {
        for (MNDIter mwi = currentData_->preMemWrites_.begin();
             mwi != currentData_->preMemWrites_.end(); mwi++) {
            checkAndCreateMemWAW(mwi->node(), moveNode.node(), pseudo);
        }
    }
    
    // this cannot be analyzed, so no edges over this.
    if (!addressTraceable(moveNode.node())) {
        currentData_->memWrites_.clear();
        currentData_->preMemWrites_.clear();
    }

    // WAR dependencies
    if (!currentData_->memReads_.empty()) {
        for (MNDIter mri = currentData_->memReads_.begin();
             mri != currentData_->memReads_.end(); mri++) {

            MoveNode& prevNode = mri->node();

            MemoryAliasAnalyzer::AliasingResult aliasResult =
                analyzeMemoryAlias(prevNode, moveNode.node());
    
            if (aliasResult != MemoryAliasAnalyzer::ALIAS_FALSE) {
                bool certainAlias = 
                    (aliasResult == MemoryAliasAnalyzer::ALIAS_TRUE);
        
                bool tailPseudo = mri->node().move().isCall();
            
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_MEMORY,
                        DataDependenceEdge::DEP_WAR, false, certainAlias,
                        tailPseudo, pseudo);
                currentDDG_->connectOrDeleteEdge((*mri).node(),
                                                 moveNode.node(), dde);
            }
        }
    }
    // mem dep may always come from before now.. if no barrier write
    if (!hasBarrierWrite && !aliasWriteFound) {
        currentData_->extDepMemWrites_.insert(MNData(moveNode));
    }
    currentData_->memWrites_.insert(MNData(moveNode));
}

/**
 * Analyzes a write to RA. Updates bookkeeping and creates WaW edge if 
 * previous RA write occurs.
 */
void
DataDependenceGraphBuilder::processRAWrite(const MNData& moveNode) {

    if (currentData_->raWrite_ != 0) {
        DataDependenceEdge* dde =
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_RA,
                DataDependenceEdge::DEP_WAW);
        currentDDG_->connectOrDeleteEdge(currentData_->raWrite_->node(),
                                         moveNode.node(), dde);
    }

    if (!currentData_->raReads_.empty()) {
        for (MNDIter rri = currentData_->raReads_.begin();
             rri != currentData_->raReads_.end(); rri++) {
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_RA,
                    DataDependenceEdge::DEP_WAR);
            currentDDG_->connectOrDeleteEdge((*rri).node(),
                                             moveNode.node(), dde);
        }
        currentData_->raReads_.empty();
    }
    currentData_->raWrite_ = new MNData(moveNode);
}

/**
 * Analyzes read from RA. 
 *
 * Creates RA RAW dependence edge and also checks function return-related 
 * dependencies.
 *
 * @param moveNode MNData related to the move which reads RA.
 */
void
DataDependenceGraphBuilder::processRARead(const MNData& moveNode) {
    
    if (currentData_->raWrite_ != 0) {
        DataDependenceEdge* dde =
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_RA, 
                DataDependenceEdge::DEP_RAW);
        currentDDG_->connectOrDeleteEdge(
            currentData_->raWrite_->node(), moveNode.node(), dde);
    } else {
        // if single-BB-mode, we have no entry nodes. 
        // then we need no dependence for RA.
        if (!singleBBMode_) {
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_RA,DataDependenceEdge::DEP_RAW);
            currentDDG_->connectOrDeleteEdge(
                *entryNode_, moveNode.node(), dde);
        }
    }

    currentData_->raReads_.insert(MNData(moveNode));
    if (moveNode.isReturn())
        processRegRead(moveNode);

}

/**
 * Analyzes write to a register.
 *
 * Updates bookkeeping and creates WaW and WaR dependence edges.
 * 
 * @param moveNode MNData related to move whose whose register write
 * is being analyzed.
*/
void
DataDependenceGraphBuilder::processRegWrite(const MNData& moveNode) {

    // check WAW's with previous reg writes in same BB 
    for (MNDIter wri = currentData_->ownRegWrites_.begin();
            wri != currentData_->ownRegWrites_.end();) {
        /* update latest write to this reg */
        if (moveNode.isWaw(*wri)) {
            // create antidependence
            // do not create antidependencies for initalizing param regs
            createRegWaw(wri->node(), moveNode.node());

            MNData mnd(*wri);
            currentData_->ownRegWrites_.erase(wri);
            int writesLeft = mnd.removeWritesOnWAW(moveNode);
            if (writesLeft) {
                currentData_->ownRegWrites_.insert(mnd);
            }
            
            // Code above has deleted an item from ownRegWrites_ 
            // so reset the iterator to beginning to be sure the iterator
            // is still valid and we do not jump over anything. 
            wri = currentData_->ownRegWrites_.begin();
        } else {
            wri++;
        }
    }

    // check WAW's with previous reg writes in preceeding BB's
    for (MNDIter wri = currentData_->preRegWrites_.begin();
            wri != currentData_->preRegWrites_.end();) {
        // update latest write to this reg 
        if (moveNode.isWaw(*wri)) {   
            // create antidependence
            createRegWaw(wri->node(), moveNode.node());
            MNData mndata(*wri);
            currentData_->preRegWrites_.erase(wri);
            int writesLeft = mndata.removeWritesOnWAW(moveNode);
            if (writesLeft) {
                currentData_->preRegWrites_.insert(mndata);
            }
            wri = currentData_->preRegWrites_.begin();
        } else {
            wri++;
        }
    }

    // WAR for variables read in current BB
    for (MNDIter rri = currentData_->ownRegReads_.begin();
            rri != currentData_->ownRegReads_.end();) {

        // create antidependence
        if (moveNode.isWar((*rri))) {
            createRegWar(rri->node(), moveNode.node());
            MNData mndata(*rri);
            currentData_->ownRegReads_.erase(rri);
            int readsLeft = mndata.removeReadsOtherWrites(moveNode);
            if (readsLeft) {
                currentData_->ownRegReads_.insert(mndata);
            }
            rri = currentData_->ownRegReads_.begin();

        } else {
            rri++;
        }
    }
    currentData_->ownRegWrites_.insert(MNData(moveNode));
}

/**
 * Analyze write to a trigger of an operation.
 *
 * Participates in ProgramOperation building.
 * Calls createTriggerDependencies(moveNode, dop) to create 
 * the dependence egdes of the operation.
 * Checks if operation is call and if it is, processes the call-related
 * register dependencies.
 * 
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 */
void
DataDependenceGraphBuilder::processTrigger(
    const MNData& moveNode, Operation &dop) 
    throw (IllegalProgram) {

    createTriggerDependencies(moveNode, dop);

    for (POLIter poli = currentData_->destPending_.begin();
            poli != currentData_->destPending_.end();
            poli++) {
        ProgramOperation* po = *poli;

        if (&dop == &po->operation()) {
            po->addInputNode(moveNode.node());
            moveNode.node().setDestinationOperation(*po);
            if (po->isReady()) {
                currentData_->destPending_.erase(poli);
                if (dop.numberOfOutputs()) {
                    currentData_->readPending_.push_back(po);
                } else {
                    currentDDG_->addProgramOperation(po);
                }
            } else {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, "Trigger too early");
            }
            return;
        }
    }
    // only one triggering input?
    if (dop.numberOfInputs() == 1) {
        ProgramOperation *po = new ProgramOperation(dop);
        moveNode.node().setDestinationOperation(*po);
        po->addInputNode(moveNode.node());
        if (dop.numberOfOutputs()) {
            currentData_->readPending_.push_back(po);
        } else {
            currentDDG_->addProgramOperation(po);
        }
    } else { // trigger came too early
        const std::string moveDisasm = 
            POMDisassembler::disassemble(moveNode.node().move());
        throw IllegalProgram(
            __FILE__,__LINE__, __func__, 
            std::string("Trigger without operand in ") + moveDisasm);
    }
    if (moveNode.isCall()) {

        // calls mess up RA.
        processRAWrite(moveNode);

        processRegRead(moveNode); // params
        processRegWrite(moveNode); // sp, rv

        // call uses mem
        processMemWrite(moveNode, true);
    }
}



/**
 * Compares a memory read against one previous memory write and
 * creates dependence if may alias.
 *
 * @param prev Previous Memory write movenode
 * @param mn Current memory read movenode
 * @param pseudo create pseudo flag in the DD edge.
 * @return true if true edge created.
 */
bool
DataDependenceGraphBuilder::checkAndCreateMemRAW(
    MoveNode& prev, MoveNode& mn, bool pseudo) {
    
    MemoryAliasAnalyzer::AliasingResult aliasResult =
        analyzeMemoryAlias(prev, mn);
    
    if (aliasResult != MemoryAliasAnalyzer::ALIAS_FALSE) {
        bool certainAlias = 
            (aliasResult == MemoryAliasAnalyzer::ALIAS_TRUE);
        
        bool tailPseudo = prev.move().isCall();
    
        ProgramOperation& po = prev.destinationOperation();
    
        for (int i = 0; i < po.inputMoveCount(); i++) {
            DataDependenceEdge* dde2 =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_MEMORY,
                    DataDependenceEdge::DEP_RAW, false, certainAlias, 
                    tailPseudo, pseudo);
            currentDDG_->connectOrDeleteEdge(
                po.inputMove(i), mn, dde2);
        }
        return certainAlias;
    }
    return false;
}

/**
 * Analyzes read from memory.
 * Creates RAW memory dependencies and updates bookkeeping.
 *
 * @param moveNode MNData related to a move that reads from memory.
 */
void
DataDependenceGraphBuilder::processMemRead(
    const MNData& moveNode, bool pseudo) throw (Exception) {

    bool hasBarrierWrite = false;
    bool aliasFound = false;

    for (MNDIter mwi = currentData_->memWrites_.begin();
         mwi != currentData_->memWrites_.end(); mwi++) {
        
        if (checkAndCreateMemRAW(mwi->node(), moveNode.node(), pseudo)) {
            aliasFound = true;
        }
        if (!addressTraceable(mwi->node())) {
            hasBarrierWrite = true;
        }
    }
    /* Check if this may depend from some write ini a previous BB */
    if (!hasBarrierWrite && !aliasFound) {
        for (MNDIter mwi = currentData_->preMemWrites_.begin();
             mwi != currentData_->preMemWrites_.end(); mwi++) {

            checkAndCreateMemRAW(mwi->node(), moveNode.node(), pseudo);
        }
        currentData_->extDepMemReads_.insert(MNData(moveNode));
    }
    /* All this read to bookkeeping for WAR dependence handling */
    currentData_->memReads_.insert(moveNode);
}

/**
 * Analyzes operation of a trigger write.
 * If memory read, callls processMemRead to manage memory read dependencies.
 * Manages FU State dependencies between operations.
 *
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 *
 */
void DataDependenceGraphBuilder::createTriggerDependencies(
    const MNData& moveNode, Operation& dop) {

    // this is here as we have data as trigger

    if (dop.readsMemory()) {
        processMemRead(moveNode);
    }

    const int affectedCount = dop.affectedByCount();
    if (affectedCount || dop.hasSideEffects()) {
        for (MNDIter fui = currentData_->fuStateWrites_.begin();
                fui != currentData_->fuStateWrites_.end(); fui++) {
            Operation& o = (*fui).node().destinationOperation().operation();

            // mem writes are handled by memory deps so exclude here
            if ((&dop == &o && o.hasSideEffects() && !o.writesMemory()) || 
                dop.dependsOn(o)) {
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_FUSTATE,
                        DataDependenceEdge::DEP_UNKNOWN);
                currentDDG_->connectOrDeleteEdge(
                    (*fui).node(), moveNode.node(), dde);
            }
        }

        for (MNDIter fui = currentData_->preFuStates_.begin();
                fui != currentData_->preFuStates_.end(); fui++) {
            Operation& o = (*fui).node().destinationOperation().operation();

            // mem writes are handled by memory deps so exclude here
            if ((&dop == &o && o.hasSideEffects() && !o.writesMemory()) || 
                dop.dependsOn(o)) {
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_FUSTATE,
                        DataDependenceEdge::DEP_UNKNOWN);
                currentDDG_->connectOrDeleteEdge(
                    (*fui).node(), moveNode.node(), dde);
            }
        }
    }

    if (dop.hasSideEffects()) {
        currentData_->fuStateWrites_.insert(MNData(moveNode));
    }
}

/**
 * Analyzes operand write.
 * 
 * Part of ProgramOperation creation.
 *
 * @param moveNode mnData related to a move which writes a parameter.
 * @param dop Operation whose parameter is being written.
 */
void
DataDependenceGraphBuilder::processOperand(
    const MNData& moveNode, Operation &dop) {


    /* Only check memory operations between addresses,
       not data. */
    if (dop.writesMemory()) {
        processMemWrite(moveNode);
    }

    for (POLIter poli = currentData_->destPending_.begin();
         poli != currentData_->destPending_.end(); poli++) {

        ProgramOperation& po = **poli;

        if (&dop == &po.operation()) {
            // add current move to operation

            // check that no other move has same input num
            // not be needed for universalmachine code?

            // does not yet exist

            // TBD: check for inconsistent input code
            po.addInputNode(moveNode.node());
            moveNode.node().setDestinationOperation(po);
            return;
        }
    }
    // create new ProgramOperation
    ProgramOperation* po = new ProgramOperation(dop);
    moveNode.node().setDestinationOperation(*po);
    po->addInputNode(moveNode.node());
    currentData_->destPending_.push_back(po);
}

/**
 * Delegates call to all registered memory address alias analyzers.
 *
 * Returns the first non-unknown result
 *
 * @return Whether the memory accesses in the given moves alias.
 */
MemoryAliasAnalyzer::AliasingResult
DataDependenceGraphBuilder::analyzeMemoryAlias(
    MoveNode& mn1, MoveNode& mn2) {

    MemoryAliasAnalyzer::AliasingResult result = 
        MemoryAliasAnalyzer::ALIAS_UNKNOWN;
    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        result = aliasAnalyzers_[i]->analyze(*currentDDG_, mn1, mn2);
        if (result != MemoryAliasAnalyzer::ALIAS_UNKNOWN) {
            return result;
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// BBData
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
DataDependenceGraphBuilder::BBData::BBData(BasicBlockNode& bb) :
    raWrite_(0), state_(BB_UNREACHABLE), processedCount_(0),
    loopDepth_(INT_MAX-1), maxLoopDepth_(0), predCount_(0), bblock_(&bb) {
}

/**
 * Destructor. 
 */
DataDependenceGraphBuilder::BBData::~BBData() {
    clear();
}

/**
 * Clears all data.
 */
void
DataDependenceGraphBuilder::BBData::clear() {
    ownRegReads_.clear();
    preRegReads_.clear();
    ownRegWrites_.clear();
    preRegWrites_.clear();
    extDepRegList_.clear();

    memWrites_.clear();
    memReads_.clear();
    preMemWrites_.clear();
    preMemReads_.clear();

    extDepMemReads_.clear();
    extDepMemWrites_.clear();

    if (raWrite_) {
        delete raWrite_;
        raWrite_ = 0;
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::Program& prog, 
    std::map<int,TerminalRegister*>& registers) {
    for (int i = 0; i < prog.procedureCount(); i++) {
        getStaticRegisters(prog, registers);
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::CodeSnippet& cs, 
    std::map<int,TerminalRegister*>& registers) {
    for (int i = 0; i < cs.instructionCount(); i++) {
        Instruction& ins = cs.instructionAtIndex(i);
        getStaticRegisters(ins, registers);
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    ControlFlowGraph& cfg,
    std::map<int,TerminalRegister*>& registers) {
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            getStaticRegisters(bbn.basicBlock(), registers);
        }
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::Instruction& ins, 
    std::map<int,TerminalRegister*>& registers) {
    for (int i = 0; i < ins.moveCount(); i++) {
        Move& move = ins.move(i);
        for (int j = 0; j < move.annotationCount(); j++) {
            ProgramAnnotation anno = move.annotation(i);
            switch (anno.id()) {
            case ProgramAnnotation::ANN_REGISTER_RV_READ:
                registers[REG_RV] = dynamic_cast<TerminalRegister*>(
                    move.source().copy());
                break;
            case ProgramAnnotation::ANN_REGISTER_RV_SAVE:
                registers[REG_RV] = dynamic_cast<TerminalRegister*>(
                    move.destination().copy());
                break;
            case ProgramAnnotation::ANN_REGISTER_SP_READ:
                registers[REG_SP] = dynamic_cast<TerminalRegister*>(
                    move.source().copy());
                break;
            case ProgramAnnotation::ANN_REGISTER_SP_SAVE:
                registers[REG_SP] = dynamic_cast<TerminalRegister*>(
                    move.destination().copy());
                break;
            case ProgramAnnotation::ANN_REGISTER_IPARAM_READ:
                registers[REG_IPARAM+Conversion::toInt(anno.stringValue())] =
                    dynamic_cast<TerminalRegister*>(move.source().copy());
                break;
            case ProgramAnnotation::ANN_REGISTER_IPARAM_SAVE:
                registers[REG_IPARAM+Conversion::toInt(anno.stringValue())] = 
                    dynamic_cast<TerminalRegister*>(
                        move.destination().copy());
            default:
                //TODO: frame pointer, not yet implemented
                break;
            }
        }
    }
}

/**
 * Initializes the static register table from register from
 * UniversalMachine. Needed for analysis of data dependencies
 * of parameter registers, SP, RV etc. 
 *
 * @param um UniversalMachine
 * @param registers map where to store those registers.
 */
void
DataDependenceGraphBuilder::getStaticRegisters(
    const UniversalMachine& um, std::map<int,TerminalRegister*>& registers) {
    RegisterFile& rf = um.integerRegisterFile();
    
    for (int i = 0; i < 6; i++) {
        registers[i] = 
            new TerminalRegister(*rf.port(0), i);
    }
}

/**
 * Can some analyzer can say something about this address?
 *
 * @param mn Movenode containing memory address write.
 * @return true if some alias analyzer knows something about the address.
 */
bool 
DataDependenceGraphBuilder::addressTraceable(MoveNode& mn) {
    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        if (aliasAnalyzers_.at(i)->addressTraceable(mn)) {
            return true;
        }
    }
    return false;
}
