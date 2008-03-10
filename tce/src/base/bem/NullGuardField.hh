/**
 * @file NullGuardField.hh
 *
 * Declaration of NullGuardField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_GUARD_FIELD_HH
#define TTA_NULL_GUARD_FIELD_HH

#include "GuardField.hh"

class BinaryEncoding;

/**
 * A null version of the GuardField class.
 */
class NullGuardField : public GuardField {
public:
    static NullGuardField& instance();

private:
    NullGuardField();
    virtual ~NullGuardField();

    /// The only instance of NullGuardField.
    static NullGuardField instance_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
