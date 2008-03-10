/**
 * @file NullGPRGuardEncoding.hh
 *
 * Declaration of NullGPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_GPR_GUARD_ENCODING_HH
#define TTA_NULL_GPR_GUARD_ENCODING_HH

#include "GPRGuardEncoding.hh"

class MoveSlot;
class BinaryEncoding;

/**
 * A null version of GPRGuardEncoding class.
 */
class NullGPRGuardEncoding : public GPRGuardEncoding {
public:
    static NullGPRGuardEncoding& instance();

private:
    NullGPRGuardEncoding();
    virtual ~NullGPRGuardEncoding();

    /// The only instance of this class.
    static NullGPRGuardEncoding instance_;
    /// The parent guard field.
    static GuardField guardField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
