/**
 * @file MoveGuard.hh
 *
 * Declaration of MoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_GUARD_HH
#define TTA_MOVE_GUARD_HH

#include "Terminal.hh"

namespace TTAMachine {
    class Guard;
}

namespace TTAProgram {

/**
 * Represents an execution predicate.
 *
 * Wraps in the Guard class of the Machine Object Model.
 */
class MoveGuard {
public:
    MoveGuard(TTAMachine::Guard& guard);
    virtual ~MoveGuard();

    bool isUnconditional() const;
    bool isInverted() const;
    TTAMachine::Guard& guard() const;

    MoveGuard* copy() const;

private:
    /// Copying not allowed.
    MoveGuard(const MoveGuard&);
    /// Assignment not allowed.
    MoveGuard& operator=(const MoveGuard&);

    /// The guard object.
    TTAMachine::Guard* guard_;
};

}

#endif
