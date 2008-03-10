/**
 * @file NullMoveGuard.hh
 *
 * Declaration of NullMoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_MOVEGUARD_HH
#define TTA_NULL_MOVEGUARD_HH

#include "MoveGuard.hh"

namespace TTAMachine {
    class Guard;
}

namespace TTAProgram {

/**
 * A singleton class that represents a null guard.
 *
 * Calling any method causes the program to abort.
 */
class NullMoveGuard : public MoveGuard {
public:
    virtual ~NullMoveGuard();
    static NullMoveGuard& instance();

    bool isUnconditional() const;
    bool isInverted() const;
    TTAMachine::Guard& guard() const;

protected:
    NullMoveGuard();

private:
    /// Copying not allowed.
    NullMoveGuard(const NullMoveGuard&);
    /// Assignment not allowed.
    NullMoveGuard& operator=(const NullMoveGuard&);

    /// Unique instance of NullMoveGuard.
    static NullMoveGuard instance_;
};

}

#endif
