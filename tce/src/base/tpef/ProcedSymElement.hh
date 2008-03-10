/**
 * @file ProcedSymElement.hh
 *
 * Declaration of ProcedSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_PROCED_SYM_ELEMENT_HH
#define TTA_PROCED_SYM_ELEMENT_HH

#include "CodeSymElement.hh"
#include "InstructionElement.hh"

namespace TPEF {

/**
 * Procedure symbol.
 */
class ProcedSymElement : public CodeSymElement {
 public:
    ProcedSymElement();
    virtual ~ProcedSymElement();
    virtual SymbolType type() const;
};
}

#endif
