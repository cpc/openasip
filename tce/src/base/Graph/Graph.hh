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
 * @file Graph.hh
 *
 * Declaration of prototype of base graph interface.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */


#ifndef TTA_GRAPH_HH
#define TTA_GRAPH_HH

#include "Exception.hh"

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

    virtual Node& node(const int index) const 
        throw (OutOfRange) = 0;
    virtual Edge& edge(const int index) const 
        throw (OutOfRange) = 0;

    virtual int outDegree(const Node& node) const
        throw (InstanceNotFound) = 0;
    virtual int inDegree(const Node& node) const
        throw (InstanceNotFound) = 0;

    virtual Edge& outEdge(const Node& node, const int index) const 
        throw (OutOfRange, InstanceNotFound) = 0;

    virtual Edge& inEdge(const Node& node, const int index) const 
        throw (OutOfRange, InstanceNotFound) = 0;

    virtual EdgeSet outEdges(const Node& node) const
        throw (InstanceNotFound) = 0;
    virtual EdgeSet inEdges(const Node& node) const
        throw (InstanceNotFound) = 0;

    virtual Node& tailNode(const Edge& edge) const 
        throw (InstanceNotFound) = 0;
    virtual Node& headNode(const Edge& edge) const 
        throw (InstanceNotFound) = 0;

    virtual bool hasEdge(
        const Node& nTail,
        const Node& nHead) const = 0;

    virtual EdgeSet connectingEdges(
        const Node& nTail, const Node& nHead) const = 0;

    virtual std::string dotString() const;
    virtual void writeToDotFile(const std::string& fileName) const;

    virtual void addNode(Node& node)
        throw (ObjectAlreadyExists) = 0;

    virtual void removeNode(Node& node)
        throw (InstanceNotFound) = 0;

    virtual void removeEdge(Edge& e)
        throw (InstanceNotFound) = 0;

    virtual void connectNodes(const Node& nTail, const Node& nHead, Edge& e)
        throw (ObjectAlreadyExists) = 0;
    virtual void disconnectNodes(const Node& nTail, const Node& nHead) = 0;
protected:
    virtual bool hasNode(const Node&) const = 0;
};

#include "Graph.icc"

#endif
