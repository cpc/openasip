/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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

#include <cstring>
#include <climits>

#include "DataDependenceEdge.hh"

#include "TCEString.hh"

#include "Conversion.hh"

#include "MoveNode.hh"
#include "HWOperation.hh"
#include "ObjectState.hh"
#include "TerminalFUPort.hh"
#include "TerminalRegister.hh"

int DataDependenceEdge::regAntidepCount_ = 0;

/**
 * Constructor.
 *
 * @param edgereason which kind of data caused this dependence,
 *        is it register, memory, operation, ra etc.
 * @param deptype whether this is real dependence(raw),
 *        antidependence(war) or output dependence(waw)
 * @param data some data about the dependence, for example reg name.
 * @param guard if this dependence is a guard use.
 * @param certainAlias if this memory dep is a known to always alias.
 * @param tailPS whether the tail of the dep is a pseudo data usage
 * @param headPS whether the head of the dep is a pseudo data usage
 * @param loopEdge loop iteration depth if the dependence goes over a loop
 */
DataDependenceEdge::DataDependenceEdge(
    EdgeReason edgereason,
    DependenceType deptype,
    const TCEString& data, bool guard,
    bool certainAlias, bool tailPs,
    bool headPs, int loopEdge) :
    dependenceType_(deptype), edgeReason_(edgereason),
    loopEdge_(loopEdge) {

    edgeProperties_ = 0;
    if (guard)
        edgeProperties_ |= EPF_GUARD;

    if (certainAlias)
        edgeProperties_ |= EPF_CERTAIN_ALIAS;

    if (tailPs)
        edgeProperties_ |= EPF_TAIL_PSEUDO;

    if (headPs)
        edgeProperties_ |= EPF_HEAD_PSEUDO;

    assert(loopEdge < UCHAR_MAX);
    loopEdge = (unsigned char)loopEdge_;

    data_ = NULL;
    setData(data);
    if (edgeReason_ == EDGE_REGISTER) {
        assert(data_ != NULL && "Register name required for reg deps");
    }
}

/**
 * Constructor.
 *
 * @param edgereason which kind of data caused this dependence,
 *        is it register, memory, operation, ra etc.
 * @param deptype whether this is real dependence(raw),
 *        antidependence(war) or output dependence(waw)
 * @param guard if this dependence is a guard use.
 * @param certainAlias if this memory dep is a known to always alias.
 * @param tailPS whether the tail of the dep is a pseudo data usage
 * @param headPS whether the head of the dep is a pseudo data usage
 * @param loopEdge loop iteration depth if the dependence goes over a loop
 */
DataDependenceEdge::DataDependenceEdge(
    EdgeReason edgereason,
    DependenceType deptype,
    bool guard, bool certainAlias,
    bool tailPs, bool headPs,
    int loopEdge) :
    dependenceType_(deptype), edgeReason_(edgereason),
    loopEdge_(loopEdge) {

    edgeProperties_ = 0;
    if (guard)
        edgeProperties_ |= EPF_GUARD;

    if (certainAlias)
        edgeProperties_ |= EPF_CERTAIN_ALIAS;

    if (tailPs)
        edgeProperties_ |= EPF_TAIL_PSEUDO;

    if (headPs)
        edgeProperties_ |= EPF_HEAD_PSEUDO;

    assert(loopEdge < UCHAR_MAX);
    loopEdge = (unsigned char)loopEdge_;
    data_ = NULL;
    if (edgeReason_ == EDGE_REGISTER) {
        assert(0 && "Register name required for reg deps");
    }

}

/**
 * Copy constructor.
 *
 * Because of data_ that needs to be deep copied.
 */
DataDependenceEdge::DataDependenceEdge(const DataDependenceEdge& other) :
    GraphEdge() {
    this->dependenceType_ = other.dependenceType_;
    this->edgeReason_ = other.edgeReason_;
    this->edgeProperties_ = other.edgeProperties_;
    this->loopEdge_ = other.loopEdge_;
    if (other.data_ != NULL) {
        data_ = new char[strlen(other.data_)+1];
        strcpy(data_, other.data_);
    } else {
        data_ = NULL;
    }
}

/**
 * Copy constructor which may modify the loop property
 *
 * @param other edge to copy
 * @invertLoop whether to invent oop propert 0->1, 1->0
 */
DataDependenceEdge::DataDependenceEdge(
    const DataDependenceEdge& other, bool invertLoop) :
    GraphEdge() {
    this->dependenceType_ = other.dependenceType_;
    this->edgeReason_ = other.edgeReason_;
    this->edgeProperties_ = other.edgeProperties_;
    if (!invertLoop || other.loopEdge_ > 1) {
        this->loopEdge_ = other.loopEdge_;
    } else {
        this->loopEdge_ = !other.loopEdge_;
    }
    if (other.data_ != NULL) {
        data_ = new char[strlen(other.data_)+1];
        strcpy(data_, other.data_);
    } else {
        data_ = NULL;
    }
}


/**
 * Prints statistics about the created edges to the given
 * output stream.
 */
void
DataDependenceEdge::printStats(std::ostream& out) {
    out << "register anti-dependencies: " << regAntidepCount_ << std::endl;
}

/**
 * Returns a textual representation of the edge.
 *
 * This is used in the generated dot files.
 */
TCEString
DataDependenceEdge::toString() const {
    if (loopEdge_ != 0) {
        return TCEString("LOOP:") + Conversion::toString(int(loopEdge_)) + "_" +
            edgeReasonSt() + guardSt() + depTypeSt() + pseudoSt() +
            ':' + data();
    }
    return edgeReasonSt() + guardSt() + depTypeSt() + pseudoSt() + ':' + data();
}

/**
 * Returns a textual representation of the edge.
 *
 * @param tail the source node of the dependence. Needed
 *        for looking up operation latencies in scheduled code.
 *
 * @todo This should be refactored.
 *
 * This is used in the generated dot files.
 */
TCEString
DataDependenceEdge::toString(MoveNode& tail) const {
    if (loopEdge_ !=0) {
        return TCEString("LOOP:") + Conversion::toString(int(loopEdge_)) +
            "_" + edgeReasonSt() + guardSt() + depTypeSt() + pseudoSt() +
            ':' + data() + latencySt(tail);
    }
    return edgeReasonSt() + guardSt() + depTypeSt() + pseudoSt() +
        ':' + data() + latencySt(tail);
}

/**
 * Helper method for creating the toString string.
 */
TCEString
DataDependenceEdge::edgeReasonSt() const {
    switch (edgeReason_) {
        case EDGE_REGISTER:
            return "R";
        case EDGE_MEMORY:
            if (certainAlias())
                return "M_T";
            else
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

/**
 * Helper method for creating the toString string.
 */
TCEString
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

/**
 * Helper method for creating the toString string.
 */
TCEString
DataDependenceEdge::guardSt() const {
    return guardUse() ? "_G" : "";
}

/**
 * Helper method for creating the toString string.
 */
TCEString
DataDependenceEdge::pseudoSt() const {
    return TCEString(tailPseudo() ? "_TP" : "") + (headPseudo() ? "_HP" : "");
}

/**
 * Helper method for creating the toString string.
 * Finds out the latency of operation dependences.
 */

TCEString
DataDependenceEdge::latencySt(MoveNode& node) const {
    if (edgeReason_ == EDGE_OPERATION && node.isAssigned()) {
        if (node.move().isTriggering()) {
            TTAProgram::TerminalFUPort &tfu
                = dynamic_cast<TTAProgram::TerminalFUPort&>
                (node.move().destination());
            std::stringstream ss;
            std::string lat;
            ss << tfu.hwOperation()->latency();
            ss >> lat;
            return ":" + lat;
        } else {
            return ":0";
        }
    } else {
        return "";
    }
}

/**
 * Comparator.
 *
 * Checks whether the two edges are identical.
 */
bool
DataDependenceEdge::operator==(const DataDependenceEdge& other) const {
    return dependenceType_ == other.dependenceType_ &&
        edgeReason_ == other.edgeReason_ &&
        data() == other.data() &&
        guardUse() == other.guardUse() &&
        certainAlias() == other.certainAlias() &&
        tailPseudo() == other.tailPseudo() &&
        headPseudo() == other.headPseudo();
}

void
DataDependenceEdge::setData(const TCEString& newData) {
    delete[] data_;
    data_ = new char[newData.length() + 1];
    strncpy(data_, newData.c_str(), newData.length() + 1);
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

    if (loopEdge_ != 0) {
        ObjectState* loopEdgeOS = new ObjectState("dist", edgeOS);
        loopEdgeOS->setValue(Conversion::toString(static_cast<int>(loopEdge_)));
    }
    
    return edgeOS;
}

