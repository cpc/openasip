/**
 * @file NullFUGuardEncoding.hh
 *
 * Declaration of NullFUGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_FU_GUARD_ENCODING_HH
#define TTA_NULL_FU_GUARD_ENCODING_HH

#include "FUGuardEncoding.hh"

class MoveSlot;
class BinaryEncoding;

/**
 * A null version of GPRGuardEncoding class.
 */
class NullFUGuardEncoding : public FUGuardEncoding {
public:
    static NullFUGuardEncoding& instance();

private:
    NullFUGuardEncoding();
    virtual ~NullFUGuardEncoding();

    /// The only instance of this class.
    static NullFUGuardEncoding instance_;
    /// The parent guard field.
    static GuardField guardField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;

};

#endif
