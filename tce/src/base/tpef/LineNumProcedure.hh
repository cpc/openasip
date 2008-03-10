/**
 * @file LineNumProcedure.hh
 *
 * Declaration of LineNumProcedure class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LINE_NUM_PROCEDURE_HH
#define TTA_LINE_NUM_PROCEDURE_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "SymbolElement.hh"
#include "LineNumElement.hh"

namespace TPEF {

/**
 * Procedure entry for line number section.
 *
 * Class represents one procedure and contains all line
 * number elements for procedure.
 */
class LineNumProcedure : public SectionElement {
public:
    LineNumProcedure();
    virtual ~LineNumProcedure();

    void addLine(const LineNumElement* elem);
    const LineNumElement* line(HalfWord index) const;

    HalfWord lineCount() const;

    void setProcedureSymbol(const ReferenceManager::SafePointer* aRef);
    void setProcedureSymbol(SymbolElement* aRef);
    SymbolElement* procedureSymbol() const;

private:
    /// Symbol element of procedure.
    const ReferenceManager::SafePointer* symbol_;

    /// Contains LineNumElements of procedure.
    std::vector<const LineNumElement*> lines_;
};
}

#include "LineNumProcedure.icc"

#endif
