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
 * @file SymbolElement.hh
 *
 * Declaration of SymbolElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_SYMBOL_ELEMENT_HH
#define TTA_SYMBOL_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "SafePointable.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "Chunk.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Symbol table entry.
 */
class SymbolElement : public SectionElement {
public:
    /// Binding types of symbol.
    enum SymbolBinding {
        STB_LOCAL = 0x0,  ///< Not visible outside the object file that
                          ///< contains it's definition.
        STB_GLOBAL = 0x1, ///< Visible to all files that are combined
                          ///< in TPEF file.
        STB_WEAK = 0x2    ///< Visible to all object files that are combined
                          ///< in the TPEF file, but with linkage priority
                          ///< lower than STB_GLOBAL symbols.
    };

    /// Type of symbol element.
    enum SymbolType {
        STT_NOTYPE = 0x0, ///< Type is not defined.
        STT_DATA = 0x1,   ///< Associated with data object.
        STT_CODE = 0x2,   ///< Associated with executable code.
        STT_SECTION = 0x3,///< Associated with section.
        STT_FILE = 0x4,   ///< Name of symbol gives the name of source file
                          ///< associated with this object file.
        STT_PROCEDURE = 0x5 ///< Symbol gives indicates procedure start
                            ///< position in section
    };

    SymbolElement();
    virtual ~SymbolElement();

    /// Returns type of symbol.
    virtual SymbolType type() const = 0;

    bool absolute() const;
    void setAbsolute(bool anAbsoluteness);

    SymbolBinding binding() const;
    void setBinding(SymbolBinding aBinding);

    Chunk* name() const;
    void setName(Chunk* aName);
    void setName(const ReferenceManager::SafePointer* aName);

    Section* section() const;
    void setSection(Section* aSect);
    void setSection(const ReferenceManager::SafePointer* aSect);

private:
    /// Is symbol absolutely or relocating.
    bool absolute_;
    /// Binding of the symbol.
    SymbolBinding bind_;
    /// The name of the symbol.
    const ReferenceManager::SafePointer* name_;
    /// Section to which the symbol belongs.
    const ReferenceManager::SafePointer* section_;
};
}

#include "SymbolElement.icc"

#endif
