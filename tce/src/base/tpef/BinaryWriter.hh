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
 * @file BinaryWriter.hh
 *
 * Declaration of BinaryWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARY_WRITER_HH
#define TTA_BINARY_WRITER_HH

#include "Binary.hh"
#include "Section.hh"
#include "BinaryStream.hh"

namespace TPEF {

/**
 * Interface for actual binary writer classes and for clients
 * to write binary object representation.
 *
 * Provides a way for calling the concrete writers with better error checking.
 */
class BinaryWriter {
public:
    virtual ~BinaryWriter();

    void writeBinary(BinaryStream& stream, const Binary* bin) const;

protected:
    BinaryWriter();

    /// Does real writing of binary.
    virtual void actualWriteBinary(
        BinaryStream& stream,
        const Binary* bin) const = 0;

    void setWriter(const BinaryWriter* writer);

private:
    /// Contains concrete writer instance that is used for writing binary.
    const BinaryWriter* writerToUse_;
};
}

#endif
