/**
 * @file BasicElement.hh
 *
 * Declaration of BasicElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BASIC_ELEMENT_HH
#define TTA_BASIC_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Basic element type.
 *
 * Allows representing data of any element which doesn't have own class
 * for representing it. Can contain any number of bytes inside.
 */
class BasicElement : public SectionElement {
public:
    BasicElement();
    virtual ~BasicElement();

    void addByte(const Byte& data);

    Byte byte(Word index) const
        throw (OutOfRange);

    Word length() const;

private:
    /// Storage of bytes that element contains.
    std::vector<Byte> bytes_;
};
}

#include "BasicElement.icc"

#endif
