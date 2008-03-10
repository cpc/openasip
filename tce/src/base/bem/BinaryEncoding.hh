/**
 * @file BinaryEncoding.hh
 *
 * Declaration of BinaryEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BINARY_ENCODING_HH
#define TTA_BINARY_ENCODING_HH

#include "Serializable.hh"
#include "InstructionField.hh"

class MoveSlot;
class ImmediateSlotField;
class SocketCodeTable;
class ImmediateControlField;
class LImmDstRegisterField;

/**
 * BinaryEncoding is the root class in the object model of the binary
 * encoding map.
 *
 * An instance of this class is sufficient to access all the
 * information pertaining a binary encoding map, so an object can be
 * passed to clients whenever a full description of the encoding map
 * is required. BinaryEncoding is mostly a specialized container of
 * objects of other classes.
 */
class BinaryEncoding : public InstructionField {
public:
    enum Position {
	LEFT,
	RIGHT
    };

    BinaryEncoding();
    BinaryEncoding(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ~BinaryEncoding();

    int moveSlotCount() const;
    MoveSlot& moveSlot(int index) const
	throw (OutOfRange);
    bool hasMoveSlot(const std::string& name) const;
    MoveSlot& moveSlot(const std::string& name) const
	throw (InstanceNotFound);
    void addMoveSlot(MoveSlot& slot)
	throw (ObjectAlreadyExists);
    void removeMoveSlot(MoveSlot& slot);

    int immediateSlotCount() const;
    ImmediateSlotField& immediateSlot(int index) const
        throw (OutOfRange);
    bool hasImmediateSlot(const std::string& name) const;
    ImmediateSlotField& immediateSlot(const std::string& name) const
        throw (InstanceNotFound);
    void addImmediateSlot(ImmediateSlotField& slot)
        throw (ObjectAlreadyExists);
    void removeImmediateSlot(ImmediateSlotField& slot);

    bool hasImmediateControlField() const;
    ImmediateControlField& immediateControlField() const;
    void setImmediateControlField(ImmediateControlField& field)
	throw (ObjectAlreadyExists);
    void unsetImmediateControlField();

    int longImmDstRegisterFieldCount() const;
    LImmDstRegisterField& longImmDstRegisterField(int index) const
        throw (OutOfRange);
    LImmDstRegisterField& longImmDstRegisterField(
        const std::string& iTemp,
        const std::string& dstUnit) const
        throw (InstanceNotFound);
    void addLongImmDstRegisterField(LImmDstRegisterField& field);
    void removeLongImmDstRegisterField(LImmDstRegisterField& field);

    int socketCodeTableCount() const;
    SocketCodeTable& socketCodeTable(int index) const
	throw (OutOfRange);
    SocketCodeTable& socketCodeTable(const std::string& name) const;
    void addSocketCodeTable(SocketCodeTable& table)
	throw (ObjectAlreadyExists);
    void removeSocketCodeTable(SocketCodeTable& table);

    // methods inherited from InstructionField
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const
	throw (OutOfRange);
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for binary encoding.
    static const std::string OSNAME_BEM;

private:
    /// Container type for MoveSlots.
    typedef std::vector<MoveSlot*> MoveSlotContainer;
    /// Container type for ImmediateSlotFields.
    typedef std::vector<ImmediateSlotField*> ImmediateSlotContainer;
    /// Container type for SocketCodeTables.
    typedef std::vector<SocketCodeTable*> SocketCodeTableContainer;
    /// Container type for LImmDstRegisterFields.
    typedef std::vector<LImmDstRegisterField*> LImmDstRegisterFieldContainer;

    bool hasSocketCodeTable(const std::string& name) const;
    void deleteMoveSlots();
    void deleteImmediateSlots();
    void deleteLongImmDstRegisterFields();
    void deleteSocketCodes();

    /// A container for move slots.
    MoveSlotContainer moveSlots_;
    /// A container for immediate slots.
    ImmediateSlotContainer immediateSlots_;
    /// A container for socket code tables.
    SocketCodeTableContainer socketCodes_;
    /// The immediate control field.
    ImmediateControlField* immediateField_;
    /// A container for long immediate register fields.
    LImmDstRegisterFieldContainer longImmDstRegFields_;
};

#endif
