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
