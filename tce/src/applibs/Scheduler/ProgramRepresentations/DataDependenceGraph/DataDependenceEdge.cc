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
 * @file DataDependenceEdge.cc
 *
 * Implementation of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "DataDependenceEdge.hh"

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
