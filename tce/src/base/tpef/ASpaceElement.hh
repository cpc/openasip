/**
 * @file ASpaceElement.hh
 *
 * Declaration of ASpaceElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_ASPACEELEMENT_HH
#define TTA_ASPACEELEMENT_HH

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Address space element.
 * Holds information of single address space.
 */
class ASpaceElement : public SectionElement {
public:
    ASpaceElement();
    virtual ~ASpaceElement();

    Byte MAU() const;
    void setMAU(Byte aMAU);

    Byte align() const;
    void setAlign(Byte aAlign);

    Byte wordSize() const;
    void setWordSize(Byte aWordSize);

    void setName(const ReferenceManager::SafePointer* aName);
    void setName(Chunk* aName);
    Chunk* name() const;

private:
    /// Minimum addressable unit.
    Byte mau_;
    /// Aligment.
    Byte align_;
    /// Word size.
    Byte wSize_;
    /// Name.
    const ReferenceManager::SafePointer* name_;
};
}

#include "ASpaceElement.icc"

#endif
