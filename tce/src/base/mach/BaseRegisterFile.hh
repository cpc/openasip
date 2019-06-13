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
 * @file BaseRegisterFile.hh
 *
 * Declaration of abstract BaseRegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_BASE_REGISTER_FILE_HH
#define TTA_BASE_REGISTER_FILE_HH

#include <string>

#include "Unit.hh"
#include "RFPort.hh"

namespace TTAMachine {

/**
 * An abstract base class for register files as ImmediateUnit and
 * RegisterFile.
 */
class BaseRegisterFile : public Unit {
public:
    virtual ~BaseRegisterFile();

    virtual int numberOfRegisters() const;
    virtual int width() const;
    virtual int size() const;

    virtual void setNumberOfRegisters(int registers);
    virtual void setWidth(int width);

    virtual RFPort* port(const std::string& name) const;
    virtual RFPort* port(int index) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

    /// ObjectState name for BaseRegisterFile.
    static const std::string OSNAME_BASE_REGISTER_FILE;
    /// ObjectState attribute key for the number of registers.
    static const std::string OSKEY_SIZE;
    /// ObjectState attribute key for bit width of the registers.
    static const std::string OSKEY_WIDTH;

protected:
    BaseRegisterFile(const std::string& name, int size, int width);
    BaseRegisterFile(const ObjectState* state);

private:
    void loadStateWithoutReferences(const ObjectState* state);

    /// Number of registers in the register file.
    int size_;
    /// Bit width of the registers in the register file.
    int width_;
};
}

#include "BaseRegisterFile.icc"

#endif
