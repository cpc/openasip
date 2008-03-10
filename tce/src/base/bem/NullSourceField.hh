/**
 * @file NullSourceField.hh
 *
 * Declaration of NullSourceField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_SOURCE_FIELD_HH
#define TTA_NULL_SOURCE_FIELD_HH

#include "SourceField.hh"

class MoveSlot;
class BinaryEncoding;

/**
 * A null version of SourceField class.
 */
class NullSourceField : public SourceField {
public:
    static NullSourceField& instance();

private:
    NullSourceField();
    virtual ~NullSourceField();

    /// The only instance.
    static NullSourceField instance_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
