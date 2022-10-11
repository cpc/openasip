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
 * @file Connection.hh
 *
 * Declaration of Connection class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#ifndef TTA_CONNECTION_HH
#define TTA_CONNECTION_HH

#include <string>

class ObjectState;
namespace TTAMachine {

class Segment;
class Socket;

/**
 * Joins Bus and Socket objects if they are connected in the machine.
 */
class Connection {
public:
    Connection(Socket& socket, Segment& bus);
    ~Connection();
    Segment* bus() const;
    Socket* socket() const;

    ObjectState* saveState() const;

    /// ObjectState name for Connection.
    static const std::string OSNAME_CONNECTION;
    /// ObjectState attribute key for socket name.
    static const std::string OSKEY_SOCKET;
    /// ObjectState attribute key for bus name.
    static const std::string OSKEY_BUS;
    /// ObjectState attribute key for segment name.
    static const std::string OSKEY_SEGMENT;

private:
    /// Socket which is connected to the bus.
    Socket* socket_;
    /// Bus which is connected to the socket.
    Segment* bus_;
};
}

#include "Connection.icc"

#endif
