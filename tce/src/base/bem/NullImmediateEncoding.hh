/**
 * @file NullImmediateEncoding.hh
 *
 * Declaration of NullImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_IMMEDIATE_ENCODING_HH
#define TTA_NULL_IMMEDIATE_ENCODING_HH

#include "ImmediateEncoding.hh"

class MoveSlot;
class BinaryEncoding;

/**
 * A null version of ImmediateEncoding class.
 */
class NullImmediateEncoding : public ImmediateEncoding {
public:
    static NullImmediateEncoding& instance();

private:
    NullImmediateEncoding();
    virtual ~NullImmediateEncoding();

    /// The only instance.
    static NullImmediateEncoding instance_;
    /// The parent source field.
    static SourceField sourceField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
