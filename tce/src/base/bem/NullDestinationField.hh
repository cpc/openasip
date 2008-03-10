/**
 * @file NullDestinationField.hh
 *
 * Declaration of NullDestinationField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_DESTINATION_FIELD_HH
#define TTA_NULL_DESTINATION_FIELD_HH

#include "DestinationField.hh"

class BinaryEncoding;
class MoveSlot;

/**
 * A null version of DestinationField class.
 */
class NullDestinationField : public DestinationField {
public:
    static NullDestinationField& instance();

private:
    NullDestinationField();
    virtual ~NullDestinationField();

    /// The instance.
    static NullDestinationField instance_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
