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
 * @file SimpleGuardAllocator.cc
 *
 * Implementation of SimpleGuardAllocator class.
 * 
 * Stupid guard allocator
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Program.hh"

/*
#include "Procedure.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "TerminalRegister.hh"
#include "RegisterFile.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "Machine.hh"

#include "RegisterMap.hh"
#include "SimpleGuardAllocator.hh"
*/

#include "SimpleGuardAllocatorCore.hh"

#include "Program.hh"
#include "SimpleGuardAllocatorPass.hh"

using namespace TTAMachine;
using namespace TTAProgram;

SCHEDULER_PASS(SimpleGuardAllocatorPass)

SimpleGuardAllocatorPass::SimpleGuardAllocatorPass(){}

SimpleGuardAllocatorPass::~SimpleGuardAllocatorPass(){}

void
SimpleGuardAllocatorPass::start() 
    throw (Exception) {

    for( int i = 0; i < program_->procedureCount(); i++) {
        SimpleGuardAllocatorCore::allocateGuards(
            program_->procedure(i), *target_,interPassData());
    }
}

/**
 * A short description of the module, usually the module name,
 * in this case "SimpleGuardAllocatorPass".
 *
 * @return The description as a string.
 */   
std::string
SimpleGuardAllocatorPass::shortDescription() const {
    return "Startable: SimpleGuardAllocatorPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
SimpleGuardAllocatorPass::longDescription() const {
    std::string answer = "Startable: SimpleGuardAllocatorPass";
    return answer;
}
