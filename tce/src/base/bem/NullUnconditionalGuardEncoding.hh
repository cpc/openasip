/**
 * @file NullUnconditionalGuardEncoding.hh
 *
 * Declaration of NullUnconditionalGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_UNCONDITIONAL_GUARD_ENCODING_HH
#define TTA_NULL_UNCONDITIONAL_GUARD_ENCODING_HH

#include "UnconditionalGuardEncoding.hh"

class BinaryEncoding;
class MoveSlot;
class GuardField;

/**
 * A null version of UnconditionalGuardEncoding class.
 */
class NullUnconditionalGuardEncoding : public UnconditionalGuardEncoding {
public:
    static NullUnconditionalGuardEncoding& instance();

private:
    NullUnconditionalGuardEncoding();
    virtual ~NullUnconditionalGuardEncoding();

    /// The parent bem.
    static BinaryEncoding bem_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent guard field.
    static GuardField guardField_;
    /// The instance.
    static NullUnconditionalGuardEncoding instance_;
};

#endif
