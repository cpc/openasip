/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SectionReader.hh
 *
 * Declaration of SectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTIONREADER_HH
#define TTA_SECTIONREADER_HH

#include <map>
#include <utility>

#include "Section.hh"
#include "BinaryReader.hh"
#include "Exception.hh"

namespace TPEF {

    class BinaryStream;

/**
 * Abstract base class for SectionReaders.
 *
 * Implements user friendly interface for registration and finding
 * concrete SectionReader instances. Defines also interface for reading
 * sections so there is no need to know interface or even type of concrete
 * SectionReaders.
 */
class SectionReader {
public:
    typedef BinaryReader::Length Length;

    virtual ~SectionReader();

    static void readSection(
        BinaryStream& stream, Section* section, BinaryReader* reader);

    static void finalizeBinary(
        Binary *binaryToFinalize,
        BinaryReader* reader);

protected:
    SectionReader();

    /// Does actual reading part for constructing section.
    virtual void readData(BinaryStream& stream, Section* section) const = 0;

    /// Returns binary reader whose sections actual section reader reads.
    virtual BinaryReader* parent() const = 0;

    /// Returns type (TPEF) section which actual section reader reads.
    virtual Section::SectionType type() const = 0;

    virtual void finalize(Section* section) const;

    static const SectionReader* findSectionReader(
        const Section::SectionType type, const BinaryReader* bReader);

    static void registerSectionReader(const SectionReader* sReader);

private:
    /// Copying is not allowed.
    SectionReader(const SectionReader&);

    /// Key type for finding values in map of section readers.
    typedef
    std::pair<const Section::SectionType, const BinaryReader*> MapKey;

    /// Map type that contains instances of registered section readers.
    typedef std::map<MapKey, const SectionReader*> MapType;

    /// Contains section readers for all kinds of sections and all kinds of
    /// binary formats that are supported.
    static MapType* prototypes_;
};
}

#endif
