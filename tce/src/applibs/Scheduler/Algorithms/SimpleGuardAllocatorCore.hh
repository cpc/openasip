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
 * @file SimpleGuardAllocatorCore.hh
 *
 * Declaration of SimpleGuardAllocatorCore class.
 * 
 * Stupid guard allocator. does not use more than one guard register,
 * Does not do any if-conversion etc.
 *
 * @author Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_GUARD_ALLOCATOR_CORE_HH
#define TTA_SIMPLE_GUARD_ALLOCATOR_CORE_HH

#include "Exception.hh"

namespace TTAProgram {
    class Move;
    class Procedure;
}

namespace TTAMachine {
    class Machine;
}

class RegisterMap;
class InterPassData;

class SimpleGuardAllocatorCore {

//public:
    SimpleGuardAllocatorCore();
    virtual ~SimpleGuardAllocatorCore();

public:
    static void allocateGuards(
        TTAProgram::Procedure& proc, const TTAMachine::Machine& mach, 
        InterPassData& interPassData) 
        throw (Exception);

private:
    static void allocateGuard(
        TTAProgram::Move& move, RegisterMap& regMap, 
        const TTAMachine::Machine& mach) throw (Exception);
};

#endif
