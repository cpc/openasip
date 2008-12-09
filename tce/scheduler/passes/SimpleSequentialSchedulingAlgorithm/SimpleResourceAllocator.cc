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
 * @file PrototypeAlgorithm.cc
 *
 * Implementation of PrototypeAlgorithm class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <vector>
#include <string>

#include "SimpleResourceAllocator.hh"
#include "CodeSnippet.hh"
#include "Program.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalFUPort.hh"
#include "TerminalRegister.hh"
#include "TerminalAddress.hh"
#include "TerminalInstructionAddress.hh"
#include "ControlUnit.hh"
#include "BaseFUPort.hh"
#include "SpecialRegisterPort.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "MathTools.hh"
#include "AssocTools.hh"
#include "NullFunctionUnit.hh"
#include "Segment.hh"
#include "Guard.hh"
#include "MoveGuard.hh"

using std::vector;
using std::string;
using namespace TTAMachine;
using namespace TTAProgram;


/////////////////////////////////////////////////////////////////////////////
// SimpleResourceAllocator
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
SimpleResourceAllocator::SimpleResourceAllocator(): resMan_(NULL) {
}


/**
 * Destructor.
 */
SimpleResourceAllocator::~SimpleResourceAllocator() {
    delete resMan_;
}


/**
 * MISSING DOCUMENTATION.
 */
void
SimpleResourceAllocator::start()
    throw (Exception) {

    if (program_ == NULL || target_ == NULL) {
        string method = "SimpleResourceAllocator::start()";
        string msg =
            "Source program and/or target architecture not defined!";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, msg);
    }
    resMan_ = new SimpleResourceManager(*target_);
    findReservedRegisters();
    assignResources(*program_, *target_);
}


/**
 * MISSING DOCUMENTATION.
 */
bool
SimpleResourceAllocator::needsTarget() const {
    return true;
}


/**
 * MISSING DOCUMENTATION.
 */
bool
SimpleResourceAllocator::needsProgram() const {
    return true;
}


/**
 * MISSING DOCUMENTATION.
 */
void
SimpleResourceAllocator::findReservedRegisters() {

    Procedure* proc = &program_->firstProcedure();
    while (proc != &NullProcedure::instance()) {
        Instruction* ins = &proc->firstInstruction();
        while (ins != & NullInstruction::instance()) {
            for (int i = 0; i < ins->moveCount(); i++) {
                Move& move = ins->move(i);
                if (move.source().isGPR()) {
                    Terminal& srcCopy(*move.source().copy());
                    resMan_->reserveRegister(
                        dynamic_cast<TerminalRegister&>(srcCopy));
                }
                if (move.destination().isGPR()) {
                    Terminal& dstCopy(*move.destination().copy());
                    resMan_->reserveRegister(
                        dynamic_cast<TerminalRegister&>(dstCopy));
                }
            }
            ins = &proc->nextInstruction(*ins);
        }
        proc = &program_->nextProcedure(*proc);
    }
}


/**
 * Assigns resources on the given program.
 *
 * @param prog
 * @param mach
 */
void
SimpleResourceAllocator::assignResources(Program& prog, const Machine& mach) {

    cout << "Assigning resources... ";
    cout.flush();

    try {

        vector<Move*> opMoves;
        Operation* currentOp = NULL;
        Operation* lastOp = NULL;
        const FunctionUnit* lastUnit = NULL;

        Procedure* proc = &prog.firstProcedure();

        while (proc != &NullProcedure::instance()) {

            Instruction* ins = &proc->firstInstruction();

            while (ins != & NullInstruction::instance()) {

                for (int i = 0; i < ins->moveCount(); i++) {

                    Move& move = ins->move(i);

                    if (move.source().isFUPort()) {

                        if (dynamic_cast<const SpecialRegisterPort*>(
                                &move.source().port()) != NULL) {

                            // Assign proper gcu port for ra.
                            // In the future should check for other types
                            // of special registers

                            ControlUnit* gcu = mach.controlUnit();
                            TerminalFUPort* newSource = new
                                TerminalFUPort(*gcu->returnAddressPort());
                            move.setSource(newSource);

                        } else {
                            // result move
                            TerminalFUPort* newSource =
                                new TerminalFUPort(
                                    *lastUnit->operation(
                                        lastOp->name()), 
                                    lastOp->numberOfInputs() + 1);
                            move.setSource(newSource);
                            assert(lastOp->numberOfOutputs() < 2);
                        }
                        
                    } else if (move.source().isGPR()) {
                        // Move from register.
                        // Set a read port.

                        const RegisterFile& regFile =
                            move.source().registerFile();

                        int i = 0;
                        while (regFile.port(i)->outputSocket() == NULL) {
                            i++;
                        }

                        const Port& port = *regFile.port(i);
                        
                        TerminalRegister* newSource =
                            new TerminalRegister(
                                port, move.source().index());
                        move.setSource(newSource);

                    }
                    // else source is immediate
                    if (move.source().isAddress()) {
                        // set address space
                        Machine::AddressSpaceNavigator asNav =
                            mach.addressSpaceNavigator();
                        for (int i = 0; i < asNav.count(); i++) {
                            AddressSpace& as = *asNav.item(i);
                            Address addr = move.source().address();
                            if (&as != mach.controlUnit()->addressSpace() &&
                                as.width() == addr.space().width() &&
                                as.start() <= addr.location() &&
                                as.end() > addr.location()) {
                                TerminalAddress* newSource =
                                    new TerminalAddress(
                                        move.source().value(), as);
                                move.setSource(newSource);
                            }
                        }
                        
                        // set correct jump target
                    } else if (move.source().isInstructionAddress()) {
                        // warning: we are forced to extract the address from
                        // the immediate value in this funny way because class
                        // TerminalInstructionAddress is not a TerminalAddress
                        // at the moment - this could change
                        int instrAddr = move.source().value().intWordValue();
                        Instruction& target = prog.instructionAt(instrAddr);
                        TerminalInstructionAddress* newSource =
                            new TerminalInstructionAddress(
                                prog.instructionReferenceManager().
                                createReference(target));
                        move.setSource(newSource);
                    }

                    if (move.destination().isFUPort()) {

                        const BaseFUPort& dst = 
                            dynamic_cast<const BaseFUPort&>(
                                move.destination().port());

                        if (dst.isTriggering()) {

                            // trigger move, should be opcode setting
                            assert(dst.isOpcodeSetting());

                            currentOp = &move.destination().operation();

                            const FunctionUnit& unit =
                                resMan_->reserveFunctionUnit(*currentOp);

                            // assign fu for each move in current operation

                            HWOperation* hwOp =
                                unit.operation(currentOp->name());

                            for (unsigned int i = 0; i < opMoves.size(); i++) {
                                TerminalFUPort* newDst =
                                    new TerminalFUPort(*hwOp, i + 1);
                                opMoves.at(i)->setDestination(newDst);
                            }

                            TerminalFUPort* newDst =
                                new TerminalFUPort(
                                    *hwOp, currentOp->numberOfInputs());
                            move.setDestination(newDst);

                            opMoves.clear();
                            lastUnit = &unit;
                            lastOp = currentOp;

                        } else if (dynamic_cast<const SpecialRegisterPort*>(
                                       &dst) != NULL) {

                            TerminalFUPort* newDst =
                                new TerminalFUPort(
                                    *mach.controlUnit()->returnAddressPort());
                            move.setDestination(newDst);

                        } else {
                            // operand move is added to the list of moves in
                            // current operation
                            opMoves.push_back(&move);
                        }
                        
                    } else {
                        // move to a register
                        const RegisterFile& regFile =
                            move.destination().registerFile();
                        int i = 0;
                        while (regFile.port(i)->inputSocket() == NULL) {
                            i++;
                        }
                        const Port& port = *regFile.port(i);
                        TerminalRegister* newDst =
                            new TerminalRegister(
                                port, move.destination().index());
                        move.setDestination(newDst);
                    }
                }
                ins = &proc->nextInstruction(*ins);
            }

            proc = &prog.nextProcedure(*proc);
        }

        // assign buses for each move in each instruction, and insert nops 
        // to fill latency and jump delay slots

        Instruction* ins = &prog.firstInstruction();

        while (ins != &NullInstruction::instance()) {

            for (int i = 0; i < ins->moveCount(); i++) {
                Move& move = ins->move(i);

                if (move.destination().isFUPort() &&
                    dynamic_cast<const BaseFUPort&>(
                        move.destination().port()).isTriggering()) {

                    Operation* currentOp = &move.destination().operation();
                    int numNops = 0;

                    // TODO: convert to currentOp->isControlFlowOperation()
                    if (currentOp->name() == "JUMP" || 
                        currentOp->name() == "CALL") {
                        numNops = mach.controlUnit()->delaySlots();
                    } else {
                        HWOperation* hwOp = 
                            move.destination().functionUnit().operation(
                                currentOp->name());
                        numNops = hwOp->latency() - 1;
                    }

                    for (int i = 0; i < numNops; i++) {
                        ins->parent().insertInstructionAfter(
                            *ins, new Instruction());
                    }
                }
            }

            resMan_->assignBuses(*ins);
            ins = &prog.nextInstruction(*ins);
        }
        
    } catch (const Exception& e) {
        throw ModuleRunTimeError(
            e.fileName(), e.lineNum(), e.procedureName(), e.errorMessage());
    }

    cout << "done." << endl;
    cout.flush();
}

using std::set;


/////////////////////////////////////////////////////////////////////////////
// SimpleResourceManager
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param mach
 */
SimpleResourceManager::SimpleResourceManager(const TTAMachine::Machine& mach):
    mach_(mach) {
}


/**
 * Destructor.
 */
SimpleResourceManager::~SimpleResourceManager() {
}


/**
 * MISSING DOCUMENTATION.
 *
 * @param reg
 */
void
SimpleResourceManager::reserveRegister(TTAProgram::TerminalRegister& reg) {

    if (!reg.isGPR()) {
        string msg = "Wrong terminal type while reserving register.";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    for (vector<TerminalRegister*>::iterator regIter = reservedRegs_.begin();
         regIter != reservedRegs_.end(); regIter++) {
        if (&(*regIter)->registerFile() == &reg.registerFile() && 
            (*regIter)->index() == reg.index()) {
            return;
        }
    }

    reservedRegs_.push_back(&reg);
}


/**
 * MISSING DOCUMENTATION.
 *
 * @param op
 */
const FunctionUnit&
SimpleResourceManager::reserveFunctionUnit(Operation& op) {

    Machine::FunctionUnitNavigator fuNav = mach_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        if (fuNav.item(i)->hasOperation(op.name())) {
            return *fuNav.item(i);
        }
    }

    if (mach_.controlUnit()->hasOperation(op.name())) {
        return *mach_.controlUnit();
    } else {
        string msg =
            "No FU supporting operation '" + op.name() + "' found!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    return NullFunctionUnit::instance();
}


/**
 * MISSING DOCUMENTATION.
 *
 * @param move
 * @param parent
 */
Bus&
SimpleResourceManager::findBus(Move& move, Instruction& parent) {

    // find buses that are connected to move destination, and if move is
    // guarded, the bus must have appropriate guard

    set<Bus*> dstBuses;
    const Port* dstPort = &move.destination().port();
    Socket* socket = dstPort->inputSocket();

    if (socket == NULL) {
	string unit = dstPort->parentUnit()->name();
	string port = dstPort->name();
	    string msg =
                "Tried to perform a move to '" + unit + "." + port +
            "' which has no connections to an input socket. "
            "Check operation bindings!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    
    for (int i = 0; i < socket->segmentCount(); i++) {

        Bus* aBus = socket->segment(i)->parentBus();

        if (!move.isUnconditional()) {

            const Guard& guard = move.guard().guard();
            assert(dynamic_cast<const RegisterGuard*>(&guard) != NULL);

            for (int i = 0; i < aBus->guardCount(); i++) {

                Guard* busGuard = aBus->guard(i);
                if (busGuard->isEqual(guard)) {
                    dstBuses.insert(aBus);
                    break;
                }
            }

        } else {
            dstBuses.insert(aBus);
        }
    }

    // now look for a bus in the set of busses that can write to destination
    // which supports also the source of the move and return it if found

    if (move.source().isImmediate()) {
        
        // find a bus with appropriate immediate width that is connected to
        // destination

        int bits = MathTools::requiredBits(
            move.source().value().intWordValue());

        for (set<Bus*>::iterator busIter = dstBuses.begin();
             busIter != dstBuses.end();
             busIter++) {
            if ((*busIter)->immediateWidth() >= bits) {
                return **busIter;
            }
        }

        string unit = "";
        if (move.destination().isGPR()) {
            unit=move.destination().registerFile().name();
        } else {
            unit=move.destination().functionUnit().name();
        }
        string port = move.destination().port().name();

        throw InstanceNotFound(
            __FILE__, __LINE__, "SimpleResourceManager::findBus",
            "No suitable bus found for imm width " +
            Conversion::toString(bits) + " to transport to "
            + unit + "." + port + "!");
        
    } else {

        // find a bus that is connected to both source and destination

        set<Bus*> srcBuses;
        const Port* srcPort = &move.source().port();
        Socket* oSocket = srcPort->outputSocket();

        if (oSocket == NULL) {
            string unit = srcPort->parentUnit()->name();
            string port = srcPort->name();
            string msg =
                "Tried to perform a move from '" + unit + "." + port +
                "' which has no connections to an output socket! Check "
                "operation bindings!";
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }

        for (int i = 0; i < oSocket->segmentCount(); i++) {
            srcBuses.insert(oSocket->segment(i)->parentBus());
        }

        set<Bus*> possibleBuses;
        AssocTools::intersection(dstBuses, srcBuses, possibleBuses);

        // if no buses connected to both source and destination are found,
        // perform a move to a temporary register
        
        if (possibleBuses.empty()) {

            // find an RF with an input port connected to the source and an
            // output port connected to the destination
            SimpleResourceManager::TempRegister tempReg =
                findTempRegConnectedTo(srcBuses, dstBuses);
            
            if (tempReg.inPort == NULL) {
                string srcUnitName = srcPort->parentUnit()->name();
                string srcPortName = srcPort->name();
                string dstUnitName = dstPort->parentUnit()->name();
                string dstPortName = dstPort->name();
                throw InstanceNotFound(
                    __FILE__, __LINE__, "SimpleResourceManager::findBus()",
                    "No connection between move source and destination (" +
                    srcUnitName + "." + srcPortName + "->" + dstUnitName +
                    "." + dstPortName + ") found "
                    "or not enough registers in machine to perform "
                    "temporary move.");
            }

            Procedure& parentProc = parent.parent();
            Instruction* newIns = new Instruction();
            
            TerminalRegister* tempDst =
                new TerminalRegister(*tempReg.inPort, tempReg.index);
            TerminalRegister* tempSrc =
                new TerminalRegister(*tempReg.outPort, tempReg.index);

            Move* newMove = new Move(
                tempSrc, move.destination().copy(), **dstBuses.begin());
            newIns->addMove(newMove);

            parentProc.insertInstructionAfter(parent, newIns);

            move.setDestination(tempDst);
            return findBus(move, parent);

        } else {
            return **possibleBuses.begin();
        }
    }
}


/**
 * MISSING DOCUMENTATION.
 *
 * @param ins
 */
void
SimpleResourceManager::assignBuses(Instruction& ins) {

    try {

        for (int i = 0; i < ins.moveCount(); i++) {

            Move& move = ins.move(i);
            Bus& bus = findBus(move, ins);
            
            if (!move.isUnconditional()) {

                const Guard& guard = move.guard().guard();
                bool guardFound = false;
                for (int i = 0; i < bus.guardCount(); i++) {
                    Guard* busGuard = bus.guard(i);
                    if (busGuard->isEqual(guard)) {
                        move.setGuard(new MoveGuard(*busGuard));
                        guardFound = true;
                        break;
                    }
                }
                assert(guardFound == true);
            }
            move.setBus(bus);
        }

    } catch (const Exception e) {
        throw ModuleRunTimeError(
            e.fileName(), e.lineNum(), e.procedureName(), e.errorMessage());
    }
}


/**
 * MISSING DOCUMENTATION.
 *
 * @note This method needs serious redesign. It is unnaceptably inefficient:
 * each invocation may require an average number of tests ranging from few
 * thousands to a million.
 *
 * @param rf
 */
int
SimpleResourceManager::findFreeIndex(RegisterFile& rf) {

    // cerr << "Checking for free reg in RF: " << rf.name() << endl;
    for (int index = 0; index < rf.numberOfRegisters(); index++) {
        // cerr << index << " ";
        bool reserved = false;
        
        for (vector<TerminalRegister*>::iterator regIter = 
                 reservedRegs_.begin();
             regIter != reservedRegs_.end(); regIter++) {

            TerminalRegister* reg = *regIter;
            if (&reg->registerFile() == &rf &&
                reg->index() == static_cast<unsigned int>(index)) {
                
                reserved = true;
                // cerr << " reserved" << endl;
                break;
            }
        }
        
        if (!reserved) {
            // cerr << " free!" << endl;
            return index;
        }
    }
    // cerr << "No free reg found!" << endl;
    return -1;
}


/**
 * MISSING DOCUMENTATION.
 *
 * @param srcBuses
 * @param dstBuses
 */
SimpleResourceManager::TempRegister
SimpleResourceManager::findTempRegConnectedTo(
    std::set<TTAMachine::Bus*>& srcBuses,
    std::set<TTAMachine::Bus*>& dstBuses) {

    // cerr << "Looking for temporary register... " << endl;
    // cerr << "Total reserved registers: " << reservedRegs_.size() << endl;

    set<Port*> srcPorts;
    TempRegister tempReg;

    // find all possible input ports in rf's that are connected to the same bus
    // as the original source

    Machine::RegisterFileNavigator regNav = mach_.registerFileNavigator();
    for (int i = 0; i < regNav.count(); i++) {
        for (int j = 0; j < regNav.item(i)->portCount(); j++) {
            Socket* iSocket = regNav.item(i)->port(j)->inputSocket();
            if (iSocket != NULL) {
                for (set<Bus*>::iterator iter = srcBuses.begin();
                     iter != srcBuses.end();
                     iter++) {
                    if (iSocket->isConnectedTo(**iter)) {
                        srcPorts.insert(regNav.item(i)->port(j));
                        break;
                    }
                }
            }
        }
    }

    // cerr << "Possible input ports: " << srcPorts.size() << endl;
    // cerr << "Checking whether a connected output port and a free register can be found... " << endl;

    // find an output port of the same RF of the input port which is also
    // connected to the original destination

    for (set<Port*>::iterator portIter = srcPorts.begin();
        portIter != srcPorts.end();
        portIter++) {

        Port* srcPort = *portIter;
        RegisterFile* rf = dynamic_cast<RegisterFile*>(srcPort->parentUnit());

        for (int i = 0; i < rf->portCount(); i++) {

            Socket* oSocket = rf->port(i)->outputSocket();
            if (oSocket == NULL) {
                continue;
            }

            for (set<Bus*>::iterator busIter = dstBuses.begin();
                 busIter != dstBuses.end();
                 busIter++) {

                if (oSocket->isConnectedTo(**busIter)) {
                    int freeIndex = findFreeIndex(*rf);
                    if (freeIndex >= 0) {
                        tempReg.inPort = srcPort;
                        tempReg.outPort = rf->port(i);
                        tempReg.index = freeIndex;
                        // cerr << "Free temporary register found!" << endl;
                        return tempReg;
                    }
                }
            }
        }
    }
    tempReg.inPort = NULL;
    tempReg.outPort = NULL;
    tempReg.index = -1;
    // cerr << "No free temporary register found!" << endl;
    return tempReg;
}

SCHEDULER_PASS(SimpleResourceAllocator)
