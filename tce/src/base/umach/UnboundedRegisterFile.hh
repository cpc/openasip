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
 * @file UnboundedRegisterFile.hh
 *
 * Declaration of UnboundedRegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_REGISTER_FILE_HH
#define TTA_UNIVERSAL_REGISTER_FILE_HH

#include "RegisterFile.hh"

/**
 * UnboundedRegisterFile class represents a register file which has
 * unlimited amount of registers. This kind of register files are used in
 * UniversalMachine.
 */
class UnboundedRegisterFile : public TTAMachine::RegisterFile {
public:
    UnboundedRegisterFile(
        const std::string& name,
        int width,
        RegisterFile::Type type)
        throw (InvalidName, OutOfRange);
    virtual ~UnboundedRegisterFile();

    virtual int numberOfRegisters() const;

    virtual void setMaxReads(int reads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setType(RegisterFile::Type type);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
};

#endif
