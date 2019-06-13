/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file FiniteStateAutomaton.cc
 *
 * Definition of FiniteStateAutomaton class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <sstream>
#include "Application.hh"
#include "FiniteStateAutomaton.hh"
#include "Conversion.hh"
#include "boost/format.hpp"
#include "FunctionUnit.hh"

const FiniteStateAutomaton::FSAStateIndex 
FiniteStateAutomaton::ILLEGAL_STATE;

const FiniteStateAutomaton::FSAStateIndex 
FiniteStateAutomaton::UNKNOWN_STATE;


/**
 * Constructor.
 *
 * @param defaultState The default state for state transitions (either 
 * ILLEGAL_STATE or UNKNOWN_STATE, as no other states are known at this 
 * point).
 * @param transitionCount The initial count of transitions.
 */
FiniteStateAutomaton::FiniteStateAutomaton(
    FSAStateTransitionIndex defaultState,
    int transitionCount) : 
    stateCount_(0), 
    transitionTypeCount_(transitionCount), transitionNames_(transitionCount),
    defaultState_(defaultState) {
}

/**
 * Destructor.
 */
FiniteStateAutomaton::~FiniteStateAutomaton() {
#if 0
    Application::logStream() << stateCount_ << " states" << std::endl;
#endif
}

/**
 * Returns the textual description of the transition at the given index.
 *
 * For example, returns the name of the operation in case of an FU FSA. 
 *
 * @param transition The index of the transition of which name to return.
 * @return The name of the transition.
 */
const std::string&
FiniteStateAutomaton::transitionName(
    FSAStateTransitionIndex transition) const {
    return transitionNames_.at(transition);
}

/**
 * Sets a name for the given transition.
 *
 * @param transition The index of the transition of which name to return.
 * @param name The name of the transition.
 */
void
FiniteStateAutomaton::setTransitionName(
    FSAStateTransitionIndex transition, const std::string& name) {
    transitionNames_.at(transition) = name;
    transitionIndices_[name] = transition;
}

/**
 * Returns the textual description of the state at the given index.
 *
 * @param state The index of the state of which name to return.
 * @return The name of the state.
 */
std::string
FiniteStateAutomaton::stateName(FSAStateIndex state) const {
    return Conversion::toString(state);
}

/**
 * Returns the index of the transition with given name.
 *
 * Returns the transition index for an operation in case of an FU FSA. 
 *
 * @param transitionName The name of the transition of index to return.
 * @return The index.
 * @exception KeyNotFound In case no transition with given is found.
 */
FiniteStateAutomaton::FSAStateTransitionIndex
FiniteStateAutomaton::transitionIndex(const std::string& transitionName) const {
    TransitionNameIndex::const_iterator i = 
        transitionIndices_.find(transitionName);

    if (i == transitionIndices_.end())
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, 
            (boost::format("No transition '%s'") % transitionName).str());

    return (*i).second;
}

/**
 * Returns the index of the state resulting from the given transition from 
 * the given source state.
 *
 * This method is called often, thus should be as fast as possible. 
 * Therefore, all range checking etc. is disabled. Method is not const
 * due to possibility of lazy initialization of states in derived
 * classes.
 *
 * @param source The source state.
 * @param transition The transition.
 * @return The index of the destination state.
 */
FiniteStateAutomaton::FSAStateIndex
FiniteStateAutomaton::destinationState(
    FSAStateIndex source, FSAStateTransitionIndex transition) {
    return transitions_[source][transition];    
}

/**
 * Returns true in case the given transition is legal (accepted by the
 * state machine) from the given state.
 *
 * This method is called often, thus should be as fast as possible. 
 * Therefore, all range checking etc. is disabled. Method is not const
 * due to possibility of lazy initialization of states in derived
 * classes.
 * 
 * @param source The source state.
 * @param transition The transition.
 * @return True in case the transition is legal.
 */
bool 
FiniteStateAutomaton::isLegalTransition(
    FSAStateIndex source,
    FSAStateTransitionIndex transition) {
    return transitions_[source][transition] != ILLEGAL_STATE;
}


/**
 * Adds a new state to the automaton.
 *
 * @return Returns the index of the new state. Indexing starts from 0.
 */
FiniteStateAutomaton::FSAStateIndex
FiniteStateAutomaton::addState() {
    // add transition vector with no transitions for the new state
    transitions_.push_back(
        TransitionVector(transitionTypeCount_, defaultState_));
    return stateCount_++;
}

/**
 * Adds a new transition type to the automaton.
 *
 * @param name Name of the transition type. Must be unique within all 
 * transition types.
 * @return Returns the index of the new transition type. Indexing starts 
 * from 0.
 */
FiniteStateAutomaton::FSAStateTransitionIndex
FiniteStateAutomaton::addTransitionType(const std::string& name) {    

    transitionNames_.push_back(name.c_str());
    transitionIndices_.insert(
        TransitionNameIndex::value_type(name, transitionTypeCount_));

    // increment the size of each state's transition vectors to
    // match the new count of transition types
    for (TransitionMap::iterator i = transitions_.begin(); 
         i != transitions_.end(); ++i) {
        TransitionVector& vec = *i;
        vec.resize(transitionTypeCount_ + 1, defaultState_);
    }

    return transitionTypeCount_++;
}

/**
 * Set a transition between two states.
 *
 * @param source The source state.
 * @param destination The destination state.
 * @param transition The type of transition.
 */
void
FiniteStateAutomaton::setTransition(
    FSAStateIndex source, 
    FSAStateIndex destination, 
    FSAStateTransitionIndex transition) {

    transitions_.at(source).at(transition) = destination;
}

/**
 * Creates a string that represents the FSA as a graph in GraphViz dot format.
 *
 * This string can be rendered to a visual graph using the 'dot' tool.
 *
 * @return The FSA in dot graph format.
 */
std::string 
FiniteStateAutomaton::toDotString() const {

    std::ostringstream s;
    s << "digraph G {" << std::endl;

    // first print all the states as nodes
    for (int i = 0; i < stateCount_; ++i) {
        s << "\tS" << i << " [label=\"S" << i << "\\n" << stateName(i) 
          << "\"]; " << std::endl;
    }

    // print transitions as edges
    for (int i = 0; i < stateCount_; ++i) {
        const TransitionVector& vec = transitions_.at(i);
        for (int t = 0; t < transitionTypeCount_; ++t) {
            std::string edgeLabel = transitionName(t);
            if (edgeLabel == "")
                edgeLabel = "[NOP]";
            edgeLabel = edgeLabel + " (" + Conversion::toString(t) + ")";
            switch (vec.at(t)) {
            case ILLEGAL_STATE:
                s << "\t/* No transition " << edgeLabel << " from S" << i 
                  << " possible */" << std::endl;
                break;
            case UNKNOWN_STATE:
                s << "\t/* Transition " << edgeLabel << " from S" << i 
                  << " not initialized yet */" << std::endl;
                break;
            default:
                s << "\tS" << i << " -> " << "S" << vec.at(t) << "[label=\""
                  << edgeLabel << "\"];" << std::endl;
                break;
            }
        }
    }
    s << "}" << std::endl;   
    return s.str();    
}

/**
 * Returns the starting state of the automaton.
 *
 * By default the starting state is assumed to be the state 0.
 *
 * @return The starting state.
 */
FiniteStateAutomaton::FSAStateIndex
FiniteStateAutomaton::startState() const {
    return 0;
}

