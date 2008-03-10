/**
 * @file DestinationField.hh
 *
 * Declaration of DestinationField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESTINATION_FIELD_HH
#define TTA_DESTINATION_FIELD_HH

#include "SlotField.hh"

/**
 * The DestinationField class represents the destination field of a move slot.
 *
 * The destination field contains only socket encodings and so it is just a
 * realization of SlotField class. It does not provide any additional methods.
 */
class DestinationField : public SlotField {
public:
    DestinationField(BinaryEncoding::Position socketIDPos, MoveSlot& parent)
	throw (ObjectAlreadyExists, IllegalParameters);
    DestinationField(const ObjectState* state, MoveSlot& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~DestinationField();

    // methods inherited from Serializable interface
    virtual ObjectState* saveState() const;

    /// ObjectState name for destination field.
    static const std::string OSNAME_DESTINATION_FIELD;
};

#endif
