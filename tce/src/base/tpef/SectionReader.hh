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

#include "BinaryStream.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

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
        BinaryStream &stream,
        Section* section,
        BinaryReader* reader)
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    static void finalizeBinary(
        Binary *binaryToFinalize,
        BinaryReader* reader);

protected:
    SectionReader();

    /// Does actual reading part for constructing section.
    virtual void readData(
        BinaryStream &stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue) = 0;

    /// Returns binary reader whose sections actual section reader reads.
    virtual BinaryReader* parent() const = 0;

    /// Returns type (TPEF) section which actual section reader reads.
    virtual Section::SectionType type() const = 0;

    virtual void finalize(Section* section) const;

    static const SectionReader* findSectionReader(
        const Section::SectionType type,
        const BinaryReader* bReader)
        throw (InstanceNotFound);

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
