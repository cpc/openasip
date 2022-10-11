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
 * @file Graph.hh
 *
 * Declaration of prototype of base graph interface.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */


#ifndef TTA_GRAPH_HH
#define TTA_GRAPH_HH

#include "Exception.hh"
#include "TCEString.hh"

#include <set>

/**
 * Graph base interface shared by all clients within the project.
 *
 * Templated interface with node and edge types as parameters. If an
 * implementation of this interface deals with multiple types of nodes, all
 * types must share a common base type. Ditto for edges.
 */
template<typename GraphNode, typename GraphEdge>
class GraphBase {
public:
    typedef std::set<GraphNode*, typename GraphNode::Comparator > NodeSet;
    typedef std::set<GraphEdge*, typename GraphEdge::Comparator > EdgeSet;

    GraphBase() {}
    virtual ~GraphBase() {}

    /// Node type of this graph (possibly, a base class).
    typedef GraphNode Node;
    /// Edge type of this graph (possibly, a base class).
    typedef GraphEdge Edge;

    virtual int nodeCount() const = 0;
    virtual int edgeCount() const = 0;

    virtual Node& node(const int index) const = 0;
    virtual Edge& edge(const int index) const = 0;

    virtual int outDegree(const Node& node) const = 0;
    virtual int inDegree(const Node& node) const = 0;

    virtual Edge& outEdge(const Node& node, const int index) const = 0;

    virtual Edge& inEdge(const Node& node, const int index) const = 0;

    virtual EdgeSet outEdges(const Node& node) const = 0;
    virtual EdgeSet inEdges(const Node& node) const = 0;

    virtual Node& tailNode(const Edge& edge) const = 0;
    virtual Node& headNode(const Edge& edge) const = 0;

    virtual bool hasEdge(
        const Node& nTail,
        const Node& nHead) const = 0;

    virtual EdgeSet connectingEdges(
        const Node& nTail, const Node& nHead) const = 0;

    virtual TCEString dotString() const;
    virtual void writeToDotFile(const TCEString& fileName) const;

    virtual void addNode(Node& node) = 0;

    virtual void removeNode(Node& node) = 0;

    virtual void removeEdge(Edge& e) = 0;

    virtual void connectNodes(
        const Node& nTail, const Node& nHead, Edge& e) = 0;
    virtual void disconnectNodes(const Node& nTail, const Node& nHead) = 0;
    virtual const TCEString& name() const = 0;
protected:
    virtual bool hasNode(const Node&) const = 0;
};

#include "Graph.icc"

#endif
