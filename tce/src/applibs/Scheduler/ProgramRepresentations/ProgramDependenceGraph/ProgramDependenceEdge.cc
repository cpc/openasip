/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
