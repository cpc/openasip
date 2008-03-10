/**
 * @file MoveNodeSet.cc
 *
 * Implementation of MoveNodeSet class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "MoveNodeSet.hh"
#include "NullMove.hh"

/**
 * Fairly empty constructor.
 */
 MoveNodeSet::MoveNodeSet() {
 }

/**
 * Returns number of MoveNodes in this set
 * @return Number of nodes in this set
 */
int
MoveNodeSet::count() const {
    return moveNodes_.size();
}

/**
 * Returns a copy of a MoveNode identified by index
 *
 * @param index Index of MoveNode to return
 * @return MoveNode identified by index
 */
MoveNode&
MoveNodeSet::at(const int index) throw(OutOfRange) {
    if (index >= count()) {
        std::string msg = "MoveNodeSet at() index too high.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return *moveNodes_[index];
}

/**
 * Returns the disassembly of the moves in the set in a single line.
 *
 * @return the disassembly.
 */
std::string
MoveNodeSet::toString() const {
    std::string disasm = "";
    for (int i = 0; i < count(); ++i) {
        disasm += moveNodes_.at(i)->toString() + " ; ";
    }
    return disasm;
}

/**
 * Add MoveNode to the MoveNodeSet
 *
 * @param newMove New MoveNode to add to set
 */
void
MoveNodeSet::addMoveNode(MoveNode& newMove){
    moveNodes_.push_back(&newMove);
}

void MoveNodeSet::removeMoveNode(MoveNode& node) 
    throw (IllegalRegistration) {
    for (std::vector<MoveNode*>::iterator i = moveNodes_.begin();
         i != moveNodes_.end(); i++) {
        if (*i == &node) {
            moveNodes_.erase(i);
            return;
        }
    }
    throw IllegalRegistration(
        __FILE__,__LINE__,__func__,"MoveNode not in MoveNodeSet");
}
