/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file ResourceConstraintAnalyzer.cc
 *
 * Definition of ResourceConstraintAnalyzer class.
 *
 * @author Pekka Jääskeläinen 2010-2012
 * @note rating: red
 */

#include "ResourceConstraintAnalyzer.hh"
#include "Machine.hh"
#include "ProgramOperation.hh"
#include "MoveNode.hh"
#include "HWOperation.hh"
#include "Instruction.hh"
#include "Guard.hh"
#include "TerminalFUPort.hh"
#include "MoveGuard.hh"
#include "Operation.hh"
#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "Move.hh"

/**
 * Analyzes the resource constraints in the schedule.
 *
 * @return true in case at least one resource or dep constrained move found. 
 * Thus always true in case the schedule is "perfect".
 */
bool
ResourceConstraintAnalyzer::analyze() {

    origDDG_.setProgramOperationNodes(true);
    optimalScheduleResourceUsage(origDDG_, graphName_);  

    origDDG_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    DataDependenceGraph* criticalPath = origDDG_.criticalPathGraph();
    criticalPath->setProgramOperationNodes(true);
    origDDG_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);

    optimalScheduleResourceUsage(*criticalPath, graphName_ + ".critical_path");  
    delete criticalPath; criticalPath = NULL;

    DataDependenceGraph* memDeps = origDDG_.memoryDependenceGraph();
    memDeps->setProgramOperationNodes(true);
    memDeps->writeToDotFile(graphName_ + ".mem_deps.dot");
    delete memDeps; memDeps = NULL;

    return true;
#if 0
    // TODO: finish the more intelligent iterative search for resource bottlenecks

    busConstrained_ = 0;
    operationConstrained_ = 0;
    rfWritePortConstrained_ = 0;
    rfReadPortConstrained_ = 0;
    unknownConstrained_ = 0;
    writePortConstrainedRfs_.clear();
    readPortConstrainedRfs_.clear();
    operationConstrainedMoves_.clear();
    origDDG_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    // only sensible basic block with 1 cycle is one with only
    // reg moves or a store, ignore those
    minScheduleLength_ = std::max(origDDG__.height(), 2);
    origDDG_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);



    const int cycleCount = ddg_.largestCycle() + 1;
    const int nodes = ddg_.nodeCount();

    /*

      Collect the resource constrained moves that are lengthening the 
      schedule over the critical path length.

      Picks up all moves in the instructions > critical path length and
      "climbs" up the dependency tree and collects the first resource
      constrained move found (we use top-down scheduling) that is 
      above the minimum schedule length.
    
      The idea is try to "lift the resource constrained DDG tree 
      upwards" to try to bring the late moves within the minimum 
      schedule length.

      What if there's no such move? If all the moves in the tree
      are after the minimum schedule? In that case some other path is
      limiting the schedule thus we should just ignore the move and look
      at other moves.

      Should we scan all cycles or just the first cycle after the min
      cycle? We might add too many resources in case the bottleneck is
      resolved at the first cycle. I guess we should scan until we find
      *one* such move? Solving its resource constraint could lead to
      removing a bottleneck so whenever we scan more than one move there's 
      a risk of adding extra resources that do not really contribute to the
      schedule length as the constraint might have been solved by the 
      single move's bottleneck. This leads to many evaluation iterations but
      I guess it's better than to waste resources. Might add some kind of
      parameter for controlling the maximum number of constrained moves 
      to find per iteration.

    */
    int maxMovesToFind = 4;
    foundMoves_.clear();
    resourceConstrained_ = 0;
    for (int cycle = minScheduleLength_;
         cycle < cycleCount && resourceConstrained_ < maxMovesToFind; ++cycle) {
        DataDependenceGraph::NodeSet moves = ddg_.movesAtCycle(cycle);
        for (DataDependenceGraph::NodeSet::iterator m = moves.begin(); 
             m != moves.end() && resourceConstrained_ < maxMovesToFind; ++m) {
            MoveNode* node = findResourceConstrainedParent(*m);
            if (node != NULL) {
                Application::logStream() 
                    << "found a schedule constraining move: " 
                    << node->move().toString() << " at cycle " 
                    << node->cycle() << " earliest DDG cycle " 
                    << ddg_.earliestCycle(*node) << std::endl;
                ++resourceConstrained_;
            }
        }
    }
 
    if (true || resourceConstrained_ > 0) {

#if 0
        ddg_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);

        // print out the instructions
        for (int c = 0; c < cycleCount; ++c) {
            Application::logStream() << c << ": ";
            DataDependenceGraph::NodeSet moves = ddg_.movesAtCycle(c);
            for (DataDependenceGraph::NodeSet::iterator m = moves.begin(); 
                 m != moves.end(); ++m) {
                MoveNode& mn = **m;
//                if (!ddg_.isInCriticalPath(mn) && !mn.move().isJump())
//                    continue;
                Application::logStream() << mn.move().toString();
                Application::logStream() << " ";
            }
            Application::logStream() << std::endl;
        }
        ddg_.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);
#endif

        Application::logStream() << "cycles: " << cycleCount << std::endl;
            
        Application::logStream() 
            << "minimum: " << minScheduleLength_ << " (from DDG longest path)"
            << std::endl;

        Application::logStream() 
            << "at least " 
            << resourceConstrained_ << " of " << nodes
            << " were resource constrained:" << std::endl;
        if (busConstrained_ > 0) {
            Application::logStream() 
                << busConstrained_ << " bus (move slot) constrained" 
                << std::endl;
        }

        if (rfReadPortConstrained_ > 0) {
            Application::logStream()             
                << rfReadPortConstrained_ << " RF read port constrained ";
            for (RFCountMap::const_iterator rfi = 
                     readPortConstrainedRfs_.begin(); 
                 rfi != readPortConstrainedRfs_.end(); ++rfi) {
                Application::logStream() 
                    << (*rfi).first << ": " << (*rfi).second << "  ";
            }
            Application::logStream() << std::endl;                
        }

        if (rfWritePortConstrained_ > 0) {
            Application::logStream() 
                << std::endl
                << rfWritePortConstrained_ << " RF write port constrained ";
            for (RFCountMap::const_iterator rfi = 
                     writePortConstrainedRfs_.begin(); 
                 rfi != writePortConstrainedRfs_.end(); ++rfi) {
                Application::logStream() 
                    << (*rfi).first << ": " << (*rfi).second << "  ";
            }
            Application::logStream() << std::endl;                
        }

        if (operationConstrained_ > 0) {
            Application::logStream() 
                << std::endl
                << operationConstrained_ 
                << " operation (FU) constrained ";
            for (OperationCountMap::const_iterator rfi = 
                     operationConstrainedMoves_.begin(); 
                 rfi != operationConstrainedMoves_.end(); ++rfi) {
                Application::logStream() 
                    << (*rfi).first << ": " << (*rfi).second << "  ";
            }
            Application::logStream() << std::endl;
        }
        Application::logStream() << std::endl;
        return true;
    }

    return regAntidepsFound;
#endif
}

/**
 * Analyses the program based on a non-resource constrained schedule.
 *
 * Produces statistics of maximum number of parallel operations that
 * can be potentially utilized. 
 *
 * @todo this is incomplete as the operand to trigger edges are missing,
 * thus the analysis results are likely to be false.
 */
void
ResourceConstraintAnalyzer::optimalScheduleResourceUsage(
    DataDependenceGraph& ddg, TCEString graphName) {

    ddg.writeToDotFile(graphName + ".dot");

    TCEString dotFileName = graphName + ".optimal_schedule.dot";
    std::ofstream s(dotFileName.c_str());
    
    int smallestCycle = 0;
    int largestCycle = 0;
    std::map<int, std::list<MoveNode*> > schedule;

    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    for (int nc = 0; nc < ddg.nodeCount(); ++nc) {
        MoveNode& n = ddg.node(nc);
        int cycle = ddg.maxSourceDistance(n);
        // immediate operand moves always get cycle 0, fix this by
        // assuming the cycle is the same as the latest other operand
        // move (at least one of them must be a non-immediate move)
        if (n.isDestinationOperation() && 
            n.move().source().isImmediate()) {
            for (int input = 0; 
                 input < n.destinationOperation().inputMoveCount();
                 ++input) {
                MoveNode& inputMove = 
                    n.destinationOperation().inputMove(input);
                if (&inputMove == &n) continue;
                cycle = std::max(cycle, ddg.maxSourceDistance(inputMove));
            }
        }
        largestCycle = std::max(cycle, largestCycle);
        schedule[cycle].push_back(&n);
    }
    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);

    s << "digraph " << graphName.replaceString(".", "_") << " {" << std::endl;


    s << "/*" << std::endl << "### dependence constraints: " << std::endl << std::endl;

    analyzeRegisterAntideps(ddg, s);


    // record stats of operationA -> operationB bypasses to guide
    // IC customization
    typedef std::map<std::pair<TCEString, TCEString>, int> BypassMap;

    typedef std::map<TCEString, int> OpCountMap;
    OpCountMap maxParallelOps;
    // The operation mix. I.e., the static occurence of operations
    // in the code.
    OpCountMap operationMix;
    BypassMap bypasses;

    /* In the critical path DDGs, the trigger nodes might be
       missing. In that case we use the operand node to record
       the operation usage cycle. This set is used to ensure the
       same operation is not recorded more than once. */
    std::set<ProgramOperation*> recordedOps;

    for (int c = smallestCycle; c <= largestCycle; ++c) {
        std::list<MoveNode*> moves = schedule[c];
        if (moves.size() == 0) continue;

        std::map<TCEString, int> parallelOpsAtCycle;
        for (std::list<MoveNode*>::iterator i = moves.begin(); 
             i != moves.end(); ++i) {
            MoveNode& n = **i;        
            TCEString opName = "";

            if (n.isBypass()) {
                std::pair<TCEString, TCEString> key =
                    std::make_pair(
                        n.sourceOperation().operation().name(),
                        n.destinationOperation().operation().name());
                bypasses[key]++;
            }

            if (n.isDestinationOperation()) {
                ProgramOperation& op = n.destinationOperation();
                if (!AssocTools::containsKey(recordedOps, &op)) {
                    opName = op.operation().name();
                    operationMix[opName]++;
                    parallelOpsAtCycle[opName]++;
                    recordedOps.insert(&op);
                }
            }
        }

        for (OpCountMap::const_iterator i = parallelOpsAtCycle.begin();
             i != parallelOpsAtCycle.end(); ++i) {
            TCEString opName = (*i).first;
            int count = (*i).second;
            maxParallelOps[opName] = 
                std::max(maxParallelOps[opName], count);
        }
    }

    s << std::endl;
    s << "### resource statistics: " << std::endl << std::endl;

    const int COL_WIDTH = 14;
    // print statistics of the graph as a comment
    s << std::setw(COL_WIDTH) << std::right << "operation stats: ";
    s << std::endl << std::endl;
        
    for (OpCountMap::const_iterator i = maxParallelOps.begin();
         i != maxParallelOps.end(); ++i) {
        TCEString opName = (*i).first;
        int parCount = (*i).second;
        int total = operationMix[opName];
        s << std::setw(COL_WIDTH) << std::right 
          << opName + ": ";
        s << std::setw(COL_WIDTH) << std::right 
          << total;
        s << " total, " << std::setw(COL_WIDTH) << std::right 
          << parCount << " at most in parallel" << std::endl;
    }

    s << std::endl;
    // print statistics of the graph as a comment
    s << std::setw(COL_WIDTH) << std::right << "bypass stats: ";
    s << std::endl << std::endl;
    
    for (BypassMap::const_iterator i = bypasses.begin(); 
         i != bypasses.end(); ++i) {
        std::pair<TCEString, TCEString> opPair = (*i).first;
        int count = (*i).second;
        s << std::setw(COL_WIDTH) << std::right 
          << opPair.first + " -> " + opPair.second + ": ";
        s << std::setw(COL_WIDTH) << std::right 
          << count << std::endl;
    }

    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);

    s << std::endl << "### moves not at the earliest cycles:" << std::endl << std::endl;
    for (int i = 0; i < ddg.nodeCount(); ++i) {
        MoveNode& n = ddg.node(i);
        // Constant writes always report 0 for the src distance. 
        if (n.isSourceConstant()) continue; 
        int ddgEarliest = ddg.maxSourceDistance(n);
        if (n.cycle() > ddgEarliest)
            s << n.toString() << " (" << n.cycle() << " > " << ddgEarliest << ")" << std::endl;
    }
    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);

    s << std::endl << "*/" << std::endl << std::endl;

    // print the DDG itself

    // print the "time line" to visualize the schedule
    s << "\t{" << std::endl
      << "\t\tnode [shape=plaintext];" << std::endl
      << "\t\t";
    for (int c = smallestCycle; c <= largestCycle; ++c) {
        s << "\"cycle " << c << "\" -> ";
    }
    s << "\"cycle " << largestCycle + 1 << "\"; " 
      << std::endl << "\t}" << std::endl;
    
    // print the nodes
    for (int c = smallestCycle; c <= largestCycle; ++c) {
        std::list<MoveNode*> moves = schedule[c];
        if (moves.size() > 0) {
            s << "\t{ rank = same; \"cycle " << c << "\"; ";
            for (std::list<MoveNode*>::iterator i = moves.begin(); 
                 i != moves.end(); ++i) {
                MoveNode& n = **i;        
                s << "n" << n.nodeID() << "; ";
            }
            s << "}" << std::endl;
        }
    }

    // first print all the nodes and their properties
    for (int i = 0; i < ddg.nodeCount(); ++i) {
        MoveNode& n = ddg.node(i);
        s << "\tn" << n.nodeID()
          << " [" << n.dotString() << "]; " 
          << std::endl;
    }

    // edges
    for (int count = ddg.edgeCount(), i = 0; i < count ; ++i) {
        DataDependenceEdge& e = ddg.edge(i);
        MoveNode& tail = ddg.tailNode(e);
        MoveNode& head = ddg.headNode(e);

        s << "\tn" << tail.nodeID() << " -> n" 
          << head.nodeID() << "[" 
          << e.dotString() << "];" << std::endl;
    }

    s << "}" << std::endl;   

    s.close();
}


/**
 * Analyzes the register antideps in the schedule.
 *
 * @return true in case at least one move that was constrained by a
 * register antidep which could be avoided with additional registers or
 * better register assignment strategy was found.
 */
bool
ResourceConstraintAnalyzer::analyzeRegisterAntideps(
    DataDependenceGraph& ddg, std::ostream& s) {

    s << "DDG largest cycle: " << ddg.largestCycle() << std::endl;

    
    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    s << "DDG height: " << ddg.height() << std::endl;
    ddg.setEdgeWeightHeuristics(DataDependenceGraph::EWH_DEFAULT);

    DataDependenceGraph* trueDepGraph = ddg.trueDependenceGraph(false);
    trueDepGraph->setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    s << "DDG height without register antideps: " 
      << trueDepGraph->height() << std::endl;

#if 0
    static int counter = 0;
    trueDepGraph->writeToDotFile((boost::format("tddg-noreg-%d.dot") % counter).str());
#endif

    DataDependenceGraph* trueDepGraph2 = ddg.trueDependenceGraph(true);
    trueDepGraph2->setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    s << "DDG height without any antideps: " 
      << trueDepGraph2->height() << std::endl;

#if 0
    trueDepGraph2->writeToDotFile(
        (boost::format("tddg-noantidep-%d.dot") % counter).str());
#endif
    delete trueDepGraph2; 

    DataDependenceGraph* trueDepGraph3 = ddg.trueDependenceGraph(true, true);
    trueDepGraph3->setEdgeWeightHeuristics(DataDependenceGraph::EWH_REAL);
    s << "DDG height without any antideps and memory deps: " 
      << trueDepGraph3->height() << std::endl;

#if 0
    trueDepGraph3->writeToDotFile(
        (boost::format("tddg-noantidep-nonmemdep-%d.dot") % counter).str());
    counter++;
#endif

    delete trueDepGraph3; 

    std::map<int, int> foundCounts;

    bool restrictingEdgeFound = false;

    for (int i = 0; i < ddg.nodeCount(); i++) {
        MoveNode& tail = ddg.node(i);
        for (int e = 0; e < ddg.outDegree(tail); ++e) {
            DataDependenceEdge& edge = ddg.outEdge(tail,e);
            // only consider non-loop register false deps here
            if (!edge.isFalseDep() ||
                edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
                edge.loopDepth() > 0)
                continue;
            MoveNode& head = ddg.headNode(edge);
            // if there is a *real* dependency path from the tail to head 
            // than the given antidep then it does not help if we rename 
            // the register
            if (trueDepGraph->hasPath(tail, head))
                continue;

            // TODO: figure out why the exception gets thrown (see trunk
            // Bug 517) and remove the try catch
            try {

            if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                if (tail.move().source().isGPR()) {
                    foundCounts[tail.move().source().registerFile().width()]++;
                } else if (dynamic_cast<const TTAMachine::RegisterGuard*>(
                               &tail.move().guard().guard())) {
                    const TTAMachine::RegisterGuard* g =
                        dynamic_cast<const TTAMachine::RegisterGuard*>(
                            &tail.move().guard().guard());
                    foundCounts[g->registerFile()->width()]++;
                } else {
                    Application::logStream() 
                        << "WARNING: not a GPR: " 
                        << tail.move().toString() << std::endl;
                }
            } else {
                if (tail.move().destination().isGPR()) {
                    foundCounts[tail.move().destination().registerFile().width()]++;
                } else {
                    Application::logStream() 
                        << "WARNING: not a GPR: " 
                        << tail.move().toString() << std::endl;
                }
            }

            } catch (const Exception& e) {
                // just skip this for now, have to take a look why
                // there are some extra R_G_war edges, example:
                // ERROR: Move is not predicated. Edge R_G_war:bool.0 
                // between moves: 32 -> add_sub_2.in1t.add eq_gt_gtu.out1 -> bool.0
                if (Application::verboseLevel() > 0) {
                    Application::logStream() 
                        << "ERROR: " << e.errorMessage() << " "
                        << "Edge " << edge.toString() << " between moves: "
                        << tail.move().toString() << " "
                        << head.move().toString() << std::endl;
                }
                continue;
                    
            }
#if 0
            Application::logStream()
                << "restricting antidep: " << edge.toString() << std::endl;
#endif
        }
    }
    for (std::map<int, int>::iterator i = foundCounts.begin(); 
         i != foundCounts.end(); ++i) {
        int foundCount = (*i).second;
        int bitWidth = (*i).first;
        s << "found " << foundCount << " " << bitWidth << " bit "
          << " register antideps that restrict parallelism" << std::endl;

    }
    delete trueDepGraph; 
    return restrictingEdgeFound;
}

/**
 * Finds a move that is a (grand)parent of the given move (or the move itself)
 * and constraints the schedule, that is, could be scheduled before 
 * the minimum schedule length in case a resource constraint was removed.
 *
 * A recursive function. 
 *
 * @return a found parent or NULL if not found in that subtree.
 */
MoveNode*
ResourceConstraintAnalyzer::findResourceConstrainedParent(MoveNode* child) {


    if (child == NULL || !child->isMove())
        return NULL;

    int ddgEarliestCycle = origDDG_.earliestCycle(*child);
    if (ddgEarliestCycle < minScheduleLength_ &&  
        foundMoves_.find(child) == foundMoves_.end() &&
        analyzeMoveNode(*child)) {
        return child;
    } else {
        DataDependenceGraph::NodeSet parents = origDDG_.predecessors(*child);
        for (DataDependenceGraph::NodeSet::const_iterator p = parents.begin();
             p != parents.end(); ++p) {
            MoveNode* mn = *p;
            if (findResourceConstrainedParent(mn) != NULL)
                return mn;
        }
    }
    return NULL;
}

/**
 * Analyzes the resource constraints of a single move in the schedule.
 *
 * @return true in case the move is resource constrained.
 */
bool
ResourceConstraintAnalyzer::analyzeMoveNode(const MoveNode& node) {

    // moves at cycle 0 cannot be scheduled earlier
    if (node.cycle() == 0)
        return false;

    int ddgEarliestCycle = origDDG_.earliestCycle(node);
    if (ddgEarliestCycle == node.cycle()) 
        return false;

    if (node.move().isJump()) {
        Application::logStream() 
            << "earliest cycle for JUMP: " << ddgEarliestCycle << std::endl;
        // JUMP cannot be scheduled earlier than the
        // schedule length - delay slots, they always have a fixed position
        // in the BB
        return false;
    }

    // operation latency
    // look for the triggering operand and see if it's less
    // than the latency away
    if (node.isSourceOperation()) {
        ProgramOperation& op = node.sourceOperation();
        MoveNode& trigger = *op.triggeringMove();
        MoveNode& opcodeSetting = op.opcodeSettingNode();
        int latency = 
            (dynamic_cast<TTAProgram::TerminalFUPort&>(
                opcodeSetting.move().destination())).hwOperation()->
            latency();
        if (node.cycle() == trigger.cycle() + latency) {
            return false;
        } 
    }

    const TTAMachine::Machine& targetMachine = origDDG_.machine();

    // check the instruction the move should have been scheduled at, 
    // resources permitting
    TTAProgram::Instruction* prevInstr = rm_.instruction(ddgEarliestCycle);

    const int busCount = targetMachine.busNavigator().count();
    // is it bus constrained? 
    if (prevInstr->moveCount() == busCount) {
        // it might be true or we just managed to find a different move 
        // to fill the unused slot later during scheduling
        ++busConstrained_;
        return true;
    }

    // RF read port constrained?
    if (node.move().source().isGPR()) {
        int readsFound = 0;
        for (int m = 0; m < prevInstr->moveCount(); ++m) {
            TTAProgram::Move& prevMove = prevInstr->move(m);
            if (prevMove.source().isGPR() && 
                &node.move().source().registerFile() ==
                &prevMove.source().registerFile()) {
                ++readsFound;
            }
        }
        if (readsFound == 
            node.move().source().registerFile().maxReads()) {
            ++rfReadPortConstrained_;
            readPortConstrainedRfs_[
                node.move().source().registerFile().name()]++;
            return true;
        }
    }

    // RF read port constrained?
    if (node.move().destination().isGPR()) {
        int writesFound = 0;
        for (int m = 0; m < prevInstr->moveCount(); ++m) {
            TTAProgram::Move& prevMove = prevInstr->move(m);
            if (prevMove.destination().isGPR() && 
                &node.move().destination().registerFile() ==
                &prevMove.destination().registerFile()) {
                ++writesFound;
            }
        }
        if (writesFound == 
            node.move().destination().registerFile().maxWrites()) {
            ++rfWritePortConstrained_;
            writePortConstrainedRfs_[
                node.move().destination().registerFile().name()]++;
            return true;
        }
    }

    // assume rest are FU constrained somehow (might be untrue in general,
    // but with a fully connected machine a good approximation)
    if (node.isDestinationOperation()) {
        // if it's a trigger node, it can be contrained by an operand move, 
        // a dependency which is not recorded by the DDG
        bool operandConstrained = false;
        if (node.move().isTriggering()) {
            ProgramOperation& po = node.destinationOperation();
            for (int input = 0; (input < po.inputMoveCount()) && 
                     !operandConstrained; ++input) {
                
                const MoveNode& operandMove = po.inputMove(input);
                
                if (&operandMove == &node)
                    continue;

                if (operandMove.cycle() == node.cycle()) {
                    operandConstrained = true;
                    break;
                }
            }
        }

        if (!operandConstrained) {
            ++operationConstrained_;
            TCEString opName = node.destinationOperation().operation().name();
#if 0
            Application::logStream()
                << opName << " constrained: "
                << node.move().toString() << std::endl;
#endif
            operationConstrainedMoves_[opName]++;
            return true;
        } else if (operandConstrained){
            return false; // operand depedency is constrained by the trigger
        }
    }

    unknownConstrained_++;
    Application::logStream()
        << std::endl
        << "resource set constrained move of unknown reason: " 
        << node.toString() << std::endl;
    if (node.isSourceOperation()) {
        ProgramOperation& op = node.sourceOperation();
        Application::logStream()
            << "ProgramOperation: " << op.toString() << std::endl;
    }
    Application::logStream() 
        << "earliest cycle: " << origDDG_.earliestCycle(node)
        << std::endl;

    
    for (int c = 3; c >= 0; --c) {
        int printCycle = node.cycle() - c;
        if (printCycle > 0) {
            Application::logStream()
                << printCycle << ": "
                << rm_.instruction(printCycle)->toString()
                << std::endl;
        }
    }
    return true;    
}
