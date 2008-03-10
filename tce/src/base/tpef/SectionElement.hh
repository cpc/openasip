/**
 * @file SectionElement.hh
 *
 * Declaration of SectionElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_ELEMENT_HH
#define TTA_SECTION_ELEMENT_HH

#include "SafePointable.hh"

namespace TPEF {

/**
 * Abstract base class for all section elements.
 */
class SectionElement : public SafePointable {
public:
    virtual ~SectionElement();
protected:
    SectionElement();

};
}

#endif
