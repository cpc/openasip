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
