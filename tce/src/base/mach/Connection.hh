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
 * @file Connection.hh
 *
 * Declaration of Connection class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#ifndef TTA_CONNECTION_HH
#define TTA_CONNECTION_HH

#include "ObjectState.hh"

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
