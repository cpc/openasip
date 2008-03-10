/**
 * @file ImmediateSlotField.hh
 *
 * Declaration of ImmediateSlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_FIELD_HH
#define TTA_IMMEDIATE_SLOT_FIELD_HH

#include "InstructionField.hh"

class BinaryEncoding;

/**
 * ImmediateSlotField represents a dedicated immediate slot in TTA 
 * instruction.
 */
class ImmediateSlotField : public InstructionField {
public:
    ImmediateSlotField(
        const std::string& name,
        int width,
        BinaryEncoding& parent)
        throw (OutOfRange, ObjectAlreadyExists);
    ImmediateSlotField(const ObjectState* state, BinaryEncoding& parent)
        throw (ObjectStateLoadingException);
    virtual ~ImmediateSlotField();

    BinaryEncoding* parent() const;
    std::string name() const;
    void setName(const std::string& name)
        throw (ObjectAlreadyExists);

    virtual int childFieldCount() const;
    virtual int width() const;
    void setWidth(int width)
        throw (OutOfRange);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for immediate slot field.
    static const std::string OSNAME_IMMEDIATE_SLOT_FIELD;
    /// ObjectState attribute key for the name of the immediate slot.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for the width of the field.
    static const std::string OSKEY_WIDTH;

private:
    /// Name of the immediate slot.
    std::string name_;
    /// The bit width of the field.
    int width_;
};

#endif
