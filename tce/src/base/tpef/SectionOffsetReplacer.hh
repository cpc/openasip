/**
 * @file SectionOffsetReplacer.hh
 *
 * Declaration of SectionOffsetReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_OFFSET_REPLACER_HH
#define TTA_SECTION_OFFSET_REPLACER_HH

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Replaces object reference with section offset. Section offset key
 * must be stored in reference manager for that object whose section
 * offset is written.
 */
class SectionOffsetReplacer : public ValueReplacer {
public:
    SectionOffsetReplacer(const SafePointable* obj);
    virtual ~SectionOffsetReplacer();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionOffsetReplacer(const SectionOffsetReplacer& replacer);
    SectionOffsetReplacer operator=(const SectionOffsetReplacer&);
};
}

#endif
