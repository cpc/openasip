/** 
 * @file MoveNodeUse.hh
 *
 * Declaration of MoveNodeUse class.
 *
 * Wrapper class that contains data dependence information related to one
 * data dependence induced by a MoveNode. 
 *
 * Multiple MoveNodeUse objects can point to a single movenode.
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef MOVENODEUSE_HH
#define MOVENODEUSE_HH

#include "MoveNode.hh"


class MoveNodeUse {
public:
    
    enum BBRelation {
        INTRA_BB = 0,
        INTER_BB = 1,
        LOOP = 3
    };

    MoveNodeUse() : mn_(NULL) {} //because STL sucks. always = after this.
    MoveNodeUse(
        const MoveNode& mn, bool guard = false, bool ra = false, 
        bool pseudo = false, BBRelation bbRelation = INTRA_BB) :
        mn_(&mn), guard_(guard), ra_(ra), pseudo_(pseudo), bbRelation_(bbRelation) {}

    inline MoveNodeUse(const MoveNodeUse& mnu, BBRelation newBBRelation);

    // TODO: should be deterministic - this is not?
    inline bool operator< (const MoveNodeUse& other) const;
    const MoveNode* mn() const { return mn_; } 
    bool guard() const { return guard_; } 
    bool ra() const { return ra_; } 
    bool pseudo() const { return pseudo_; }
    bool loop() const { return bbRelation_ == LOOP; }
    bool interBB() const { return bbRelation_ > INTRA_BB; }
    BBRelation bbRelation() const { return bbRelation_; }
private:
    const MoveNode* mn_;
    bool guard_;
    bool ra_;
    bool pseudo_;
    BBRelation bbRelation_;
};

#include "MoveNodeUse.icc"

#endif
