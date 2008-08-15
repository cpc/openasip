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
 * @file Binary.hh
 *
 * Declaration of Binary class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
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
};
}

#include "Binary.icc"

#endif
