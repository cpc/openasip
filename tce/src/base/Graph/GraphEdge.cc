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
 * @file GraphEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "GraphEdge.hh"
#include "Conversion.hh"

GraphEdge::~GraphEdge() { }

GraphEdge::GraphEdge() : edgeID_(edgeCounter_++) {}

GraphEdge::GraphEdge(const GraphEdge&) : edgeID_(edgeCounter_++) {}

/**
 * Clones edge. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the edge. 
 */
GraphEdge* 
GraphEdge::clone() const {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Cloning must be done in subclass or use copy constructor.");
    return NULL;
}

/**
 * Return the label of the edge as a string. 
 *
 * Used for printing graph in GraphViz .dot file.
 *
 * @return The edge ID as a string
 */
std::string
GraphEdge::toString() const {
    return Conversion::toString(edgeID());
}

/**
 * Returns the string that should be placed in the edge's properties section in
 * the GraphViz Dot string.
 *
 * This can be overridden in the derived class to add different properties (e.g.,
 * colors, etc.) to different type of edges. The default implementation only
 * sets the label property.
 *
 * @return String describing the Dot properties of the edge.
 */
std::string 
GraphEdge::dotString() const {
    return std::string("label=\"") + toString() + "\"";
}


/**
 * Return edge ID as integer.
 *
 * @return The edge ID as integer
 */
int
GraphEdge::edgeID() const {
    return edgeID_;
}

/**
 * Compariron based on edge ID's for maps and sets.
 */
bool 
GraphEdge::Comparator::operator()(GraphEdge* e1, GraphEdge* e2) const {
    return e1->edgeID_ < e2->edgeID_;
}

int GraphEdge::edgeCounter_ = 0;
