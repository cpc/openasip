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

GraphEdge::GraphEdge() : edgeID_(edgeCounter_++), weight_(-1) {}

GraphEdge::GraphEdge(const GraphEdge& e) : edgeID_(edgeCounter_++), weight_(e.weight()) {}

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
TCEString
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
TCEString
GraphEdge::dotString() const {
    return TCEString("label=\"") + toString() + "\"";
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


int GraphEdge::edgeCounter_ = 0;
