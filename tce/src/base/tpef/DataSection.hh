/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file DataSection.hh
 *
 * Declaration of DataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
