/**
 * @file ImmediateControlField.hh
 *
 * Declaration of ImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_CONTROL_FIELD_HH
#define TTA_IMMEDIATE_CONTROL_FIELD_HH

#include <string>
#include <map>

#include "InstructionField.hh"
#include "Exception.hh"
#include "Serializable.hh"

class BinaryEncoding;
class MoveSlot;

/**
 * ImmediateControlField is a specialised InstructionField and represents the
 * field of the TTA instruction that contains the long immediate control tag.
 *
 * This tag defines the template of the instruction. A TTA instruction
 * template specifies which move slots are used to program data
 * transports and which are used to encode immediate bits. An
 * ImmediateControlField object can never exist alone. It is always
 * registered to a BinaryEncoding object.
 */
class ImmediateControlField : public InstructionField {
public:
    ImmediateControlField(BinaryEncoding& parent)
	throw (ObjectAlreadyExists);
    ImmediateControlField(const ObjectState* state, BinaryEncoding& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~ImmediateControlField();

    BinaryEncoding* parent() const;

    int templateCount() const;
    std::string instructionTemplate(int index) const
	throw (OutOfRange);

    bool hasTemplateEncoding(const std::string& name) const;
    unsigned int templateEncoding(const std::string& name) const
	throw (InstanceNotFound);

    void addTemplateEncoding(const std::string& name, unsigned int encoding)
	throw (ObjectAlreadyExists);
    void removeTemplateEncoding(const std::string& name);

    // virtual methods derived from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int) const
	throw (OutOfRange);

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for immediate control field.
    static const std::string OSNAME_IMM_CONTROL_FIELD;
    /// ObjectState name for a template mapping.
    static const std::string OSNAME_TEMPLATE_MAP;
    /// ObjectState attribute key for the name of the instruction template.
    static const std::string OSKEY_TEMPLATE_NAME;
    /// ObjectState attribute key for the encoding of the instruction
    /// template.
    static const std::string OSKEY_ENCODING;

private:
    /// A map type for binary encodings.
    typedef std::map<std::string, unsigned int> EncodingMap;

    void clearTemplateEncodings();

    /// Binary encodings for instruction templates.
    EncodingMap templates_;
};

#endif
