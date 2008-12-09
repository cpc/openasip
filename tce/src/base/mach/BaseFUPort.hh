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
 * @file BaseFUPort.hh
 *
 * Declaration of BaseFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASE_FU_PORT_HH
#define TTA_BASE_FU_PORT_HH

#include "Port.hh"

namespace TTAMachine {

/**
 * BaseFUPort is an abstract base class for FUPort and SpecialRegisterPort
 * classes.
 */
class BaseFUPort : public Port {
public:
    virtual ~BaseFUPort();

    FunctionUnit* parentUnit() const;

    virtual int width() const;
    void setWidth(int width)
        throw (OutOfRange);

    virtual bool isTriggering() const = 0;
    virtual bool isOpcodeSetting() const = 0;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState attribute key for bit width of the port.
    static const std::string OSKEY_WIDTH;

protected:
    BaseFUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent)
        throw (InvalidName, ComponentAlreadyExists, OutOfRange);
    BaseFUPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);

private:
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Bit width of the port.
    int width_;
};
}

#endif
