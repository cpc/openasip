/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SectionWriter.hh
 *
 * Declaration of SectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_WRITER_HH
#define TTA_SECTION_WRITER_HH

#include <map>
#include <utility>

#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

    class BinaryWriter;
    class BinaryStream;
    class Binary;

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
        const BinaryWriter* writer);

    static void writeData(
        BinaryStream& stream,
        const Section* sect,
        const BinaryWriter* writer);

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
