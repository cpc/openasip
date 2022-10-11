#include "LoopAnalyzer.hh"
#include "Move.hh"
#include "MoveNodeSet.hh"
#include "Operation.hh"
#include "Terminal.hh"
#include "Instruction.hh"
#include "BasicBlock.hh"
#include "BasicBlockNode.hh"
#include "MoveGuard.hh"

//#define DEBUG_LOOP_ANALYZER

LoopAnalyzer::LoopAnalysisResult* 
LoopAnalyzer::analyze(BasicBlockNode& bbn, DataDependenceGraph& ddg) {
    MoveNode* jumpMove = NULL;

#ifdef DEBUG_LOOP_ANALYZER
    TCEString dotName("loop_analyzis");
    dotName << ddg.name();
    dotName << ".dot";
    ddg.writeToDotFile(dotName);
#endif

    TTAProgram::BasicBlock& bb = bbn.basicBlock();
    for (int i = bb.instructionCount()-1; i >= 0;i--) {
        TTAProgram::Instruction& ins = bb[i];
        for (int j = 0; j < ins.moveCount(); j++) {
            TTAProgram::Move& move = ins.move(j);
            if (move.isJump()) {
                jumpMove = &ddg.nodeOfMove(move);
            }
        }
    }
    if (jumpMove == NULL) {
        return NULL;
    }

    MoveNode* guardDef = ddg.onlyGuardDefOfMove(*jumpMove);
    if (guardDef == NULL) {
        return NULL;
    }
    while (guardDef->isSourceVariable()) {
        guardDef = ddg.onlyRegisterRawSource(*guardDef);
        if (guardDef == NULL) {
            return NULL;
        }
    }
    // find guard def op.
    if (!guardDef->isSourceOperation()) {
        return NULL;
    }
    ProgramOperation* gdpo = &guardDef->sourceOperation();
    bool inverted = jumpMove->move().guard().isInverted();
    
    // machine does not have ne operation so eq + xor used.
    if (gdpo->operation().name() == "XOR") {
        MoveNodeSet& input1Set = gdpo->inputNode(1);
        MoveNodeSet& input2Set = gdpo->inputNode(2);
        if (input1Set.count() != 1 || input2Set.count() != 1) {
            return NULL;
        }
        MoveNode& input1 = input1Set.at(0);
        MoveNode& input2 = input2Set.at(0);
        
        MoveNode *regMove = NULL;
        if (input1.move().source().isGPR() && 
            input2.move().source().isImmediate()) {
            regMove = &input1;
        } 
        
        if (input2.move().source().isGPR() && 
            input1.move().source().isImmediate()) {
            regMove = &input2;
        }
        
        // bypassed directly from eq?
        if (input1.isSourceOperation() &&
            input2.move().source().isImmediate()) {
            gdpo = &input1.sourceOperation();
        } else {
            if (input2.isSourceOperation() &&
                input1.move().source().isImmediate()) {
                gdpo = &input2.sourceOperation();
            } else { // not bypassed.
                if (regMove == NULL) {
                    return NULL;
                }
                MoveNode* xorSrc = ddg.onlyRegisterRawSource(*regMove);
                if (xorSrc != NULL && xorSrc->isSourceOperation()) {
                    inverted = !inverted;
                    gdpo = &xorSrc->sourceOperation();
                } else {
                    return NULL;
                }
            }
        }
    }
    // now we should have the comparison op.
    // supported comparisons:

    // 0 -> N: x  ne N -> jump
    // 0 -> N: x  lt N -> jump
    // 0 -> N: N gt x -> jump
    // and same with inverted. (eq, geu, leu)

    // N -> 0: x  ne 0 -> jump
    // N -> 0: x  gt 0 -> jump
    // N -> 0: 0 lt x -> jump
    // and same with inverted (eq, ltu, geu)
    bool gtu = false;
    bool ltu = false;

#ifndef DEBUG_LOOP_ANALYZER
    if (Application::verboseLevel() > 1) {
#endif
        std::cerr << "\tLoop comparison op is: " <<gdpo->toString()<< std::endl;
#ifndef DEBUG_LOOP_ANALYZER
    }
#endif

    if (!((gdpo->operation().name() == "NE" && !inverted) ||
          (gdpo->operation().name() == "EQ" && inverted))) {
        if ((gdpo->operation().name() == "GTU" && !inverted) ||
            (gdpo->operation().name() == "LEU" && inverted)) {
            gtu = true;
        } else {
            if ((gdpo->operation().name() == "LTU" && !inverted) ||
                (gdpo->operation().name() == "GEU" && inverted)) {
                ltu = true;
            } else {
                // unsupported comparison operation.
#ifndef DEBUG_LOOP_ANALYZER
                if (Application::verboseLevel() > 1) {
#endif
                    std::cerr <<
                        "Unsupported comparison op for loop analyzer." <<
                        std::endl;
#ifndef DEBUG_LOOP_ANALYZER
                }
#endif
                return NULL;
            }
        }
    }

    MoveNode* constCmp = NULL;
    MoveNode* regCmp1 = NULL;
    MoveNode* regCmp2 = NULL;
    MoveNode* onlyRegCmp = NULL;
    
    MoveNodeSet& input1Set = gdpo->inputNode(1);
    MoveNodeSet& input2Set = gdpo->inputNode(2);
    if (input1Set.count() != 1 || input2Set.count() != 1) {
        return NULL;
    }
    MoveNode& input1 = input1Set.at(0);
    MoveNode& input2 = input2Set.at(0);
    if (input1.move().source().isGPR()) {
        onlyRegCmp = regCmp1 = &input1;
    } else if (input1.move().source().isImmediate() || 
               input1.move().source().isImmediateRegister()) {
        constCmp = &input1;
    }
    
    if (input2.move().source().isGPR()) {
        regCmp2 = &input2;
        if (onlyRegCmp == NULL) {
            onlyRegCmp = regCmp2;
        } else {
            onlyRegCmp = NULL;
        }
    } else if (input2.move().source().isImmediate() || 
               input2.move().source().isImmediateRegister()) {
        if (constCmp != NULL) {
            return NULL;
        }
        constCmp = &input2;
    }
    
    int endVal;
    // case 1: Either one is constant.
    if (constCmp) {
        if (constCmp == &input1) {
            // operand 1 of comparison is limit. this means
            // comparison operands are "swapped".
            if (gtu) {
                ltu = true;
                gtu = false;
            } else if (ltu) {
                ltu = false;
                gtu = true;
            }
        }

        TTAProgram::Terminal& src = constCmp->move().source();
        if (src.isImmediateRegister()) {
            return NULL; // not yet supported
        } 
        endVal = src.value().intValue();
        InitAndUpdate* initAndUpdate = findInitAndUpdate(ddg, *onlyRegCmp);
        if (initAndUpdate == NULL) {
            return NULL;
        }
        int update = initAndUpdate->update;
        if (initAndUpdate->initNode != NULL) {
            // goes from X to zero.
            if (update  <0 && endVal == 0) {
                if (ltu) return NULL;
                return new LoopAnalysisResult(
                    initAndUpdate->loopEdge, initAndUpdate->initNode,-update);
            } else {
                if (update < 0 && abs(endVal) < -update) {
                    if (ltu) return NULL;
                    return new LoopAnalysisResult(
                        initAndUpdate->loopEdge,
                        initAndUpdate->initNode, -update);
                } else {
                    if (ltu) return NULL;
                    if (endVal < 0) {
                        return new LoopAnalysisResult(
                            initAndUpdate->loopEdge + (endVal/update),
                            initAndUpdate->initNode, -update);
                    }
                }
                return NULL;
            }
        }
        int diff = endVal - initAndUpdate->initVal;
        if (diff % update != 0) {
            return NULL;
        }
        if (update < 0 && gtu) return NULL;
        if (update > 0 && ltu) return NULL;
        return new LoopAnalysisResult(
            (diff / update) + initAndUpdate->loopEdge, NULL);

    } else { // case 2: both are registers.
        InitAndUpdate* initAndUpdate = findInitAndUpdate(ddg, *regCmp1);
        EndCondition* endCond = NULL;
        if (initAndUpdate == NULL) {
            initAndUpdate = findInitAndUpdate(ddg, *regCmp2);
            if (initAndUpdate == NULL) {
                return NULL;
            }
            // operand 1 of comparison is limit. this means
            // comparison operand are "swapped".
            if (gtu) {
                ltu = true;
                gtu = false;
            } else if (ltu) {
                ltu = false;
                gtu = true;
            }

            endCond = findEndCond(ddg, *regCmp1, false);
            if (endCond == NULL) {
                endCond = findEndCond(ddg, *regCmp1, true);
            }
        } else {
            endCond = findEndCond(ddg, *regCmp2, false);
            if (endCond == NULL) {
                endCond = findEndCond(ddg, *regCmp2, true);
            }
        }
        if (endCond == NULL) {
#ifndef DEBUG_LOOP_ANALYZER
            if (Application::verboseLevel() > 1) {
#endif
                std::cerr << "Could not analyze end condition" << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
            }
#endif
            // Could not analyze endcond
            return NULL;
        }

        // init from variable.
        if (initAndUpdate->initNode != NULL) {
            int update = initAndUpdate->update;
            // go down from variable to 0.
            if (endCond->first == 0 && endCond->second == NULL) {
                // no update value of wrong comparison
                if (ltu || update >= 0 ) return NULL;
                if (update == -1 || gtu) {
                    return new LoopAnalysisResult(
                        initAndUpdate->loopEdge,
                        initAndUpdate->initNode, -update);
                }
            }
            // both variables, but calculated from same source
            // with add or sub operation.
            if (endCond->second && initAndUpdate->initNode) {
                EndCondition* commonAncestor =
                    tryTrackCommonAncestor(
                        ddg, *initAndUpdate->initNode, *endCond->second);
                if (commonAncestor != NULL) {
                    if (commonAncestor->second == NULL) {
                        if (update < 0 && ltu) return NULL;
                        if (update > 0 && gtu) return NULL;
                        return new LoopAnalysisResult(
                            commonAncestor->first/update +
                            initAndUpdate->loopEdge);
                    }
                }
            }
            return NULL;
        }

        if (endCond->second == NULL) {
#ifndef DEBUG_LOOP_ANALYZER
            if (Application::verboseLevel() > 1) {
#endif
                std::cerr << "End condition is constant:" << endCond->first << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
            }
#endif
            int diff = endCond->first - initAndUpdate->initVal;
            int update = initAndUpdate->update;
            if (diff % update != 0) {
                return NULL;
            }
            if (update < 0 && ltu) return NULL;
            if (update > 0 && gtu) return NULL;
            return new LoopAnalysisResult((diff / update) + initAndUpdate->loopEdge, NULL);
        } else {

#ifndef DEBUG_LOOP_ANALYZER
            if (Application::verboseLevel() > 1) {
#endif
            std::cerr << "\tLoop end register might be: " << endCond->second->toString() << std::endl;
            std::cerr << "\tEnd cond first is: " << endCond->first << std::endl;
            std::cerr << "\t init is: " << initAndUpdate->initVal
                      << " and update is: " << initAndUpdate->update << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
            }
#endif
            if (initAndUpdate->initVal != 0) {
#ifndef DEBUG_LOOP_ANALYZER
                if (Application::verboseLevel() > 1) {
#endif
                    std::cerr << "Variable loop counts with init !=0 not yet supported" << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
                }
#endif
                return NULL;
            }
            return new LoopAnalysisResult(initAndUpdate->loopEdge, endCond->second, initAndUpdate->update);
        }
      
    }

    return NULL;
}


LoopAnalyzer::EndCondition*
LoopAnalyzer::findEndCond(DataDependenceGraph& ddg, MoveNode& cmpVal, bool allowVariable) {
#ifdef DEBUG_LOOP_ANALYZER
    std::cerr << "\tSearching end cond, allow var: " << allowVariable
              << " from: " << cmpVal.toString() << std::endl;
#endif
    if (cmpVal.isSourceConstant()) {
        return new EndCondition(
            cmpVal.move().source().value().intValue(), NULL);
    }
    MoveNode* cmpSrc = ddg.onlyRegisterRawSource(cmpVal);
    if (cmpSrc == NULL) {
#ifdef DEBUG_LOOP_ANALYZER
        std::cerr << "\t\tNo only RRAW source found, returning this or NULL"
                  << std::endl;
#endif
        if (cmpVal.move().source().isGPR() && allowVariable) {
            return new EndCondition(0, &cmpVal);
        } else {
            return NULL;
        }
    }

    if (cmpSrc->isSourceConstant()) {
        return new EndCondition(
            cmpSrc->move().source().value().intValue(), NULL);
    }

    if (&ddg.getBasicBlockNode(cmpVal) != &ddg.getBasicBlockNode(*cmpSrc)
        && allowVariable) {
#ifdef DEBUG_LOOP_ANALYZER
        std::cerr << "cmp val" << cmpVal.toString() << " and cmpsrc "
                  << cmpSrc->toString() << " has different BB, ret cmpval"
                  << std::endl;
#endif
        return new EndCondition(0, &cmpVal);
    }

    if (cmpSrc->isSourceVariable()) {
        return findEndCond(ddg, *cmpSrc, allowVariable);
    }

    if (!cmpSrc->isSourceOperation()) {
        if (allowVariable) {
            return new EndCondition(0, &cmpVal);
        } else {
            return NULL;
        }
    }

    ProgramOperation& po = cmpSrc->sourceOperation();    
    bool isSub = po.operation().name() == "SUB";
    bool isAdd = po.operation().name() == "ADD";
    if (!isSub && !isAdd) {
#ifdef DEBUG_LOOP_ANALYZER
        std::cerr << "\t\tFind End Cond unknown op, returning NULL" << std::endl;
#endif
        return NULL;
    }
    
    MoveNodeSet& input1Set = po.inputNode(1);
    MoveNodeSet& input2Set = po.inputNode(2);
    if (input1Set.count() != 1 || input2Set.count() != 1) {
        if (allowVariable) { // TODO: is this also borken?
            return new EndCondition(0, &cmpVal);
        } else {
            return NULL;
        }
    }
    MoveNode& input1 = input1Set.at(0);
    MoveNode& input2 = input2Set.at(0);
        
    EndCondition* tmpRes2 = findEndCond(ddg, input2, allowVariable);
    if (tmpRes2 == NULL) {
        return NULL;
    }
    if (tmpRes2->second == NULL) {
        int value = tmpRes2->first;
        if (isSub) {
            value *= -1;
        }

        EndCondition* tmpRes1 = findEndCond(ddg, input1, allowVariable);
        if (tmpRes1 == NULL) {
            return NULL;
        }
        tmpRes1->first += value;
        return tmpRes1;
    } else { // 2 comes from a reg.
        // sub by register getting too cmplex, giving up.
        if (isSub) {
            if (allowVariable) {
                return new EndCondition(0, &cmpVal);
            } else {
                return NULL;
            }
        }
        EndCondition* tmpRes1 = findEndCond(ddg, input1, allowVariable);
        if (tmpRes1 == NULL) {
            return NULL;
        }
        if (tmpRes1->second == NULL) {
            tmpRes2->first += tmpRes1->first;
            return tmpRes2;
        } else { // both from reg.. return me.
            return new EndCondition(0, &cmpVal);
        }
    }
    // should never be here
    assert(false);
}



LoopAnalyzer::InitAndUpdate*
LoopAnalyzer::findInitAndUpdate(DataDependenceGraph& ddg, MoveNode& cmpVal) {
    // TODO: looping and non-looping update
#ifndef DEBUG_LOOP_ANALYZER
    if (Application::verboseLevel() > 1) {
#endif
        std::cerr << "\tSearching init and update from cmpval: "
                  << cmpVal.toString() << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
    }
#endif

    MoveNode* cmpSrc = ddg.onlyRegisterRawSource(cmpVal,0);
    MoveNode* backCmpSrc = ddg.onlyRegisterRawSource(cmpVal,1);

    if (cmpSrc == NULL) {
        MoveNode* fwdCmpSrc = ddg.onlyRegisterRawSource(cmpVal,2);
        if (backCmpSrc && fwdCmpSrc) {
            // update happens after comparison, this means
            // iteration count is one more.
            cmpSrc = backCmpSrc;
        } else {
            return NULL;
        }
    }

    MoveNode* originalCmpSrc = cmpSrc;

    if (!cmpSrc->isSourceOperation()) {
        MoveNode* cmpSrcSrc = ddg.onlyRegisterRawSource(*cmpSrc);
        if (cmpSrcSrc == NULL) {
            return NULL;
        } else {
            if (!cmpSrcSrc->isSourceOperation()) {
                return NULL;
            } else {
                cmpSrc = cmpSrcSrc;
            }
        }
    }

    ProgramOperation& po = cmpSrc->sourceOperation();    
    bool isSub = po.operation().name() == "SUB";
    bool isAdd = po.operation().name() == "ADD";
    if (!isSub && !isAdd) {
        return NULL;
    }

    // is add or sub
    MoveNodeSet& input1Set = po.inputNode(1);
    MoveNodeSet& input2Set = po.inputNode(2);
    if (input1Set.count() != 1 || input2Set.count() != 1) {
        return NULL;
    }
    MoveNode& input1 = input1Set.at(0);
    MoveNode& input2 = input2Set.at(0);
    
    MoveNode *regMove = NULL;
    MoveNode *immMove = NULL;
    if (input1.move().source().isGPR() && 
        input2.move().source().isImmediate()) {
        regMove = &input1;
        immMove = &input2;
    } else if (input2.move().source().isGPR() && 
        input1.move().source().isImmediate()) {
        // no sub by variable
        if (isSub) {
            return NULL;
        }
        regMove = &input2;
        immMove = &input1;
    } else { // not yet supported.
        return NULL;
    }

    int updateVal = immMove->move().source().value().intValue();
    if (isSub) {
        updateVal *=-1;
    }


    MoveNode* updateSource = ddg.onlyRegisterRawSource(*regMove, 2, 1);
    if (updateSource == NULL ||
        (updateSource != cmpSrc && updateSource != originalCmpSrc)) {
        return NULL;
    }

    DataDependenceGraph::NodeSet initNodes =
        ddg.regRawPredecessors(*regMove, 2);
    MoveNode* counterInit =  NULL;
    if (initNodes.size() == 1) {
        counterInit = *initNodes.begin();
    } else if (initNodes.size() > 1) {
#ifdef DEBUG_LOOP_ANALYZER
        std::cerr << "multiple init nodes, must be dynamic " << std::endl;
#endif
        for (auto i = initNodes.begin(); i != initNodes.end(); i++) {
            std::cerr << "\t\t" << (**i).toString() << std::endl;
        }
        return new InitAndUpdate(*regMove, updateVal, backCmpSrc != 0);
    }

    if (counterInit == NULL) {
        return NULL;
    }
    int counterOffset = 0;
    while (true) {
        if (counterInit->isSourceVariable()) {
            counterInit = ddg.onlyRegisterRawSource(*counterInit);
            if (counterInit == NULL) {
                return new InitAndUpdate(*regMove, updateVal, backCmpSrc !=0);
            }
        } else { // not variable
#ifdef DEBUG_LOOP_ANALYZER
            std::cerr << "Coutner init src not var: " << counterInit->toString() << std::endl;
#endif
            if (counterInit->isSourceOperation()) {
#ifdef DEBUG_LOOP_ANALYZER
                std::cerr << "Coutner init src op: " << counterInit->toString() << std::endl;
#endif
                ProgramOperation& po = counterInit->sourceOperation();
                bool isSub = po.operation().name() == "SUB";
                bool isAdd = po.operation().name() == "ADD";
                if (!isSub && !isAdd) {
                    break;
                }

                // is add or sub
                MoveNodeSet& input1Set = po.inputNode(1);
                MoveNodeSet& input2Set = po.inputNode(2);
                if (input1Set.count() != 1 || input2Set.count() != 1) {
                    break;
                }
                MoveNode& input1 = input1Set.at(0);
                MoveNode& input2 = input2Set.at(0);
#ifdef DEBUG_LOOP_ANALYZER
                std::cerr << "inputs: " << input1.toString() << " and "
                          << input2.toString() << std::endl;
#endif
                if (input2.isSourceConstant()) {
#ifdef DEBUG_LOOP_ANALYZER
                    std::cerr << "input2 imm" <<  input2.toString() << std::endl;
#endif
                    if (isAdd) {
                        counterOffset += input2.move().source().value().intValue();
                    } else {// sisSub
                        counterOffset -= input2.move().source().value().intValue();
                    }
                    if (input1.isSourceConstant()) {
                        return new InitAndUpdate(input1.move().source().value().intValue()
                                                 + counterOffset, updateVal, backCmpSrc != 0);
                    } else {
                        counterInit = &input1;
                    }
                } else { // 2nd input not const, cannot track..
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    if (counterInit->isSourceConstant()) {
        return new InitAndUpdate(
            counterInit->move().source().value().intValue()+counterOffset,
            updateVal, backCmpSrc != 0);
    }
#ifndef DEBUG_LOOP_ANALYZER
    if (Application::verboseLevel() > 1) {
#endif
        std::cerr << "\t\tCounter init variable: " << counterInit->toString() << std::endl;
#ifndef DEBUG_LOOP_ANALYZER
    }
#endif
    return new InitAndUpdate(*counterInit, counterOffset, updateVal, backCmpSrc != 0);
}


LoopAnalyzer::EndCondition* LoopAnalyzer::tryTrackCommonAncestor(
    DataDependenceGraph& ddg,MoveNode& init, MoveNode& endCond) {

    MoveNode* i = &init;
    MoveNode* initSrc = ddg.onlyRegisterRawSource(*i);
    while (initSrc != NULL) {
        i = initSrc;
        initSrc = ddg.onlyRegisterRawSource(*i);
    }

    MoveNode* e = &endCond;
    MoveNode* eSrc = ddg.onlyRegisterRawSource(*e);
    while (eSrc != NULL) {
        e = eSrc;
        eSrc = ddg.onlyRegisterRawSource(*e);
    }

    if (e == NULL || i == NULL) {
        return NULL;
    }
    if (!e->isSourceOperation() ||
        (e->sourceOperation().operation().name() != "ADD" &&
         (e->sourceOperation().operation().name() != "SUB"))) {
        return NULL;
    }
    ProgramOperation& po = e->sourceOperation();
    MoveNodeSet& input1Set = po.inputNode(1);
    MoveNodeSet& input2Set = po.inputNode(2);
    if (input1Set.count() != 1 || input2Set.count() != 1) {
        return NULL;
    }
    MoveNode& input1 = input1Set.at(0);
    MoveNode& input2 = input2Set.at(0);

    if (!i->isSourceOperation()) {
        return NULL;
    }

    if (input1.isSourceOperation() && &input1.sourceOperation() ==
        &i->sourceOperation() && input1.move().source()==i->move().source()) {
        if (input2.move().source().isImmediate()) {
            int updateVal = input2.move().source().value().intValue();
            return new EndCondition(updateVal, NULL);
        }
    }

    if (input2.isSourceOperation() && &input2.sourceOperation() ==
        &i->sourceOperation() && input2.move().source()==i->move().source()) {
        if (input1.move().source().isImmediate()) {
            int updateVal = input1.move().source().value().intValue();
            return new EndCondition(updateVal, NULL);
        }

    }


    return NULL;
}
