/**
 * @file DataDependenceEdge.cc
 *
 * Implementation of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
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
