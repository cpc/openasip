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
 * @file AddressSpaceCheck.hh
 *
 * Declaration of AddressSpaceCheck class that checks two things:
 * 1) if the function units' operations access memory
 * 2) if there is a memory address space available for them
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ADDRESS_SPACE_CHECK_HH
#define ADDRESS_SPACE_CHECK_HH

#include "MachineCheck.hh"

namespace TTAMachine {
    class Machine;
}

class OperationPool;

/**
 * MachineCheck to check if the FUs' operations access memory and
 * there is a memory address space available for them.
 */
class AddressSpaceCheck : public MachineCheck {
public:
    AddressSpaceCheck(OperationPool& operationPool);
    virtual ~AddressSpaceCheck();

    virtual bool check(const TTAMachine::Machine& mach,
                       MachineCheckResults& results) const;
    
private:
    OperationPool& operationPool_;

};

#endif
