/**
 * @file NullNOPEncoding.hh
 *
 * Declaration of NullNOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_NOP_ENCODING_HH
#define TTA_NULL_NOP_ENCODING_HH

#include "NOPEncoding.hh"

class MoveSlot;
class BinaryEncoding;
class SourceField;

/**
 * A null version of ImmediateEncoding class.
 */
class NullNOPEncoding : public NOPEncoding {
public:
    static NullNOPEncoding& instance();

private:
    NullNOPEncoding();
    virtual ~NullNOPEncoding();

    /// The only instance.
    static NullNOPEncoding instance_;
    /// The parent source field.
    static SourceField sourceField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
