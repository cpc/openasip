/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file LineNumProcedure.hh
 *
 * Declaration of LineNumProcedure class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
