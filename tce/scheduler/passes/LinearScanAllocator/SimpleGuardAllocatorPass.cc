/**
 * @file SimpleGuardAllocator.cc
 *
 * Implementation of SimpleGuardAllocator class.
 * 
 * Stupid guard allocator
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
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
