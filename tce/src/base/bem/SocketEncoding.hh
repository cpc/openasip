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
 * @file SocketEncoding.hh
 *
 * Declaration of SocketEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_ENCODING_HH
#define TTA_SOCKET_ENCODING_HH

#include "Encoding.hh"

class SocketCodeTable;
class SlotField;

/**
 * The SocketEncoding class models how socket sources or destinations are
 * mapped to control codes.
 *
 * Sockets have an independent encoding in each source or destination
 * field they appear in. A socket encoding can be more complex than a
 * fixed control code and encode many destinations or sources all
 * reached from the same socket. In this case, the socket encoding
 * contains SocketCodeTable instance.
 */
class SocketEncoding : public Encoding {
public:
    SocketEncoding(
	const std::string& name,
	unsigned int encoding,
	unsigned int extraBits,
	SlotField& parent)
	throw (ObjectAlreadyExists);
    SocketEncoding(const ObjectState* state, SlotField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~SocketEncoding();

    SlotField* parent() const;

    std::string socketName() const;

    void setSocketCodes(SocketCodeTable& codeTable);
    void unsetSocketCodes();
    bool hasSocketCodes() const;
    SocketCodeTable& socketCodes() const;

    int socketIDPosition() const;
    int socketIDWidth() const;
    virtual int width() const;
    virtual int bitPosition() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for socket encoding.
    static const std::string OSNAME_SOCKET_ENCODING;
    /// ObjectState attribute key for the name of the socket.
    static const std::string OSKEY_SOCKET_NAME;
    /// ObjectState attribute key for the name of the socket code table.
    static const std::string OSKEY_SC_TABLE;

private:
    /// Name of the socket.
    std::string name_;
    /// Socket code table.
    SocketCodeTable* socketCodes_;
};

#endif
