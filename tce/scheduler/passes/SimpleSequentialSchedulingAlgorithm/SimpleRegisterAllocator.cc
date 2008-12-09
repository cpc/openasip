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
 * @file SimpleRegisterAllocator.cc
 *
 * Implementation of SimpleRegisterAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
using std::cout;
using std::endl;

#include "SimpleRegisterAllocator.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "CodeSnippet.hh"
#include "RegisterFile.hh"
#include "TerminalRegister.hh"
#include "NullPort.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "UniversalMachine.hh"
#include "UnboundedRegisterFile.hh"
#include "NullRegisterFile.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "MapTools.hh"
#include "ControlUnit.hh"
#include "Program.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"

using std::string;
using std::pair;
using namespace TTAMachine;
using namespace TTAProgram;

/////////////////////////////////////////////////////////////////////////////
// SimpleRegisterAllocator
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
SimpleRegisterAllocator::SimpleRegisterAllocator():
    StartableSchedulerModule() {
}

/**
 * Destructor.
 */
SimpleRegisterAllocator::~SimpleRegisterAllocator() {
}

void
SimpleRegisterAllocator::start()
    throw (Exception) {    

    if (program_ == NULL || target_ == NULL) {
        string method = "PrototypeAlgorithm::start()";
        string msg =
            "Source program and/or target architecture not defined!";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, msg);
    }

    um_ = dynamic_cast<UniversalMachine*>(&program_->targetProcessor());
    if (um_ == NULL) {
        string msg = "Couldn't get UniversalMachine instance. "
            "Is source pure sequential?";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    Machine::RegisterFileNavigator regNav =
        target_->registerFileNavigator();

    for (int i = 0; i < regNav.count(); i++) {
        for (int j = 0; j < regNav.item(i)->numberOfRegisters(); j++) {
            registers_.push_back(
                new TerminalRegister(*regNav.item(i)->port(0), j));
        }
    }

    AddressSpace& instructionAS = *target_->controlUnit()->addressSpace();
    Machine::AddressSpaceNavigator asNav =
        target_->addressSpaceNavigator();
    AddressSpace* dataAS = NULL;
    for (int i = 0; i < asNav.count(); i++) {
        if (asNav.item(i) != &instructionAS) {
            dataAS = asNav.item(i);
            break;
        }
    }

    Program* newProgram = program_->copy();
    newProgram->replaceUniversalAddressSpaces(*dataAS);

    cout << "Performing register assignment...";
    cout.flush();

    Procedure* proc = &newProgram->firstProcedure();

    while (proc != &NullProcedure::instance()) {
        Instruction* ins = &proc->firstInstruction();
        while (ins != &NullInstruction::instance()) {
            allocateRegisters(*ins);
            allocateGuards(*ins);
            ins = &proc->nextInstruction(*ins);
        }

        proc = &newProgram->nextProcedure(*proc);
    }

    try {
        *program_ = *newProgram;
    } catch (const Exception& e) {
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, e.errorMessage());
    }

    cout << " done." << endl;
    cout.flush();
}

bool
SimpleRegisterAllocator::needsTarget() const {
    return true;
}

bool
SimpleRegisterAllocator::needsProgram() const {
    return true;
}

/**
 * Allocates registers used in moves of given instruction.
 */
void
SimpleRegisterAllocator::allocateRegisters(Instruction& ins) {

        for (int j = 0; j < ins.moveCount(); j++) {
            Move& move = ins.move(j);

            if (move.source().isGPR()) {
                const RegisterFile* regFile = &move.source().registerFile();
                string var = getVariableName(regFile, move.source().index());
                if (regBookkeeper_.isReserved(var)) {
                    TerminalRegister& reservedSource = regBookkeeper_.regForVar(var);
                    const RegisterFile& rf = reservedSource.registerFile();
                    int i = 0;
                    while (rf.port(i)->outputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newSource =
                        new TerminalRegister(*rf.port(i), reservedSource.index());
                    move.setSource(newSource);

                } else {
                    TerminalRegister& pendingSource = *findFreeRegister(var);
                    regBookkeeper_.reserveRegister(pendingSource, var);
                    const RegisterFile& rf = pendingSource.registerFile();
                    int i = 0;
                    while (rf.port(i)->outputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newSource =
                        new TerminalRegister(*rf.port(i), pendingSource.index());
                    move.setSource(newSource);
                }
            }

            if (move.destination().isGPR()) {

                const RegisterFile* regFile =
                    &move.destination().registerFile();
                string var = getVariableName(
                    regFile, move.destination().index());

                if (regBookkeeper_.isReserved(var)) {

                    TerminalRegister& reservedDst = regBookkeeper_.regForVar(var);
                    const RegisterFile& rf = reservedDst.registerFile();
                    int i = 0;
                    while (rf.port(i)->inputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newDst =
                        new TerminalRegister(*rf.port(i), reservedDst.index());
                    move.setDestination(newDst);

                } else {

                    TerminalRegister& pendingDst = *findFreeRegister(var);
                    regBookkeeper_.reserveRegister(pendingDst, var);
                    const RegisterFile& rf = pendingDst.registerFile();
                    int i = 0;
                    while (rf.port(i)->inputSocket() == NULL) {
                        i++;
                        assert(i < rf.portCount());
                    }
                    TerminalRegister* newDst =
                        new TerminalRegister(*rf.port(i), pendingDst.index());
                    move.setDestination(newDst);
                }

            }
        }
}

/**
 * Allocates registers for guards in moves of given instruction.
 */
void
SimpleRegisterAllocator::allocateGuards(Instruction& ins) {

    for (int j = 0; j < ins.moveCount(); j++) {
        Move& move = ins.move(j);

        if (!move.isUnconditional()) {
            const RegisterGuard* guard =
                dynamic_cast<RegisterGuard*>(&move.guard().guard());
            if (guard == NULL) {
                string msg = "Unexpected guard in move.";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
            }
            string var = getVariableName(guard->registerFile(), guard->registerIndex());
            TerminalRegister& guardReg = regForVar(var);

            // find one bus with matching guard and set it

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
    }
}

/**
 * Finds a non-allocated register.
 */
TerminalRegister*
SimpleRegisterAllocator::findFreeRegister(const std::string& var) const {

    if (registers_.empty()) {
        string msg = "Machine contains no registers!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    for (unsigned int i = 0; i < registers_.size(); i++) {
        TerminalRegister* reg = registers_.at(i);
        if (regBookkeeper_.isReserved(*reg)) {
            continue;
        } else if (var.at(0) == 'B'
                   && reg->registerFile().width() > 1) {
            continue;
        } else if (var.at(0) != 'B'
                   && reg->registerFile().width() <= 1) {
            continue;
        } else {
            return reg;
        }
    }

    string msg = "Not enough registers in machine! More than " +
        Conversion::toString(registers_.size()) +" required.";
    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
}

/**
 * Connects the given register with a variable name and returns it.
 */
std::string
SimpleRegisterAllocator::getVariableName(
    const TTAMachine::RegisterFile* regFile, int index) const {

    string var = "";

    if (regFile == &um_->integerRegisterFile()) {
        var.append("R");
    } else if (regFile == &um_->doubleRegisterFile()) {
        var.append("F");
    } else if (regFile == &um_->booleanRegisterFile()) {
        var.append("B");
    } else {
        string msg = "Unexpected register file in machine!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    var.append(Conversion::toString(index));
    return var;
}

/**
 * Returns the register reserved for the given variable.
 *
 * @param var The variable.
 * @return The register reserved for the given variable.
 */
TerminalRegister&
SimpleRegisterAllocator::regForVar(const std::string& var) const {
    return regBookkeeper_.regForVar(var);
}

/////////////////////////////////////////////////////////////////////////////
// RegisterBookkeeper
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
RegisterBookkeeper::RegisterBookkeeper() {
}

/**
 * Destructor.
 */
RegisterBookkeeper::~RegisterBookkeeper() {
}

/**
 * Allocates a register for the given variable name.
 *
 * @param var The variable to be connected with the register.
 * @param reg The register to be allocated for the variable.
 * @exception Exception if a register has already been reserved for the
 * variable name or the given register has already been reserved.
 */
void
RegisterBookkeeper::reserveRegister(TerminalRegister& reg, std::string var) {
    reservedRegisters_.insert(
        pair<TerminalRegister*, string>(&reg, var));
}

/**
 * Tells whether the given register has been reserved or not.
 *
 * @param reg The register that is checked for availability.
 * @return True if the register has been allocated, false otherwise.
 */
bool
RegisterBookkeeper::isReserved(TerminalRegister& reg) const {
    return MapTools::containsKey(reservedRegisters_, &reg);
}

/**
 * Tells whether a register has been assigned to the given variable.
 *
 * @param var The variable.
 * @return True if a register has been reserved for the variable, false
 *         otherwise.
 */
bool
RegisterBookkeeper::isReserved(std::string var) const {
    return MapTools::containsValue(reservedRegisters_, var);
}

/**
 * Returns the register reserved for the given variable.
 *
 * @param var The variable.
 * @return The register reserved for the given variable.
 */
TerminalRegister&
RegisterBookkeeper::regForVar(const std::string& var) const {
    return *MapTools::keyForValue<TerminalRegister*>(
        reservedRegisters_, var);
}

/**
 * Makes the given register available for reallocation.
 *
 * @param reg The register to deallocate.
 */
void
RegisterBookkeeper::freeRegister(TerminalRegister& reg) {
    reservedRegisters_.erase(&reg);
}

/**
 * Returns the number of reserved registers.
 *
 * @return The number of reserved registers.
 */
int
RegisterBookkeeper::numReservedRegisters() const {
    return reservedRegisters_.size();
}
