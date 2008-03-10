/**
 * @file FileOffsetReplacer.hh
 *
 * Declaration of FileOffsetReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_FILE_OFFSET_REPLACER_HH
#define TTA_FILE_OFFSET_REPLACER_HH

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Replaces object reference with file offset. File offset key
 * must be stored in reference manager for that object whose file
 * offset is written.
*/
class FileOffsetReplacer : public ValueReplacer {
public:
    FileOffsetReplacer(const SafePointable* obj);
    virtual ~FileOffsetReplacer();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    FileOffsetReplacer(const FileOffsetReplacer& replacer);
    FileOffsetReplacer operator=(const FileOffsetReplacer&);
};
}

#endif
