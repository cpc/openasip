/**
 * @file SectionReader.hh
 *
 * Declaration of SectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
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
