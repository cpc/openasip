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
 * @file ControlFlowEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_FLOW_EDGE_HH
#define TTA_CONTROL_FLOW_EDGE_HH

#include <string>

#include "GraphEdge.hh"

/**
 * Edge of the control flow graph (control flow edge). Each edge implies a
 * possible change of execution flow from the source basic block to the
 * target basic block.
 *
 */
class ControlFlowEdge : public GraphEdge {
public:
    enum CFGEdgePredicate {
        CFLOW_EDGE_NORMAL,
        CFLOW_EDGE_TRUE,
        CFLOW_EDGE_FALSE,
	CFLOW_EDGE_LOOP_BREAK
    };

    ControlFlowEdge(
        CFGEdgePredicate edgePredicate = CFLOW_EDGE_NORMAL,
        bool isJumpEdge = true);

    virtual ~ControlFlowEdge();

    bool isControlFlowEdge() const;
    bool isNormalEdge() const;
    bool isTrueEdge() const;
    bool isFalseEdge() const;
    bool isCallPassEdge() const;
    bool isJumpEdge() const;
    bool isLoopBreakEdge() const;
    std::string toString() const;

private:
    CFGEdgePredicate edgePredicate_;
    bool isJumpEdge_;
};

#endif
