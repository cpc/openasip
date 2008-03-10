/**
 * @file SlotField.hh
 *
 * Declaration of SlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SLOT_FIELD_HH
#define TTA_SLOT_FIELD_HH

#include <vector>
#include "InstructionField.hh"
#include "BinaryEncoding.hh"

class SocketEncoding;
class MoveSlot;
class NOPEncoding;

/**
 * SlotField is an abstract base class that models the properties common to
 * SourceField and DestinationField classes.
 *
 * SlotField contains and manages the encodings for sockets. Encodings
 * for different sources and destinations consists of two parts:
 * socket ID and optional socket code. Socket ID determines the
 * socket. Socket code determines the port (and opcode) the socket is
 * connected to. Socket ID may be on either left or right side of the
 * socket code in the source or destination field. If socket ID is on
 * the left side of socket code, socket ID is aligned to the left end
 * of the whole source/destination field. In the other case, socket ID
 * is aligned to the right end of the source/destination field.
 */
class SlotField : public InstructionField {
public:
    virtual ~SlotField();

    MoveSlot* parent() const;

    void addSocketEncoding(SocketEncoding& encoding)
	throw (ObjectAlreadyExists);
    void removeSocketEncoding(SocketEncoding& encoding);

    int socketEncodingCount() const;
    SocketEncoding& socketEncoding(int index) const
	throw (OutOfRange);
    bool hasSocketEncoding(const std::string& socket) const;
    SocketEncoding& socketEncoding(const std::string& socket) const;

    void setNoOperationEncoding(NOPEncoding& encoding)
        throw (ObjectAlreadyExists);
    void unsetNoOperationEncoding();
    bool hasNoOperationEncoding() const;
    NOPEncoding& noOperationEncoding() const;

    BinaryEncoding::Position componentIDPosition() const;

    // methods inherited from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const
	throw (OutOfRange);

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for slot field.
    static const std::string OSNAME_SLOT_FIELD;
    /// ObjectState attribute key for component ID position.
    static const std::string OSKEY_COMPONENT_ID_POSITION;

protected:
    SlotField(BinaryEncoding::Position componentIDPos, MoveSlot& parent);
    SlotField(const ObjectState* state, MoveSlot& parent)
	throw (ObjectStateLoadingException);

private:
    /// A container type for socket encodings.
    typedef std::vector<SocketEncoding*> SocketEncodingTable;

    void clearSocketEncodings();
    void clearNoOperationEncoding();

    /// The NOP encoding.
    NOPEncoding* nopEncoding_;
    /// The container for socket encodings.
    SocketEncodingTable encodings_;
    /// Position of the socket and bridge IDs within the field.
    BinaryEncoding::Position componentIDPos_;
};

#endif


