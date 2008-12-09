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
 * @file SpecialRegisterPort.hh
 *
 * Declaration of SpecialRegisterPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SPECIAL_REGISTER_PORT_HH
#define TTA_SPECIAL_REGISTER_PORT_HH

#include "BaseFUPort.hh"

namespace TTAMachine {

class ControlUnit;

/**
 * SpecialRegisterPort class represent a port of a special register in
 * control unit.
 *
 * An example of such a port is the return address port.
 */
class SpecialRegisterPort : public BaseFUPort {
public:
    SpecialRegisterPort(
        const std::string& name,
        int width,
        ControlUnit& parent)
        throw (InvalidName, OutOfRange, ComponentAlreadyExists);
    SpecialRegisterPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~SpecialRegisterPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for special register port.
    static const std::string OSNAME_SPECIAL_REG_PORT;
};
}

#endif
