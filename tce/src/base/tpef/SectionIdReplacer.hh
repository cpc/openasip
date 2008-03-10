/**
 * @file SectionIdReplacer.hh
 *
 * Declaration of SectionIdReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_ID_REPLACER_HH
#define TTA_SECTION_ID_REPLACER_HH

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Replaces object references with the section identification code that
 * identifies the object. The reference manager must contain a SectionId key
 * for the objects whose section identification code is written out.
 */
class SectionIdReplacer : public ValueReplacer {
public:
    SectionIdReplacer(const SafePointable* obj);
    virtual ~SectionIdReplacer();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionIdReplacer(const SectionIdReplacer& replacer);
    SectionIdReplacer operator=(const SectionIdReplacer&);
};
}

#endif
