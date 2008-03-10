/**
 * @file FileSymElement.hh
 *
 * Declaration of FileSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_FILE_SYM_ELEMENT_HH
#define TTA_FILE_SYM_ELEMENT_HH

#include "SymbolElement.hh"

namespace TPEF {

/**
 * File Symbol element.
 */
class FileSymElement : public SymbolElement {
 public:
    FileSymElement();
    virtual ~FileSymElement();

    virtual SymbolType type() const;

    Word value() const;
    void setValue(Word aValue);

 private:
    /// Value of the symbol.
    Word value_;
};
}

#endif
