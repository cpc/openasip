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

using namespace TTAMachine;
using namespace TTAProgram;

SimpleGuardAllocator::SimpleGuardAllocator(){}

SimpleGuardAllocator::~SimpleGuardAllocator(){}

void
SimpleGuardAllocator::start() 
    throw (Exception) {
    
    RegisterMap regMap(*target_);
    for (int proci = 0; proci < program_->procedureCount(); proci++ ) {
        Procedure& proc = program_->procedure(proci);
        for( int insi = 0; insi < proc.instructionCount(); insi++ ) {
            Instruction& ins = proc.instructionAtIndex(insi);
            for( int movei = 0; movei < ins.moveCount(); movei++ ) {
                Move& move = ins.move(movei);

                if (!move.isUnconditional()) {
                    allocateGuard(move,regMap);
                }
                if (move.destination().isGPR()) {
                    TerminalRegister& tr = dynamic_cast
                        <TerminalRegister&>(move.destination());
                    if (tr.registerFile().width() == 1) {
                        move.setDestination(regMap.brTerminal(0).copy());
                    }
                }
            }                
        }
    }
}

void
SimpleGuardAllocator::allocateGuard(Move& move,RegisterMap& regMap) {
        const RegisterGuard* guard =
        dynamic_cast<RegisterGuard*>(&move.guard().guard());
    if (guard == NULL) {
        std::string msg = "Unexpected guard in move.";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    const TerminalRegister& guardReg = regMap.brTerminal(guard->registerIndex());
    
    Machine::BusNavigator busNav = target_->busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int i = 0; i < bus->guardCount(); i++) {
            RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(bus->guard(i));
            if (regGuard != NULL &&
                regGuard->registerFile() == &guardReg.registerFile() &&
                regGuard->registerIndex() == (int)guardReg.index() &&
                regGuard->isInverted() == guard->isInverted()) {
                move.setGuard(new MoveGuard(*regGuard));
                break;
            }
        }
    }
}
/**
 * A short description of the module, usually the module name,
 * in this case "SimpleGuardAllocator".
 *
 * @return The description as a string.
 */   
std::string
SimpleGuardAllocator::shortDescription() const {
    return "Startable: SimpleGuardAllocator";
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
SimpleGuardAllocator::longDescription() const {
    std::string answer = "Startable: SimpleGuardAllocator";
    return answer;
}
