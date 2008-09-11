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
 * @file RawImageWriter.hh
 *
 * Declaration of RawImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RAW_IMAGE_WRITER_HH
#define TTA_RAW_IMAGE_WRITER_HH

#include "BitImageWriter.hh"

class BitVector;

/**
 * Bit image writer that writes a real binary image.
 */
class RawImageWriter : public BitImageWriter {
public:
    RawImageWriter(const BitVector& bits);
    virtual ~RawImageWriter();

    virtual void writeImage(std::ostream& stream) const;

private:
    static char character(const BitVector& bits, unsigned int startIndex);

    /// The bits to be written.
    const BitVector& bits_;
};

#endif
