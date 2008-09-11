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
 * @file UniversalFUPort.hh
 *
 * Declaration of UniversalFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_FU_PORT_HH
#define TTA_UNIVERSAL_FU_PORT_HH

#include "FUPort.hh"

class UniversalFunctionUnit;

/**
 * UniversalFUPort class represents a port of
 * UniversalFunctionUnit. The port is similar to normal FUPort but
 * there can be several operation code setting UniversalFUPorts in
 * UniversalFunctionUnit. UniversalFUPort is a fixed port. Its properties
 * cannot be changes after it is created.
 */
class UniversalFUPort : public TTAMachine::FUPort {
public:
    UniversalFUPort(
        const std::string& name,
        int width,
        UniversalFunctionUnit& parent,
        bool isTriggering,
        bool setsOpcode)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);
    virtual ~UniversalFUPort();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setTriggering(bool triggers);
    virtual void setOpcodeSetting(bool setsOpcode)
        throw (ComponentAlreadyExists);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
};

#endif
