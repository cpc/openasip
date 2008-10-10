/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
