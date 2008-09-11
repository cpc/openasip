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
 * @file ImmediateEncoding.hh
 *
 * Declaration of ImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_ENCODING_HH
#define TTA_IMMEDIATE_ENCODING_HH

#include "Encoding.hh"

class SourceField;
class ObjectState;

/**
 * Represents the encoding for inline immediate within the source field.
 */
class ImmediateEncoding : public Encoding {
public:
    ImmediateEncoding(
        unsigned int encoding,
        unsigned int extraBits,
        int immediateWidth,
        SourceField& parent)
        throw (OutOfRange, ObjectAlreadyExists);
    ImmediateEncoding(const ObjectState* state, SourceField& parent)
        throw (ObjectAlreadyExists, ObjectStateLoadingException);
    virtual ~ImmediateEncoding();

    SourceField* parent() const;
    int immediateWidth() const;
    int encodingWidth() const;
    int encodingPosition() const;
    int immediatePosition() const;

    virtual int bitPosition() const;
    virtual int width() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for immediate encoding.
    static const std::string OSNAME_IMM_ENCODING;
    /// ObjectState attribute key for the immediate width.
    static const std::string OSKEY_IMM_WIDTH;

private:
    int immediateWidth_;
};

#endif
