/**
 * @file ImmediateElement.hh
 *
 * Declaration of ImmediateElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rl, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_IMMEDIATE_ELEMENT_HH
#define TTA_IMMEDIATE_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Immediate data element of CodeSection.
 */
class ImmediateElement : public InstructionElement {
public:
    ImmediateElement();
    virtual ~ImmediateElement();

    bool isInline() const;

    void addByte(Byte aByte);

    Byte byte(unsigned int index) const
        throw (OutOfRange);

    void setByte(unsigned int index, Byte aValue)
        throw (OutOfRange);

    void setWord(Word aValue);

    Word word() const
        throw (OutOfRange);

    unsigned int length() const;
    
    Byte destinationUnit() const;
    void setDestinationUnit(Byte aDestinationUnit);

    Byte destinationIndex() const;
    void setDestinationIndex(Byte aDestinationIndex);

private:
    /// Value of immediate.
    std::vector<Byte> value_;
    /// Destination unit of immediate.
    Byte destinationUnit_;
    /// Destination index of immediate.
    Byte destinationIndex_;
};
}

#include "ImmediateElement.icc"

#endif
