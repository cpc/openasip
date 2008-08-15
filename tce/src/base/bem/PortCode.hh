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
 * @file PortCode.hh
 *
 * Declaration of PortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_CODE_HH
#define TTA_PORT_CODE_HH

#include <string>
#include "Exception.hh"

class SocketCodeTable;
class ObjectState;

/**
 * Base class for FUPortCode, RFPortCode and IUPortCode.
 */
class PortCode {
public:
    virtual ~PortCode();

    std::string unitName() const;

    bool hasEncoding() const;
    unsigned int encoding() const
        throw (NotAvailable);
    unsigned int extraBits() const;
    int width() const;
    int encodingWidth() const;
    int indexWidth() const;

    SocketCodeTable* parent() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for PortCode.
    static const std::string OSNAME_PORT_CODE;
    /// ObjectState attribute key for the name of the unit.
    static const std::string OSKEY_UNIT_NAME;
    /// ObjectState attribute key for the encoding.
    static const std::string OSKEY_ENCODING;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;
    /// ObjectState attribute key for the widht of register index.
    static const std::string OSKEY_INDEX_WIDTH;

protected:
    PortCode(
        const std::string& unitName,
        unsigned int encoding,
        unsigned int extraBits,
        int indexWidth)
        throw (OutOfRange);
    PortCode(
        const std::string& unitName,
        int indexWidth)
        throw (OutOfRange);
    PortCode(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void setParent(SocketCodeTable* parent);

private:
    /// Name of the unit.
    std::string unitName_;
    /// The encoding.
    unsigned int encoding_;
    /// The number of extra bits in the encoding.
    unsigned int extraBits_;
    /// The width of the register index in the port code.
    int indexWidth_;
    /// Tells whether the port code has an encoding.
    bool hasEncoding_;
    /// The parent socket code table.
    SocketCodeTable* parent_;
};

#endif
