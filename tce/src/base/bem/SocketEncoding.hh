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
