/**
 * @file BinaryReader.hh
 *
 * Declaration of BinaryReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARYREADER_HH
#define TTA_BINARYREADER_HH

#include <set>

#include "BinaryStream.hh"
#include "Binary.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Abstract base class for each actual BinaryReader class.
 *
 * Handles registrations and storing of every BinaryReader instance.
 * BinaryReaders implement Singleton design pattern.
 */
class BinaryReader {
public:
    typedef unsigned long Length;

    virtual ~BinaryReader();

    static Binary* readBinary(BinaryStream& stream)
        throw (Exception);
protected:
    BinaryReader();

    static void registerBinaryReader(BinaryReader* reader);

    /// Does actual reading and constructing part.
    virtual Binary* readData(BinaryStream& stream) const
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue) = 0;

    /// Checks if given stream can be read with this BinaryReader class.
    virtual bool isMyStreamType(BinaryStream& stream) const
        throw (UnreachableStream) = 0;


private:
    BinaryReader(const BinaryReader&);

    /// Contains instances of concrete BinaryReaders.
    static std::set<BinaryReader*>* prototypes_;
};
}

#endif
