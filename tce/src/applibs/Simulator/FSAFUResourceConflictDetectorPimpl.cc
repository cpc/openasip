/**
 * @file FSAFUResourceConflictDetectorPimpl.cc
 *
 * Definition of FSAFUResourceConflictDetectorPimpl (private implementation)
 * class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "FSAFUResourceConflictDetectorPimpl.hh"


FSAFUResourceConflictDetectorPimpl::FSAFUResourceConflictDetectorPimpl(
    const TTAMachine::FunctionUnit& fu) :
    fsa_(fu, true),
    currentState_(fsa_.startState()),  
    operationIssued_(false), NOP(fsa_.transitionIndex("[NOP]")), 
    fuName_(fu.name()) {
}


FSAFUResourceConflictDetectorPimpl::~FSAFUResourceConflictDetectorPimpl() {
}

