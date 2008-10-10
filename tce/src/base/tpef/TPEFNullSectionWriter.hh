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
 * @file TPEFNullSectionWriter.hh
 *
 * Declaration of TPEFNullSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_NULL_SECTION_WRITER_HH
#define TTA_TPEF_NULL_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes Null section in to stream.
 */
class TPEFNullSectionWriter : public TPEFSectionWriter {
protected:
    TPEFNullSectionWriter();
    virtual ~TPEFNullSectionWriter();

    virtual Section::SectionType type() const;

    virtual Word elementSize(const Section* section) const;

    virtual SectionId getSectionId() const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFNullSectionWriter(const TPEFNullSectionWriter&);
    /// Assignment not allowed.
    TPEFNullSectionWriter operator=(const TPEFNullSectionWriter&);

    /// Unique instance of a class.
    static const TPEFNullSectionWriter instance_;
};
}

#endif
