/**
 * @file DataSymElement.hh
 *
 * Declaration of DataSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DATA_SYM_ELEMENT_HH
#define TTA_DATA_SYM_ELEMENT_HH

#include "SymbolElement.hh"

namespace TPEF {

class DataSymElement : public SymbolElement {
 public:
    DataSymElement();
    virtual ~DataSymElement();

    virtual SymbolType type() const;

    Chunk* reference() const;
    void setReference(Chunk* aReference);
    void setReference(const ReferenceManager::SafePointer* aReference);

    Word size() const;
    void setSize(Word aSize);

 private:
    /// Referred object.
    const ReferenceManager::SafePointer* reference_;
    /// Size of referred object in MAUs.
    Word size_;
};
}

#endif
