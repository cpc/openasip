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
 * @file DataDependenceEdge.hh
 *
 * Declaration of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
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
