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
 * @file AOutSymbolSectionReader.hh
 *
 * Declaration of AOutSymbolSectionReader.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 23 October 2003 by pj, am, ll, jn
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_SYMBOL_SECTION_READER_HH
#define TTA_AOUT_SYMBOL_SECTION_READER_HH

#include <deque>

#include "AOutSectionReader.hh"
#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

    class SymbolElement;
    class AOutReader;

/**
 * Reads symbol table section from a.out binary file.
 */
class AOutSymbolSectionReader : public AOutSectionReader {

public:
    virtual ~AOutSymbolSectionReader();

protected:

    AOutSymbolSectionReader();

    virtual Section::SectionType type() const;

    virtual void finalize(Section* section) const;

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

private:

    /**
     * All the information of read a.out stab.
     */
    struct StabData {
        StabData(
            Byte aType, Byte anOther, HalfWord aDescr,
            Word aValue, Word aName) :
            stabType(aType), other(anOther), description(aDescr),
            value(aValue), nameOffset(aName) {
        }
                
        /// Stab type.
        Byte stabType;
        /// Stab other field.
        Byte other;
        /// Description field.
        HalfWord description;
        /// Value of the stab.
        Word value;
        /// Offset to string table.
        Word nameOffset;
    };

    SymbolElement* initializeSymbol(
        BinaryStream& stream,
        SectionOffset sectionOffset,
        AOutReader* reader) const
        throw (KeyAlreadyExists, UnreachableStream, OutOfRange);

    /// Copying not allowed.
    AOutSymbolSectionReader(const AOutSymbolSectionReader&);
    /// Assignment not allowed.
    AOutSymbolSectionReader& operator=(const AOutSymbolSectionReader&);

    /// Data symbol size in bytes.
    static const Word DATA_SYMBOL_SIZE;
    
    /// One of the symbols that are used to indicate start of new 
    /// compilation module in a.out symbol table
    static const std::string GCC_MODULE_START_SYMBOL1;

    /// One of the symbols that are used to indicate start of new 
    /// compilation module in a.out symbol table
    static const std::string GCC_MODULE_START_SYMBOL2;

    /// Class-wide (unique) prototype instance of AOutSymbolSectionReader
    /// registered into SectionReader.
    static AOutSymbolSectionReader proto_;

    /**
     * Map of recources for each compilation module.
     *
     * In linked a.out the same resource symbols are added for
     * every different compilation unit, with a different value.
     *
     * This class stores information how resource ids should be replaced
     * for instructions starting from startAddress. There is own
     * replacement map for each compilation unit.
     */
    struct CompilationModule {        
        CompilationModule(Word startAddress) : 
            startAddress_(startAddress) {
        }
        
        /// Start address of linked compilation module in a.out
        Word startAddress_;
        
        /// Resources id replacements for the compilation module.
        std::map<Word, SectionOffset> resources_;
    };
    
    /// Resource id:s for strings.
    mutable std::map<std::string, Word> resolvedResources_;
    
    /// List of compilation modules of a.out
    mutable std::vector<CompilationModule> addedResources_;

    mutable std::vector< std::pair<Word, std::string> > annotationes_;
};
}

#endif
