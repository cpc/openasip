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
