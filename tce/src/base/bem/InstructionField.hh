/**
 * @file InstructionField.hh
 *
 * Declaration of InstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_FIELD_HH
#define TTA_INSTRUCTION_FIELD_HH

#include "Exception.hh"
#include "Serializable.hh"

/**
 * InstructionField is an abstract base class that represents the properties
 * common to all types of bit fields of the TTA instruction word.
 */
class InstructionField : public Serializable {
public:
    virtual ~InstructionField();

    InstructionField* parent() const;

    /**
     * Returns the number of (immediate) child fields within the instruction
     * field.
     */
    virtual int childFieldCount() const = 0;

    virtual InstructionField& childField(int position) const
	throw (OutOfRange);

    /**
     * Returns the bit width of the field.
     */
    virtual int width() const = 0;

    int bitPosition() const;
    int relativePosition() const;
    virtual void setRelativePosition(int position)
	throw (OutOfRange);
    void setExtraBits(int bits)
	throw (OutOfRange);
    int extraBits() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for instruction field.
    static const std::string OSNAME_INSTRUCTION_FIELD;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;
    /// ObjectState attribute key for the relative position of the field.
    static const std::string OSKEY_POSITION;

protected:
    InstructionField(InstructionField* parent);
    InstructionField(const ObjectState* state, InstructionField* parent)
	throw (ObjectStateLoadingException);

    void setParent(InstructionField* parent);
    static void reorderSubfields(ObjectState* state);

private:
    /// Indicates the relative position of the field.
    int relativePos_;
    /// The number of extra bits.
    int extraBits_;
    /// The parent instruction field.
    InstructionField* parent_;
};

#endif
