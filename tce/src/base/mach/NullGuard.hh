/**
 * @file NullGuard.hh
 *
 * Declaration of NullGuard class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_GUARD_HH
#define TTA_NULL_GUARD_HH

#include "Guard.hh"

namespace TTAMachine {

/**
 * A null version of Guard class.
 */
class NullGuard : public Guard {
public:
    static NullGuard& instance();

    virtual Bus* parentBus() const;
    virtual bool isEqual(const Guard&) const;
    virtual bool isInverted() const;
    virtual bool isMoreRestrictive(const Guard&) const;
    virtual bool isLessRestrictive(const Guard&) const;
    virtual bool isDisjoint(const Guard&) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState*)
        throw (ObjectStateLoadingException);

private:
    NullGuard();
    virtual ~NullGuard();

    /// The only instance of NullGuard.
    static NullGuard instance_;
    /// The parent bus.
    static Bus bus_;
};
}

#endif
