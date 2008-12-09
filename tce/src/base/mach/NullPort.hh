/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file NullPort.hh
 *
 * Declaration of NullPort class
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_PORT_HH
#define TTA_NULL_PORT_HH

#include "Port.hh"

namespace TTAMachine {

class RegisterFile;

/**
 * A singleton class which represents a null Port.
 *
 * All the methods abort the program.
 */
class NullPort : public Port {
public:
    static NullPort& instance();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual int width() const;
    virtual void attachSocket(Socket& socket)
        throw (IllegalRegistration, ComponentAlreadyExists,
               IllegalConnectivity);
    virtual void detachSocket(Socket& socket)
        throw (InstanceNotFound);
    virtual void detachAllSockets();
    virtual Socket* inputSocket() const;
    virtual Socket* outputSocket() const;
    virtual Socket* unconnectedSocket(int index) const
        throw (OutOfRange);
    virtual int socketCount() const;
    virtual bool isConnectedTo(const Socket& socket) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullPort();
    virtual ~NullPort();

    /// The only NullPort instance.
    static NullPort instance_;
    /// The parent unit of the null port.
    static RegisterFile regFile_;
};
}

#endif
