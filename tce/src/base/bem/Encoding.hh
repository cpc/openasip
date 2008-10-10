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
 * @file Encoding.hh
 *
 * Declaration of Encoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENCODING_HH
#define TTA_ENCODING_HH

#include <string>
#include "Exception.hh"

class InstructionField;
class ObjectState;

/**
 * Represents an encoding of a source of destination within a move slot.
 * This is a base class for different encodings.
 */
class Encoding {
public:
    virtual ~Encoding();

    InstructionField* parent() const;
    
    unsigned int encoding() const;
    unsigned int extraBits() const;
    virtual int width() const;

    /**
     * Returns the position of the encoding within the parent field.
     */
    virtual int bitPosition() const = 0;

    virtual ObjectState* saveState() const;

    /// ObjectState name for Encoding class.
    static const std::string OSNAME_ENCODING;
    /// ObjectState attribute key for the encoding.
    static const std::string OSKEY_ENCODING;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;

protected:
    Encoding(
        unsigned int encoding,
        unsigned int extraBits,
        InstructionField* parent);
    Encoding(const ObjectState* state, InstructionField* parent)
        throw (ObjectStateLoadingException);
    void setParent(InstructionField* parent);

private:
    /// The encoding.
    unsigned int encoding_;
    /// The number of extra bits.
    unsigned int extraBits_;
    /// The parent instruction field.
    InstructionField* parent_;
};

#endif
