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
 * @file SequentialFirstFitResourceAllocator.hh
 *
 * Declaration of SequentialFirstFitResourceAllocator class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORTED_RESOURCE_ALLOCATOR_HH
#define TTA_PORTED_RESOURCE_ALLOCATOR_HH

#include <set>
#include <list>
#include "StartableSchedulerModule.hh"
#include "Exception.hh"
#include "SimpleResourceManager.hh"

namespace TTAProgram {
    class Program;
    class Move;
    class Instruction;
    class TerminalRegister;
}

namespace TTAMachine {
    class FunctionUnit;
    class Bus;
    class RegisterFile;
    class Port;
}

class Operation;

/**
 * Schedules the given program for the target architecture without any
 * parallelisation or other optimisations. Simply maps the program on to
 * the target machine.
 */
class SequentialFirstFitResourceAllocator : public StartableSchedulerModule {
public:
    SequentialFirstFitResourceAllocator();
    virtual ~SequentialFirstFitResourceAllocator();

    virtual void start()
        throw (Exception);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    typedef std::list<MoveNode*> MoveNodeList;

    void findReservedRegisters();
    void assignResources();
    MoveNodeList createMoveNodes();
    SimpleResourceManager* resMan_;
};

#endif
