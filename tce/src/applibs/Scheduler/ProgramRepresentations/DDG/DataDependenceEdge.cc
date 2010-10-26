/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file DataDependenceEdge.cc
 *
 * Implementation of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "DataDependenceEdge.hh"
#include "ObjectState.hh"
#include "MoveNode.hh"
#include "HWOperation.hh"

using std::string;

DataDependenceEdge::DataDependenceEdge(EdgeReason edgereason,
                                       DependenceType deptype,
                                       bool guard,
                                       bool certainAlias,
                                       bool tailPs,
                                       bool headPs) :
        dependenceType_(deptype), edgeReason_(edgereason),
        guard_(guard), certainAlias_(certainAlias),
        tailPseudo_(tailPs), headPseudo_(headPs) {}

std::string
DataDependenceEdge::toString() const {
    return edgeReasonSt() + guardSt() + depTypeSt() + pseudoSt();
}

std::string
DataDependenceEdge::edgeReasonSt() const {
    switch (edgeReason_) {
        case EDGE_REGISTER:
            return "R";
        case EDGE_MEMORY:
            return "M";
        case EDGE_FUSTATE:
            return "F";
        case EDGE_OPERATION:
            return "O";
        case EDGE_RA:
            return "RA";
    }
    return "BUG";
}

std::string
DataDependenceEdge::depTypeSt() const {
    switch (dependenceType_) {
        case DEP_RAW:
            return "_raw";
        case DEP_WAR:
            return "_war";
        case DEP_WAW:
            return "_waw";
        case DEP_UNKNOWN:
        default:
            return "";
    }
}

std::string
DataDependenceEdge::guardSt() const {
    return guard_ ? "_G" : "";
}

std::string
DataDependenceEdge::pseudoSt() const {
    return std::string(tailPseudo_ ? "_TP" : "") + (headPseudo_ ? "_HP" : "");
}

bool 
DataDependenceEdge::operator ==(const DataDependenceEdge& other) const {
    return dependenceType_ == other.dependenceType_ &&
        edgeReason_ == other.edgeReason_ &&
        guard_ == other.guard_ &&
        certainAlias_ == other.certainAlias_ &&
        tailPseudo_ == other.tailPseudo_ &&
        headPseudo_ == other.headPseudo_;
}

/**
 * State dumping for XML generation.
 *
 * @param tail source node of the edge
 * @param head sink node of the edge
 */

ObjectState*
DataDependenceEdge::saveState(
    const MoveNode& tail,
    const MoveNode& head) {

    ObjectState* edgeOS = new ObjectState("edge");
    ObjectState* tailOS = new ObjectState("nref", edgeOS);
    ObjectState* headOS = new ObjectState("nref", edgeOS);
    ObjectState* typeOS = new ObjectState("type", edgeOS);
    ObjectState* reasonOS = new ObjectState("reason", edgeOS);

    tailOS->setValue(tail.nodeID());
    headOS->setValue(head.nodeID());

    switch (dependenceType_) {
    case DEP_UNKNOWN:
	typeOS->setName("unknown");
	break;
    case DEP_RAW:
	typeOS->setName("raw");
	break;
    case DEP_WAR:
	typeOS->setName("war");
	break;
    case DEP_WAW:
	typeOS->setName("waw");
	break;
    case DEP_TRIGGER:
	typeOS->setName("trg");
	break;
    }

    switch (edgeReason_) {
    case EDGE_REGISTER:
	reasonOS->setName("reg");
	break;
    case EDGE_MEMORY:
	reasonOS->setName("mem");
	break;
    case EDGE_FUSTATE:
	reasonOS->setName("fu");
	break;
    case EDGE_OPERATION:
	reasonOS->setName("op");
	break;
    case EDGE_RA:
	reasonOS->setName("ra");
	break;
    }

    // Add operation latency information to trigger moves
    if (tail.isMove() && tail.move().isTriggering()) {
	TTAProgram::TerminalFUPort &tfu
	    = dynamic_cast<TTAProgram::TerminalFUPort&>
	    (tail.move().destination());

	ObjectState* latencyOS = new ObjectState("lat", edgeOS);
	latencyOS->setValue(Conversion::toString(tfu.hwOperation()->latency()));
    }

    return edgeOS;
}

