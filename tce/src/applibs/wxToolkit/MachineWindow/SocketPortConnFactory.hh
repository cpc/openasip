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
 * @file SocketPortConnFactory.hh
 *
 * Declaration of SocketPortConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_SOCKET_PORT_CONN_FACTORY_HH
#define TTA_SOCKET_PORT_CONN_FACTORY_HH

#include "ConnectionFactory.hh"

class EditPart;

/**
 * Factory for creating socket-to-port connection EditParts.
 */
class SocketPortConnFactory : public ConnectionFactory {
public:
    SocketPortConnFactory();
    virtual ~SocketPortConnFactory();

    virtual EditPart* createConnection(
	EditPart* source,
	EditPart* target) const;

private:
    /// Assignment not allowed.
    SocketPortConnFactory& operator=(SocketPortConnFactory& old);
    /// Copying not allowed.
    SocketPortConnFactory(SocketPortConnFactory& old);
};

#endif
