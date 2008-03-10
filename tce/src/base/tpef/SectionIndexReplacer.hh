/**
 * @file SectionIndexReplacer.hh
 *
 * Declaration of SectionIndexReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_INDEX_REPLACER_HH
#define TTA_SECTION_INDEX_REPLACER_HH

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Replaces object reference with section index. Section index key
 * must be stored in reference manager for that object whose section
 * index is written.
 */
class SectionIndexReplacer : public ValueReplacer {
public:
    SectionIndexReplacer(const SafePointable* obj, Byte fieldSize);
    virtual ~SectionIndexReplacer();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionIndexReplacer(const SectionIndexReplacer& replacer);
    SectionIndexReplacer operator=(const SectionIndexReplacer&);

    void writeReplacement(Word value);

    /// Field how many bytes of stream are replaced with section index.
    const Byte fieldSize_;
};
}

#endif
