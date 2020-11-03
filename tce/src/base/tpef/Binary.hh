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
 * @file Binary.hh
 *
 * Declaration of Binary class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARY_HH
#define TTA_BINARY_HH

#include <list>
#include <map>
#include "SafePointer.hh"
#include "Section.hh"
#include "StringSection.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Root class of tpef binary representation.
 */
class Binary {
public:
    /**
     * Storing types of TTA code.
     */
    enum FileType {
        FT_NULL    = 0x00, ///< Illegal/undefined type.
        FT_OBJSEQ  = 0x04, ///< Sequential TTA object code.
        FT_PURESEQ = 0x05, ///< Sequential TTA code, no unresolved symbols.
        FT_LIBSEQ  = 0x06, ///< Collection of sequential object code files
                           ///< for linkage.
        FT_MIXED   = 0x09, ///< Partly scheduled code.
        FT_PARALLEL = 0x0D ///< Fully scheduled or mixed code.
    };

    /**
     * Encodings of program sections.
     */
    enum FileArchitecture {
        FA_NOARCH   = 0x00, ///< Illegal/undefined architecture.
        FA_TTA_MOVE = 0x01, ///< Move design framework.
        FA_TTA_TUT  = 0x02, ///< New TTA template.
        FA_TDS_TI   = 0x03  ///< TTA Design Studio architecture.
    };

    Binary();
    virtual ~Binary();

    void addSection(Section* section);

    Section* section(Word index) const;
    Section* section(Section::SectionType type, Word number) const;

    Word sectionCount() const;
    Word sectionCount(Section::SectionType type) const;

    StringSection* strings() const;
    void setStrings(StringSection* strTable);
    void setStrings(const ReferenceManager::SafePointer* strTable);

    FileArchitecture arch() const;
    void setArch(FileArchitecture arch);

    FileType type() const;
    void setType(FileType type);

    void setTPEFVersion(TPEFHeaders::TPEFVersion version);
    TPEFHeaders::TPEFVersion TPEFVersion() const;

private:
    Binary(const Binary&);
    Binary operator=(const Binary&);

    /// Reference to string table of binary.
    const ReferenceManager::SafePointer* strings_;

    /// Stores sections of binary.
    std::vector<Section*> sections_;

    /// Binary's file type. See enumerations for more info.
    FileType fileType_;
    /// Binary's file architecture. See enumerations for more info.
    FileArchitecture fileArch_;

    /// Indicates TPEF format version used.
    TPEFHeaders::TPEFVersion tpefVersion_;
};
}

#include "Binary.icc"

#endif
