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
 * @file GraphUtilities.hh
 *
 * Declaration of miscellaneous generic graph utility functions for the
 * prototype graph class. Probably these functions won't be needed once the
 * boost and TCE framework applib-specific functions are fully utilised.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GRAPH_UTILITIES_HH
#define TTA_GRAPH_UTILITIES_HH


/**
 * Base template class for functors that apply to graph nodes.
 *
 * The node type is an independent template parameter. This enables using a
 * subclass instead of the actual graph node class, or even an unrelated
 * class, as long as it provides an appropriate conversion operator.
 *
 * When the graph contains several specialised types of nodes, the template
 * parameter should be convertible to the base node type.
 */
template<typename Graph, typename GraphNode>
class GraphNodeFunctor {
public:
    /// Constructor. Keep record of the parent graph of the nodes to work on.
    GraphNodeFunctor(const Graph& pGraph) :
        graph_(&pGraph) {}
    /// Destructor. Subclasses should clean up any state created during
    /// operation in this method.
    virtual ~GraphNodeFunctor() { };
    /// Operation to apply to a given node of the graph.
    virtual void operator()(GraphNode&) const = 0;

protected:
    /// Expected parent graph.
    const Graph* graph_;
};


/**
 * Base template class for functors that apply to graph edges.
 *
 * The edge type is an independent template parameter. This enables using a
 * subclass instead of the actual graph edge class, or even an unrelated
 * class, as long as it provides an appropriate conversion operator.
 *
 * When the graph contains several specialised types of edges, the template
 * parameter should be convertible to the base edge type.
 */
template<typename Graph, typename GraphEdge>
class GraphEdgeFunctor {
public:
    /// Constructor. Keep record of the parent graph of the nodes to work on.
    GraphEdgeFunctor(const Graph& pGraph) :
        graph_(&pGraph) {}
    /// Destructor. Subclasses should clean up any state created during
    /// operation in this method.
    virtual ~GraphEdgeFunctor() { };
    /// Operation to apply to a given edge of the graph.
    virtual void operator()(GraphEdge&) const = 0;

protected:
    /// Expected parent graph.
    const Graph* graph_;
};


////////////////////////////////////////////////////////////////////////////
// Algorithms based on edge and node functors
////////////////////////////////////////////////////////////////////////////

/**
 * Utility class. Its instances are function objects that delete the given
 * pointer.
 *
 * Use it inside std::for_each and std::transform to deallocate storage
 * pointer to by elements in a container.
 */
template<class T>
class Destroyer {
public:
    Destroyer() {}
    T* operator()(const T* element) {
        delete element;
        return NULL;
    }
};


/**
 * Algorithm: apply given functor to all nodes of a given graph.
 *
 * @param pGraph The input graph.
 * @param functor The function object to apply to each node.
 */
template<typename Graph, typename GraphNode>
void
doOnAllNodes(
    Graph& pGraph,
    const GraphNodeFunctor<Graph, GraphNode>& functor);


/**
 * Algorithm: apply given functor to all edges of a given graph.
 *
 * @param pGraph The input graph.
 * @param functor The function object to apply to each edge.
 */
template<typename Graph, typename GraphEdge>
void
doOnAllEdges(
    Graph& pGraph,
    const GraphEdgeFunctor<Graph, GraphEdge>& functor);


#include "GraphUtilities.icc"

#endif
