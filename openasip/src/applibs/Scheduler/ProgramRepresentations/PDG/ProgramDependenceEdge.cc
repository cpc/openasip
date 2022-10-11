/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProgramDependenceEdge.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence edge.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "ProgramDependenceEdge.hh"
#include "Exception.hh"

/**
 * Constructor, makes new Program Dependence Edge without reference
 * to original Control Dependence Edge in CDG. It is artificial edge.
 */
ProgramDependenceEdge::ProgramDependenceEdge(EdgeType type)
    : cEdge_(NULL), dEdge_(NULL), type_(type), fixed_(false) {
}

/**
 * Constructor, makes new Program Dependence Edge with reference to
 * original Control Dependence Edge in CDG.
 * @param cEdge Control Dependence Edge
 */
ProgramDependenceEdge::ProgramDependenceEdge(
    ControlDependenceEdge& cEdge)
    : cEdge_(&cEdge), dEdge_(NULL), type_(PDG_EDGE_CONTROL), fixed_(false) {
    if (cEdge.isLoopCloseEdge()) {
        type_ = PDG_EDGE_LOOP_CLOSE;
    } else {
        type_ = PDG_EDGE_CONTROL;
    }
}

/**
 * Constructor, makes new Program Dependence Edge with reference to
 * original Data Dependence Edge in DDG.
 * @param dEdge Data Dependence Edge
 */
ProgramDependenceEdge::ProgramDependenceEdge(
    DataDependenceEdge& dEdge)
    : cEdge_(NULL), dEdge_(&dEdge), type_(PDG_EDGE_DATA), fixed_(false) {
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
TCEString
ProgramDependenceEdge::toString() const {
    if (isArtificialControlDependence()) {
        return "PDG_ARTIFICIAL";
    }
    if (isLoopCloseEdge()) {
        return "LoopClose";
    }
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
TCEString
ProgramDependenceEdge::dotString() const {
    if (isArtificialControlDependence()) {
       return TCEString("label=\"") + toString() + "\",color=green";
    }
    if (isLoopCloseEdge()) {
       return TCEString("label=\"") + toString() + "\",color=cyan";
    }

    if (isControlDependence()) {
        if (cEdge_->isTrueEdge()) {
            return TCEString("label=\"") + toString() + "\",color=blue";
        }
        if (cEdge_->isFalseEdge()) {
            return TCEString("label=\"") + toString() + "\",color=red";
        }
        return TCEString("label=\"") + toString() + "\"";
    }
    return TCEString("label=\"") + toString() + "\",style=dashed";
}

/**
 * Tests if edge is control dependence type.
 * @return true if edge is control dependence
 */
bool
ProgramDependenceEdge::isControlDependence() const {
    if (isArtificialControlDependence()) {
        return true;
    }
    return type_ == PDG_EDGE_CONTROL && cEdge_ != NULL ;
}
/**
 * Tests if edge is data dependence type.
 * @return true if edge is data dependence
 */
bool
ProgramDependenceEdge::isDataDependence() const {
    return type_ == PDG_EDGE_DATA && dEdge_ != NULL;
}

/**
 * Tests if edge is artificial control dependence type.
 * Artificial edges are added during PDG synchronization.
 * @return true if edge is artificial control dependence
 */
bool
ProgramDependenceEdge::isArtificialControlDependence() const {
    return type_ == PDG_EDGE_CONTROL_ARTIFICIAL;
}

/**
 * Tests if edge is loop close edge from artificial close node to
 * loop entry node
 * @return true if edge is loop close edge
 */
bool
ProgramDependenceEdge::isLoopCloseEdge() const {
    return type_ == PDG_EDGE_LOOP_CLOSE;
}

/**
 * Returns reference to control dependence edge of CDG.
 * @return control dependence edge of underlying CDG
 */
ControlDependenceEdge&
ProgramDependenceEdge::controlDependenceEdge() {
    if (cEdge_ != NULL) {
        return *cEdge_;
    } else {
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Not a control dependence edge!");
    }
}
/**
 * Returns reference to data dependence edge of CDG.
 * @return data dependence edge of underlying DDG
 */
DataDependenceEdge&
ProgramDependenceEdge::dataDependenceEdge() {
    return *dEdge_;
}
