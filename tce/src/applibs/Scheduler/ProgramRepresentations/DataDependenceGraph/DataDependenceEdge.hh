/**
 * @file DataDependenceEdge.hh
 *
 * Declaration of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEPENDENCE_EDGE_HH
#define TTA_DATA_DEPENDENCE_EDGE_HH

#include <string>

#include "GraphEdge.hh"

class DataDependenceEdge : public GraphEdge {
public:
    enum DependenceType {
        DEP_UNKNOWN = 0,
        DEP_RAW     = 1,
        DEP_WAR     = 2,
        DEP_WAW     = 3};

    enum EdgeReason {
        EDGE_REGISTER,
        EDGE_MEMORY,
        EDGE_FUSTATE,
        EDGE_OPERATION,
        EDGE_RA};

    DataDependenceEdge(
        EdgeReason edgereason, DependenceType deptype,
        bool guard = false, bool certainAlias = false, 
        bool tailPseudo = false, bool headPseudo = false );
    virtual ~DataDependenceEdge() {}

    std::string toString() const;
    DependenceType dependenceType() {
        return dependenceType_;
    }
    EdgeReason edgeReason() {
        return edgeReason_;
    };
    bool guardUse() {
        return guard_;
    }
    bool certainAlias() {
        return certainAlias_;
    }
    bool tailPseudo() {
        return tailPseudo_;
    }
    bool headPseudo() {
        return headPseudo_;
    }

    bool operator ==(const DataDependenceEdge& other) const;
private:
    std::string depTypeSt() const;
    std::string edgeReasonSt() const;
    std::string guardSt() const;
    std::string pseudoSt() const;

    DependenceType dependenceType_;
    EdgeReason edgeReason_;
    bool guard_;
    bool certainAlias_;
    bool tailPseudo_;
    bool headPseudo_;
};

#endif
