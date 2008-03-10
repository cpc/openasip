/**
 * @file DebugStabElem.hh
 *
 * Declaration of DebugStabElem class.
 *
 * @author Mikael Lepistö 2006 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DEBUG_STAB_ELEMENT_HH
#define TTA_DEBUG_STAB_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "DebugElement.hh"

namespace TPEF {

/**
 * Stab debug element type.
 */
class DebugStabElem : public DebugElement {
public:
    DebugStabElem(std::vector<Byte>& data);

    DebugStabElem(
        Byte stabType,
        Byte other,
        HalfWord description,
        Word value);

    virtual ~DebugStabElem();

    virtual ElementType type() const;

    virtual Byte byte(Word index) const throw (OutOfRange);
    virtual Word length() const;

    Byte stabType() const;
    Byte other() const;
    HalfWord description() const;
    Word value() const;

private:
    /// a.out stab type.
    Byte stabType_;
    /// a.out stab other field.
    Byte other_;
    /// a.out stab description field.
    HalfWord description_;
    /// a.out stab value field.
    Word value_;
};

}

#endif
