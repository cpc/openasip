/**
 * @file MoveNodeSet.hh
 *
 * Declaration of MoveNodeSet class.
 *
 * @author Heikki Kultala 2006 (hkultala@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TCE_MOVE_NODE_SET_HH
#define TCE_MOVE_NODE_SET_HH

#include "MoveNode.hh"
#include "Exception.hh"
#include "NullMove.hh"
#include <vector>

class MoveNodeSet{
public:

    MoveNodeSet();

    int count() const;
    MoveNode& at(int index) throw(OutOfRange);
    void addMoveNode(MoveNode&);
    void removeMoveNode(MoveNode&) throw (IllegalRegistration);

    std::string toString() const;

private:
    // Vector holding pointers to MoveNodes in a collection
    std::vector<MoveNode*> moveNodes_;
};

#endif
