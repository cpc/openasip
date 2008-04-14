/**
 * @file SimpleGuardAllocatorCore.cc
 *
 * Implementation of SimpleGuardAllocator class.
 * 
 * Stupid guard allocator
 *
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include <string>
#include <stdexcept>
#include <iostream>

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
#include "SimpleGuardAllocatorCore.hh"

using namespace TTAMachine;
using namespace TTAProgram;

/*
SimpleGuardAllocatorCore::SimpleGuardAllocator(){}

SimpleGuardAllocatorCore::~SimpleGuardAllocator(){}
*/

void
SimpleGuardAllocatorCore::allocateGuards(
    Procedure& proc, const Machine& mach, 
    InterPassData& interPassData) throw (Exception) {

    RegisterMap regMap(mach, interPassData);

    for (int insi = 0; insi < proc.instructionCount(); insi++) {
        Instruction& ins = proc.instructionAtIndex(insi);
        for (int movei = 0; movei < ins.moveCount(); movei++) {
            Move& move = ins.move(movei);

            if (!move.isUnconditional()) {
                allocateGuard(move,regMap,mach);
            }
            if (move.destination().isGPR()) {
                TerminalRegister& tr = dynamic_cast
                    <TerminalRegister&>(move.destination());
                if (tr.registerFile().width() == 1) {
                    if (regMap.brCount() == 0) {
                        std::string msg = "Suitable guard not found from"
                            " the machine. Please add Guard to some bus.";
                        throw ModuleRunTimeError(__FILE__,__LINE__,__func__, msg);
                    } 
                    move.setDestination(regMap.brTerminal(0).copy());
                }
            }
            if (move.source().isGPR()) {
                TerminalRegister& tr = dynamic_cast
                    <TerminalRegister&>(move.source());
                if (tr.registerFile().width() == 1) {
                    if (regMap.brCount() == 0) {
                        std::string msg = "Suitable guard not found from"
                            " the machine. Please add Guard to some bus.";
                        throw ModuleRunTimeError(__FILE__,__LINE__,__func__, msg);
                    } 
                    move.setSource(regMap.brTerminal(0).copy());
                }
            }

        }                
    }
}

void
SimpleGuardAllocatorCore::allocateGuard(
    Move& move, RegisterMap& regMap, const Machine& mach) 
    throw (Exception) {
        const RegisterGuard* guard =
        dynamic_cast<RegisterGuard*>(&move.guard().guard());
    if (guard == NULL) {
        std::string msg = "Unexpected guard in move.";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    try {
        const TerminalRegister& guardReg = regMap.brTerminal(guard->registerIndex());
        
        Machine::BusNavigator busNav = mach.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            for (int i = 0; i < bus->guardCount(); i++) {
                RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(bus->guard(i));
                if (regGuard != NULL &&
                    regGuard->registerFile() == &guardReg.registerFile() &&
                    regGuard->registerIndex() == (int)guardReg.index() &&
                    regGuard->isInverted() == guard->isInverted()) {
                    move.setGuard(new MoveGuard(*regGuard));
                    return;
                }
            }
        }
    } catch (std::out_of_range) {
        // can be empty..
    }
    // as we are here either because we did not find a guard or we do not have a
    // guard register
    std::string msg = "Suitable guard not found from the machine. Please add"
        " Guard to some bus.";
    throw ModuleRunTimeError(__FILE__,__LINE__,__func__, msg);
}
