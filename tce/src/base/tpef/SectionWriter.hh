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
 * @file SectionWriter.hh
 *
 * Declaration of SectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_WRITER_HH
#define TTA_SECTION_WRITER_HH

#include <map>
#include <utility>

#include "BinaryWriter.hh"
#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Abstract base class for actual section writer classes.
 *
 * SectionWriter base class stores instances of all registed section
 * writer classes and serve registeration methods for actual SectionWriter
 * instances. Actual section writer classes needs also static instance()
 * method to work correctly.
 */
class SectionWriter {
public:
    static void writeHeader(
        BinaryStream& stream,
        const Section* sect,
        const BinaryWriter* writer)
        throw (InstanceNotFound);

    static void writeData(
        BinaryStream& stream,
        const Section* sect,
        const BinaryWriter* writer)
        throw (InstanceNotFound);

    static void finalizeBinary(
        BinaryStream& stream,
        const Binary* binaryToFinalize,
        const BinaryWriter* writer);

protected:
    SectionWriter();
    virtual ~SectionWriter();

    /// Gets parent of instance which for SectionWriter is created.
    virtual const BinaryWriter& parent() const = 0;

    /// Returns SectionType that actual reader or writer instance can handle.
    virtual Section::SectionType type() const = 0;

    virtual void finalize(BinaryStream& stream, Section* section) const;

    /// Does actual writing of sections header.
    virtual void actualWriteHeader(
        BinaryStream& stream,
        const Section* sect) const = 0;

    /// Does actual writing of sections data.
    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const = 0;

    static void registerSectionWriter(const SectionWriter* sWriter);

    static SectionId getUniqueSectionId();

private:
    /// Key type for finding values in map of section writers.
    typedef
    std::pair<const Section::SectionType, const BinaryWriter*> MapKey;

    /// Map type that contains instances of registered section writers.
    typedef std::map<MapKey, const SectionWriter*> MapType;

    SectionWriter(const SectionWriter&);

    static const SectionWriter* findSectionWriter(
        const Section::SectionType type,
        const BinaryWriter* bWriter)
        throw (InstanceNotFound);

    /// Contains section writers for all kinds of sections and all kinds of
    /// binrary fromats that are supported.
    static MapType* prototypes_;
};
}

#endif
