/**
 * @file ProgramDependenceEdge.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence edge.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ProgramDependenceEdge.hh"

/**
 * Constructor, makes new Program Dependence Edge with reference to
 * original Control Dependence Edge in CDG.
 * @param cEdge Control Dependence Edge
 * @param edgeID unique ID of and edge
 */
ProgramDependenceEdge::ProgramDependenceEdge(
    ControlDependenceEdge& cEdge)
    : cEdge_(&cEdge), dEdge_(NULL) {
}

/**
 * Constructor, makes new Program Dependence Edge with reference to
 * original Data Dependence Edge in DDG.
 * @param dEdge Data Dependence Edge
 * @param edgeID unique ID of and edge
 */
ProgramDependenceEdge::ProgramDependenceEdge(
    DataDependenceEdge& dEdge)
    : cEdge_(NULL), dEdge_(&dEdge) {
}

/**
 * Empty destructor.
 */
ProgramDependenceEdge::~ProgramDependenceEdge() {
}

/**
 * Returns the edge description as a string.
 * @return string describing the edge payload
 */
std::string
ProgramDependenceEdge::toString() const {
    if (isDataDependence()) {
        return dEdge_->toString();
    }
    if (isControlDependence()) {
        return cEdge_->toString();
    }
    return "Error!";
}
/**
 * Returns the edge description as a string in .dot format.
 * @return string describing the edge payload in .dot format
 */
std::string
ProgramDependenceEdge::dotString() const {
    if (isControlDependence()) {
        if (cEdge_->isTrueEdge()) {
            return std::string("label=\"") + toString() + "\",color=blue";
        }
        if (cEdge_->isFalseEdge()) {
            return std::string("label=\"") + toString() + "\",color=red";
        } 
        return std::string("label=\"") + toString() + "\"";
    }
    return std::string("label=\"") + toString() + "\",style=dashed";    
}

/**
 * Tests if edge is control dependence type.
 * @return true if edge is control dependence
 */
bool 
ProgramDependenceEdge::isControlDependence() const {
    return cEdge_ != NULL;
}
/**
 * Tests if edge is data dependence type.
 * @return true if edge is data dependence
 */
bool 
ProgramDependenceEdge::isDataDependence() const {
    return dEdge_ != NULL;
}

/**
 * Returns reference to control dependence edge of CDG.
 * @return control dependence edge of underlying CDG
 */
ControlDependenceEdge& 
ProgramDependenceEdge::controlDependenceEdge() {
    return *cEdge_;
}
/**
 * Returns reference to data dependence edge of CDG.
 * @return data dependence edge of underlying DDG
 */
DataDependenceEdge& 
ProgramDependenceEdge::dataDependenceEdge() {
    return *dEdge_;
}
