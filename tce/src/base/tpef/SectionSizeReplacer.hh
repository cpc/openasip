/**
 * @file SectionSizeReplacer.hh
 *
 * Declaration of SectionSizeReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_SIZE_REPLACER_HH
#define TTA_SECTION_SIZE_REPLACER_HH

#include <map>

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Replaces object reference with size of object. Size for object must be
 * stored with static setSize method of this class.
 */
class SectionSizeReplacer : public ValueReplacer {
public:
    SectionSizeReplacer(const SafePointable* obj);
    virtual ~SectionSizeReplacer();

    static void setSize(const SafePointable* obj, Word size);
    static void clear();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionSizeReplacer(const SectionSizeReplacer& replacer);
    SectionSizeReplacer operator=(const SectionSizeReplacer&);

    /// Contains sections sizes.
    static std::map<const SafePointable*, Word> sizeMap;
};
}

#endif
