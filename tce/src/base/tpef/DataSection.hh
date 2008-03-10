/**
 * @file DataSection.hh
 *
 * Declaration of DataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DATA_SECTION_HH
#define TTA_DATA_SECTION_HH

#include <vector>

#include "UDataSection.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Data section for initialized data.
 *
 * Derives from uninitialized data section so, this is extension
 * of UDataSection. 
 */
class DataSection : public UDataSection {
public:
    virtual ~DataSection();

    virtual SectionType type() const;

    virtual Byte byte(const Chunk* chunk) const;
    virtual Byte byte(Word offset) const;
    virtual void addByte(Byte aByte);

    virtual MinimumAddressableUnit MAU(Word index) const;
    virtual void addMAU(MinimumAddressableUnit aMAU)
        throw (OutOfRange);

    virtual void writeValue(Word index, Word numOfMAUs, unsigned long value);
    virtual void writeValue(Word index, Word numOfMAUs, signed long value);

    virtual Word length() const;
    virtual void setDataLength(Word length);

protected:
    DataSection(bool init);
    virtual Section* clone() const;

    virtual void setByte(Word offset, Byte aByte);

private:
    /// Prototype instance of section.
    static DataSection proto_;

    /// Contains the data of data section.
    std::vector<Byte> data_;
};
}

#endif
