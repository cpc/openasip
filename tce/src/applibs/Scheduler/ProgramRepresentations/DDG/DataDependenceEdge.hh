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
 * @file DataDependenceEdge.hh
 *
 * Declaration of data dependence edge class
 *
 * @author Heikki Kultala 2006-2008 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEPENDENCE_EDGE_HH
#define TTA_DATA_DEPENDENCE_EDGE_HH

#include "TCEString.hh"
#include "GraphEdge.hh"

class MoveNode;
class ObjectState;
class DataDependenceGraph;

class DataDependenceEdge : public GraphEdge {
public:
    enum DependenceType {
        DEP_UNKNOWN = 0,
        DEP_RAW     = 1,
        DEP_WAR     = 2,
        DEP_WAW     = 3,
        DEP_TRIGGER = 4};

    enum EdgeReason {
        EDGE_REGISTER,
        EDGE_MEMORY,
        EDGE_FUSTATE,
        EDGE_OPERATION,
        EDGE_RA};

    DataDependenceEdge(
        EdgeReason edgereason, DependenceType deptype,
        const TCEString& data,
        bool guard = false, bool certainAlias = false, 
        bool tailPseudo = false, bool headPseudo = false,
        int loop = 0);

    DataDependenceEdge(
        EdgeReason edgereason, DependenceType deptype,
        bool guard = false, bool certainAlias = false, 
        bool tailPseudo = false, bool headPseudo = false,
        int loop = 0);

    DataDependenceEdge(const DataDependenceEdge& other);
    DataDependenceEdge(const DataDependenceEdge& other, bool invertLoop);

    virtual ~DataDependenceEdge() {
        delete[] data_;
        data_ = NULL;
    }

    TCEString toString() const;
    TCEString toString(MoveNode& tail) const;

    // For xml dumping
    ObjectState* saveState(
        const MoveNode& tail,
        const MoveNode& head);

    DependenceType dependenceType() const {
        return static_cast<DependenceType>(dependenceType_);
    }
    EdgeReason edgeReason() const {
        return static_cast<EdgeReason>(edgeReason_);
    }

    bool isFalseDep() const {
        return edgeReason_ != EDGE_OPERATION && 
            (dependenceType_ == DEP_WAR || dependenceType_ == DEP_WAW);
    }

    bool guardUse() const {
        return edgeProperties_ & EPF_GUARD;
    }
    bool certainAlias() const {
        return edgeProperties_ & EPF_CERTAIN_ALIAS;
    }
    /** returns whether the tail of the dependence does not directly 
        write/read the data but instead is a control flow move which may 
        cause it to be read/written */
    bool tailPseudo() const {
        return edgeProperties_ & EPF_TAIL_PSEUDO;
    }
    /** returns whether the head of the dependence does not directly write/read
       the data but instead is a control flow move which may cause it to be
       read/written */
    bool headPseudo() const {
        return edgeProperties_ & EPF_HEAD_PSEUDO;
    }
    bool isBackEdge() const {
        return loopEdge_ != 0;
    }
    int loopDepth() const {
        return loopEdge_;
    }

    bool isRegisterOrRA() const {
        return edgeReason() == EDGE_REGISTER ||
            edgeReason() == EDGE_RA;
    }

    bool isWAW() const {
        return dependenceType_ == DEP_WAW;
    }

    bool isRAW() const {
        return dependenceType_ == DEP_RAW;
    }

    const TCEString data() const { 
        if (data_ == NULL) {
            return TCEString("");
        } else {
            return TCEString(data_);
        }
    }
    bool operator ==(const DataDependenceEdge& other) const;

    static void printStats(std::ostream& out);

    // statistic counters for different types of edges created
    static int regAntidepCount_;

    void setData(const TCEString& newData);

private:
    TCEString depTypeSt() const;
    TCEString edgeReasonSt() const;
    TCEString guardSt() const;
    TCEString pseudoSt() const;
    TCEString latencySt(MoveNode& node) const;

    // note: the size of this class must be highly optimized as there
    // will be *lots* of edges in big basic blocks

    // the flags stored in edgeProperties_
    enum EdgePropertyFlags {
        EPF_GUARD = 1 << 0,
        EPF_CERTAIN_ALIAS = 1 << 1,
        EPF_TAIL_PSEUDO = 1 << 2,
        EPF_HEAD_PSEUDO = 1 << 3
    };

    unsigned char dependenceType_; // DependenceType
    unsigned char edgeReason_; // EdgeReason
    unsigned char edgeProperties_;
    unsigned char loopEdge_;
    // register or mem alias info.
    char* data_;
};

#endif
