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
    MoveNodeUse() : mn_(NULL) {} //because STL sucks. always = after this.
    MoveNodeUse(
        const MoveNode& mn, bool guard = false, bool ra = false, 
        bool pseudo = false, bool loop = false) :
        mn_(&mn), guard_(guard), ra_(ra), pseudo_(pseudo), loop_(loop) {}

    inline MoveNodeUse(const MoveNodeUse& mnu, bool setLoopProperty);

    // TODO: should be deterministic - this is not?
    inline bool operator< (const MoveNodeUse& other) const;
    const MoveNode* mn() const { return mn_; } 
    bool guard() const { return guard_; } 
    bool ra() const { return ra_; } 
    bool pseudo() const { return pseudo_; }
    bool loop() const { return loop_; }
private:
    const MoveNode* mn_;
    bool guard_;
    bool ra_;
    bool pseudo_;
    bool loop_;
};

#include "MoveNodeUse.icc"

#endif
