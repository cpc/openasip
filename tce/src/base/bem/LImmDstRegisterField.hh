/**
 * @file LImmDstRegisterField.hh
 *
 * Declaration of LImmDstRegisterField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_LIMM_DST_REGISTER_FIELD_HH
#define TTA_LIMM_DST_REGISTER_FIELD_HH

#include <string>
#include <map>

#include "InstructionField.hh"

class BinaryEncoding;

/**
 * Represets the instruction field that contains the register index
 * where a long immediate is going to be written.
 */
class LImmDstRegisterField : public InstructionField {
public:
    LImmDstRegisterField(int width, BinaryEncoding& parent)
        throw (OutOfRange);
    LImmDstRegisterField(const ObjectState* state, BinaryEncoding& parent)
        throw (ObjectStateLoadingException);
    virtual ~LImmDstRegisterField();

    BinaryEncoding* parent() const;

    void addDestination(
        const std::string& instructionTemplate,
        const std::string& immediateUnit)
        throw (NotAvailable);

    int instructionTemplateCount() const;
    std::string instructionTemplate(int index) const
        throw (OutOfRange);
    bool usedByInstructionTemplate(
        const std::string& instructionTemplate) const;
    std::string immediateUnit(const std::string& instructionTemplate) const
        throw (NotAvailable);

    virtual int width() const;
    virtual int childFieldCount() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for long immediate destination register field.
    static const std::string OSNAME_LIMM_DST_REGISTER_FIELD;
    /// ObjectState attribute key for the width of the field.
    static const std::string OSKEY_WIDTH;
    /// ObjectState name for a immediate unit destination.
    static const std::string OSNAME_IU_DESTINATION;
    /// ObjectState attribute key for the name of the instruction template.
    static const std::string OSKEY_ITEMP;
    /// ObjectState attribute key for the name of the immediate unit.
    static const std::string OSKEY_DST_IU;

private:
    /// Map type for strings.
    typedef std::map<std::string, std::string> StringMap;

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Width of the field.
    int width_;
    /// Maps instruction templates to immediate units
    StringMap destinationMap_;
};

#endif
