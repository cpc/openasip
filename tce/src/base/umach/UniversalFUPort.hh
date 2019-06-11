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
