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
 * @file SocketPortConnCmd.cc
 *
 * Definition of SocketPortConnCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "SocketPortConnCmd.hh"
#include "Port.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param port Port to connect/disconnect.
 * @param socket Socket to connect/disconnect.
 */
SocketPortConnCmd::SocketPortConnCmd(Socket* socket, Port* port):
    ComponentCommand(),
    socket_(socket), port_(port) {
}


/**
 * The Destructor.
 */
SocketPortConnCmd::~SocketPortConnCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SocketPortConnCmd::Do() {

    if (port_->isConnectedTo(*socket_)) {
        try {
            port_->detachSocket(*socket_);
        } catch (Exception& e) {
            return false;
        }
    } else {
        try {
            port_->attachSocket(*socket_);
        } catch (Exception& e) {
            return false;
        }
    }

    return true;
}
