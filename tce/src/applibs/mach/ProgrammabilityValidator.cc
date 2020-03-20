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
/*
 * @file ProgrammabilityValidator.cc
 *
 * Implementation of ProgrammabilityValidator class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "Application.hh"
#include "SetTools.hh"
#include "ContainerTools.hh"
#include "ProgrammabilityValidator.hh"
#include "ProgrammabilityValidatorResults.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "Segment.hh"
#include "Bus.hh"
#include "Unit.hh"
#include "HWOperation.hh"
#include "Instruction.hh"
#include "Program.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "FUPort.hh"
#include "Move.hh"
#include "Procedure.hh"
#include "ProgramWriter.hh"
#include "MoveGuard.hh"
#include "Binary.hh"
#include "BinaryEncoding.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "MinimalOpSetCheck.hh"
#include "Guard.hh"

using std::set;
using std::vector;
using std::string;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;
using namespace TTAMachine;
using namespace TTAProgram;


/**
 * The constructor.
 *
 * @param machine The machine that will be profiled.
 * @param procedure ProgrammabilityValidator generates procedure.
 */
ProgrammabilityValidator::ProgrammabilityValidator(const Machine& machine)
    : machine_(machine), minimalOpSetCheck_(new MinimalOpSetCheck()) {
    directCounter = 0;
    gcrCounter = 0;
    booleanRegister_ = NULL;
    MachineValidator machineValidator(machine);
    set<MachineValidator::ErrorCode> errorsToCheck;
    errorsToCheck.insert(MachineValidator::GCU_MISSING);
    errorsToCheck.insert(MachineValidator::GCU_AS_MISSING);

    MachineValidatorResults* results = 
        machineValidator.validate(errorsToCheck);

    for (int i = 0; i < results->errorCount(); i++) {
        MachineValidator::ErrorCode code = results->error(i).first;
        if (code == MachineValidator::GCU_MISSING) {
            string msg = "ADF file doesn't have a GCU.";
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
       }
        if (code == MachineValidator::GCU_AS_MISSING) {
            string msg = "No address space bound to GCU.";
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
        }
    }
    delete results;

    AddressSpace* space = machine_.controlUnit()->addressSpace();
    program_ = new Program(*space);
    procedure_ = new Procedure("programmabilityValidator", *space);
    program_->addProcedure(procedure_);
    
    FUConnections.clear();
    GCUConnections.clear();
    RFConnections.clear();
    IMMConnections.clear();
}

/**
 * The destructor.
 */
ProgrammabilityValidator::~ProgrammabilityValidator() {

    FUConnections.clear();
    GCUConnections.clear();
    RFConnections.clear();
    IMMConnections.clear();
    delete minimalOpSetCheck_;
    minimalOpSetCheck_ = NULL;
}



/**
 * Does all validations of ProgrammabilityValidator.
 *
 * @return Returns pointer to ProgrammabilityValidatorResults where all error
 * messages are stored.
 */
ProgrammabilityValidatorResults* 
ProgrammabilityValidator::validate() {

    ProgrammabilityValidatorResults* results = 
        new ProgrammabilityValidatorResults();

    checkPrimitiveSet(*results);
    checkBooleanRegister(*results);
    findConnections();
    findGlobalConnectionRegister();
    
    if (globalConnectionRegister_ == NULL) {
        results->addError(GLOBAL_CONNECTION_REGISTER_NOT_FOUND,
                          "Can't determine global connection register.");
    }
    
    findGlobalRegisterConnections();

    vector<pair<string, string> > missingInputs;
    vector<pair<string, string> > missingOutputs;
    searchMissingRFConnections(
        globalConnectionRegister_,
        missingInputs,
        missingOutputs);

    if ((missingInputs.size() > 0) || (missingOutputs.size() > 0)) {
        for (unsigned int i = 0; i < missingInputs.size(); i++) {
            string missing = "";
            missing.append("Missing connection: ");
            missing.append(missingInputs[i].first);
            missing.append(" : ");
            missing.append(missingInputs[i].second);
            missing.append(" -> ");
            missing.append(globalConnectionRegister_->name());
            results->addError(MISSING_CONNECTION, missing);
        }
        for (unsigned int i = 0; i < missingOutputs.size(); i++) {
            string missing = "";
            missing.append("Missing connection: ");
            missing.append(globalConnectionRegister_->name());
            missing.append(" -> ");
            missing.append(missingOutputs[i].first);
            missing.append(" : ");
            missing.append(missingOutputs[i].second);
            results->addError(MISSING_CONNECTION, missing);
        }
    }
    return results;
}


/**
 * Does selected validations of ProgrammabilityValidator.
 *
 * @param errorsToCheck Contains the error codes for the errors to check.
 * @return Returns pointer to ProgrammabilityValidatorResults where all error
 * messages are stored.
 */
ProgrammabilityValidatorResults* 
ProgrammabilityValidator::validate(const std::set<ErrorCode>& errorsToCheck) {

    ProgrammabilityValidatorResults* results = 
        new ProgrammabilityValidatorResults();

    for (std::set<ErrorCode>::const_iterator iter = errorsToCheck.begin();
         iter != errorsToCheck.end(); iter++) {
        ErrorCode code = *iter;
        if (code == OPERATION_MISSING_FROM_THE_PRIMITIVE_OPERATION_SET) {
            checkPrimitiveSet(*results);
        } else if (code == BOOLEAN_REGISTER_ERROR ||
                   code == BOOLEAN_REGISTER_GUARD_MISSING) {
            checkBooleanRegister(*results);
        } else if (code == GLOBAL_CONNECTION_REGISTER_NOT_FOUND) {
            findConnections();
            findGlobalConnectionRegister();
            if (globalConnectionRegister_ == NULL) {
                results->addError(GLOBAL_CONNECTION_REGISTER_NOT_FOUND,
                                 "Can't determine global connection register.");
            }
        } else if (code == MISSING_CONNECTION) {
            findConnections();
            findGlobalConnectionRegister();
            if (globalConnectionRegister_ == NULL) {
                results->addError(GLOBAL_CONNECTION_REGISTER_NOT_FOUND,
                                  "Can't determine global connection register.");
                break;
            }
            vector<pair<string, string> > missingInputs;
            vector<pair<string, string> > missingOutputs;
            searchMissingRFConnections(
                globalConnectionRegister_,
                missingInputs,
                missingOutputs);

            if ((missingInputs.size() > 0) || (missingOutputs.size() > 0)) {
                for (unsigned int i = 0; i < missingInputs.size(); i++) {
                    string missing = "";
                    missing.append("Missing connection: ");
                    missing.append(missingInputs[i].first);
                    missing.append(" : ");
                    missing.append(missingInputs[i].second);
                    missing.append(" -> ");
                    missing.append(globalConnectionRegister_->name());
                    results->addError(MISSING_CONNECTION, missing);
                }
                for (unsigned int i = 0; i < missingOutputs.size(); i++) {
                    string missing = "";
                    missing.append("Missing connection: ");
                    missing.append(globalConnectionRegister_->name());
                    missing.append(" -> ");
                    missing.append(missingOutputs[i].first);
                    missing.append(" : ");
                    missing.append(missingOutputs[i].second);
                    results->addError(MISSING_CONNECTION, missing);
                }
            }
        } else {
            assert(false);
        }
    }
    return results;
}


/**
 * Does all the steps needed to check maintaining the programmability.
 *
 * @param results Possible error messages are added here.
 * @return Returns the binary.
 * @exception NotAvailable Thrown if binary creation is failed.
 * @exception InvalidData Thrown if the binary can't be created.
 */
TPEF::Binary*
ProgrammabilityValidator::profile(ProgrammabilityValidatorResults& results) {
    checkPrimitiveSet(results);
    
    if (!checkBooleanRegister(results)) {
        // error found in boolean register check.
        string msg = "Error found in boolean register check.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    findConnections();
    findGlobalConnectionRegister();
    
    if (globalConnectionRegister_ == NULL) {
        results.addError(GLOBAL_CONNECTION_REGISTER_NOT_FOUND,
                         "Can't determine global connection register.");
        string msg = "Global connection register can't be determined.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    
    findGlobalRegisterConnections();

    vector<pair<string, string> > missingInputs;
    vector<pair<string, string> > missingOutputs;
    searchMissingRFConnections(
        globalConnectionRegister_,
        missingInputs,
        missingOutputs);


    if ((missingInputs.size() > 0) || (missingOutputs.size() > 0)) {
        for (unsigned int i = 0; i < missingInputs.size(); i++) {
            string missing = "";
            missing.append("Missing connection: ");
            missing.append(missingInputs[i].first);
            missing.append(" : ");
            missing.append(missingInputs[i].second);
            missing.append(" -> ");
            missing.append(globalConnectionRegister_->name());
            results.addError(MISSING_CONNECTION, missing);
        }
        for (unsigned int i = 0; i < missingOutputs.size(); i++) {
            string missing = "";
            missing.append("Missing connection: ");
            missing.append(globalConnectionRegister_->name());
            missing.append(" -> ");
            missing.append(missingOutputs[i].first);
            missing.append(" : ");
            missing.append(missingOutputs[i].second);
            results.addError(MISSING_CONNECTION, missing);
        }
    }
    ProgramWriter progWriter(*program_);
    try {
        TPEF::Binary* binaryProgram = progWriter.createBinary();
        return binaryProgram;
    } catch (NotAvailable& e) {
        throw e;
    }
}

/**
 * Checks the machine which primitive operations are missing.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
ProgrammabilityValidator::checkPrimitiveSet(
    ProgrammabilityValidatorResults& results) const {
    
    std::vector<std::string> missingOps;
    minimalOpSetCheck_->missingOperations(machine_, missingOps);

    string msg = "Operation missing from the minimal operation set: ";
    std::vector<std::string>::iterator it = missingOps.begin();
    std::vector<std::string>::iterator end = missingOps.end();
    while (it != end) {
        results.addError(
                OPERATION_MISSING_FROM_THE_PRIMITIVE_OPERATION_SET,
                msg.append(*it++));
    }
}


/**
 * Check the machine for a boolean register file. Only one is allowed.
 * Boolean register should have size and width of one.
 *
 * @param results Results of the validation are added to the given instance.
 * @return Returns true if check is passed. False is returned if error 
 *         is found.
 */
bool
ProgrammabilityValidator::checkBooleanRegister(
    ProgrammabilityValidatorResults& results) {

    bool returnValue = true;
    const Machine::RegisterFileNavigator& nav =
        machine_.registerFileNavigator();
    bool found = false;
    for (int i = 0; i < nav.count(); i++) {
        RegisterFile* rf = nav.item(i);
        if (rf->width() == 1) {
            if (!found) {
                found = true;
                booleanRegister_ = rf;
            } else {
                results.addError(BOOLEAN_REGISTER_ERROR,
                                 "Multiple boolean registers.");
                returnValue = false;
                break;
            }
        }
    }

    if (!found) {
        results.addError(BOOLEAN_REGISTER_ERROR,
                         "No boolean register found.");
        returnValue = false;
    }
    
    string buses = "";
    const Machine::BusNavigator& busNav =
	machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
	Bus* bus = busNav.item(i);
        for (int n = 0; n < bus->guardCount(); n++) {
            RegisterGuard* registerGuard = 
                dynamic_cast<RegisterGuard*>(bus->guard(n));
            if (registerGuard != NULL) {
                if (registerGuard->parentBus() == bus && 
                    registerGuard->registerFile() == booleanRegister_ &&
                    registerGuard->registerIndex() == 0) {
                    continue;
                } else {
                    string message = 
                        "Missing boolean register guard on bus: ";
                    results.addError(BOOLEAN_REGISTER_GUARD_MISSING,
                                     message.append(bus->name()));
                    returnValue = false;
                    break;
                }
            }
        }
        if (bus->guardCount() == 0) {
            string message = 
                "Missing boolean register guard on bus: ";
            results.addError(BOOLEAN_REGISTER_GUARD_MISSING,
                             message.append(bus->name()));
            returnValue = false;
        }
    }
    return returnValue;
}


/**
 * Prints one connection.
 *
 * @param sourcePort The source port of the connection.
 * @param destPort The destination port of the connection.
 * @param segment The segment which is used in the connection.
 * @param counter Updates the amount of printed connections.
 */
void
ProgrammabilityValidator::printConnection(
    const Port* sourcePort,
    const Port* destPort,
    Segment* segment,
    int& counter) {

    const Unit* sourceUnit = sourcePort->parentUnit();
    const Unit* destUnit = destPort->parentUnit();
    const BaseRegisterFile* destRF = 
        dynamic_cast<const BaseRegisterFile*>(destUnit);
    const BaseRegisterFile* sourceRF = 
        dynamic_cast<const BaseRegisterFile*>(sourceUnit);
    const FunctionUnit* destFU =
        dynamic_cast<const FunctionUnit*>(destUnit);
    const FunctionUnit* sourceFU = 
        dynamic_cast<const FunctionUnit*>(sourceUnit);
    const BaseFUPort* destBaseFUPort = 
        dynamic_cast<const BaseFUPort*>(destPort);
    const BaseFUPort* sourceBaseFUPort = 
        dynamic_cast<const BaseFUPort*>(sourcePort);
    

    if (sourceFU != 0) {
        // move is from FU or GCU
        if (sourceBaseFUPort->isTriggering()) {
            // move is from trigger port
            // this is not supported
            return;
        }
        // else
        // move is from FUPort
        
        if (destFU != 0) {
            // move is to FU or GCU

            if (destBaseFUPort->isTriggering()) {
                // move is to FU of CGU trigger port
                
                for (int n = 0; n < destFU->operationCount(); n++) {

                    HWOperation* destOperation = destFU->operation(n);
                    counter++;
                    std::cout << sourceUnit->name() << " : "
			      << sourcePort->outputSocket()->name()
			      << " -> "
			      << destPort->inputSocket()->name() << " ("
                              << destOperation->name()
			      << ")  ("
			      << segment->parentBus()->name() << ","
			      << segment->name() << ") " 
			      << std::endl;
                }

            }
            else {
                // move is to FUPort or Special register port
                // move is to FUPort
                counter++;
                std::cout << sourceUnit->name() << " : "
                          << sourcePort->outputSocket()->name()
                          << " -> "
                          << destPort->inputSocket()->name() << "  ("
                          << segment->parentBus()->name() << ","
                          << segment->name() << ") " 
                          << std::endl;
            }
            
        }
        else { //if (destRF != 0)
            // move is to RF
            for (int i = 0; i < destRF->numberOfRegisters(); i++) {
                counter++;
                std::cout << sourceUnit->name() << " : "
                          << sourcePort->outputSocket()->name()
                          << " -> "
                          << destPort->inputSocket()->name() << "("
                          << i
                          << ")  ("
                          << segment->parentBus()->name() << ","
                          << segment->name() << ") " 
                          << std::endl;
            }
        }
    }
    else if (sourceRF != 0) {
        // move is from RF or IMM
        for (int i = 0; i < sourceRF->numberOfRegisters(); i++) {
            if (destFU != 0) {
                // move is to FU or GCU
                if (destBaseFUPort->isTriggering()) {
                    // move is to FU of CGU trigger port
                    
                    for (int n = 0; n < destFU->operationCount(); n++) {
                        HWOperation* destOperation = destFU->operation(n);
                        counter++;
                        std::cout << sourceUnit->name() << " : "
                                  << sourcePort->outputSocket()->name() 
                                  << "("
                                  << i << ")"
                                  << " -> "
                                  << destPort->inputSocket()->name() << "("
                                  << destOperation->name()
                                  << ")  ("
                                  << segment->parentBus()->name() << ","
                                  << segment->name() << ") " 
                                  << std::endl;
                    }                
                }
                else {
                    // move is to FUPort or Special register port
                    counter++;
                    std::cout << sourceUnit->name() << " : "
                              << sourcePort->outputSocket()->name() << "("
                              << i << ")"
                              << " -> "
                              << destPort->inputSocket()->name() << "  ("
                              << segment->parentBus()->name() << ","
                              << segment->name() << ") " 
                              << std::endl;
                }
            }
            else { //if (destRF != 0)
                // move is to RF
                for (int j = 0; j < destRF->numberOfRegisters(); j++) {
                    counter++;
                    std::cout << sourceUnit->name() << " : "
                              << sourcePort->outputSocket()->name() << "("
                              << i << ")"
                              << " -> "
                              << destPort->inputSocket()->name() << "("
                              << j << ")  ("
                              << segment->parentBus()->name() << ","
                              << segment->name() << ") " 
                              << std::endl;
                }
            }
        }
    }
}


/**
 * Finds the guard of the boolean register file.
 *
 * @param segment The segment where the guard is searched from. 
 * @param guard The found guard is put here.
 */
void
ProgrammabilityValidator::findRegisterGuard(
    Segment* segment, RegisterGuard*& guard) {
    
    Bus* bus = segment->parentBus();
    for (int i = 0; i < bus->guardCount(); i++) {
        RegisterGuard* registerGuard = 
            dynamic_cast<RegisterGuard*>(bus->guard(i));
        if (registerGuard != NULL) {
            if (registerGuard->parentBus() == bus && 
                registerGuard->registerFile() == booleanRegister_ &&
                registerGuard->registerIndex() == 0) {
                guard = registerGuard;
            }
        }
    }
}


/**
 * Adds connection to program. Generates instruction that 
 * is given to procedure.
 * 
 * @param sourcePort The source port of the connection.
 * @param destPort The destination port of the connection.
 * @param segment The segment which is used in the connection.
 * @param counter Counter that is updated when an instruction is added.
 */
void
ProgrammabilityValidator::addConnectionToProgram(
    const Port* sourcePort,
    const Port* destPort,
    Segment* segment,
    int& counter) {

    const Unit* sourceUnit = sourcePort->parentUnit();
    const Unit* destUnit = destPort->parentUnit();
    const BaseRegisterFile* destRF = 
        dynamic_cast<const BaseRegisterFile*>(destUnit);
    const BaseRegisterFile* sourceRF = 
        dynamic_cast<const BaseRegisterFile*>(sourceUnit);
    const FunctionUnit* destFU = dynamic_cast<const FunctionUnit*>(destUnit);
    const FunctionUnit* sourceFU = 
        dynamic_cast<const FunctionUnit*>(sourceUnit);
    const BaseFUPort* destBaseFUPort = 
        dynamic_cast<const BaseFUPort*>(destPort);
    const BaseFUPort* sourceBaseFUPort = 
        dynamic_cast<const BaseFUPort*>(sourcePort);
    
    if (sourceFU != 0) {
        // move is from FU or GCU
        if (sourceBaseFUPort->isTriggering()) {
            // move is from trigger port
            // this is not supported
            return;
        }
        // else
        // move is from FUPort
        
        if (destFU != 0) {
            // move is to FU or GCU
            const FUPort* destFUPort = 
                dynamic_cast<const FUPort*>(destBaseFUPort);

            if (destBaseFUPort->isTriggering()) {
                // move is to FU of CGU trigger port
                
                for (int n = 0; n < destFU->operationCount(); n++) {
                    counter++;
                    HWOperation* destOperation = destFU->operation(n);
                    Instruction* instruction = 
                        new Instruction(NullInstructionTemplate::instance());
                    int destOperand = destOperation->io(*destFUPort);
                    Terminal* source = 
                        new TerminalFUPort(*sourceBaseFUPort);
                    Terminal* destination = 
                        new TerminalFUPort(*destOperation, destOperand);
                    RegisterGuard* guard;
                    findRegisterGuard(segment, guard);
                    MoveGuard* moveGuard = new MoveGuard(*guard);
                    auto movePtr = std::make_shared<Move>(source,
                                          destination,
                                          *segment->parentBus(),
                                          moveGuard);
                    instruction->addMove(movePtr);
                    procedure_->add(instruction);
                }

            }
            else {
                // move is to FUPort or Special register port
                // move is to FUPort
                counter++;
                Instruction* instruction = 
                    new Instruction(NullInstructionTemplate::instance());
                Terminal* source = 
                    new TerminalFUPort(*sourceBaseFUPort);
                Terminal* destination = 
                    new TerminalFUPort(*destBaseFUPort);
                RegisterGuard* guard;
                findRegisterGuard(segment, guard);
                MoveGuard* moveGuard = new MoveGuard(*guard);
                auto movePtr = std::make_shared<Move>(source,
                                      destination,
                                      *segment->parentBus(),
                                      moveGuard);
                instruction->addMove(movePtr);
                procedure_->add(instruction);
            }
        }
        else { //if (destRF != 0)
            // move is to RF
            for (int i = 0; i < destRF->numberOfRegisters(); i++) {
                Instruction* instruction
                    = new Instruction(NullInstructionTemplate::instance());
                Terminal* source = new TerminalFUPort(*sourceBaseFUPort);
                Terminal* destination = new TerminalRegister(*destPort, i);
                RegisterGuard* guard;
                findRegisterGuard(segment, guard);
                MoveGuard* moveGuard = new MoveGuard(*guard);
                auto movePtr = std::make_shared<Move>(source,
                                      destination,
                                      *segment->parentBus(),
                                      moveGuard);
                instruction->addMove(movePtr);
                procedure_->add(instruction);
                counter++;
            }
        }
    }
    else if (sourceRF != 0) {
        // move is from RF or IMM
        for (int i = 0; i < sourceRF->numberOfRegisters(); i++) {
            if (destFU != 0) {
                // move is to FU or GCU
                if (destBaseFUPort->isTriggering()) {
                    // move is to FU of CGU trigger port
                    const FUPort* destFUPort = 
                        dynamic_cast<const FUPort*>(destBaseFUPort);
                    for (int n = 0; n < destFU->operationCount(); n++) {
                        
                        counter++;
                        HWOperation* destOperation = destFU->operation(n);
                        Instruction* instruction =
                            new Instruction(
                                NullInstructionTemplate::instance());
                        int destOperand = destOperation->io(*destFUPort);
                        Terminal* source = 
                            new TerminalRegister(*sourcePort, i);
                        Terminal* destination = 
                            new TerminalFUPort(*destOperation, destOperand);
                        RegisterGuard* guard;
                        findRegisterGuard(segment, guard);
                        MoveGuard* moveGuard = new MoveGuard(*guard);
                        auto movePtr = std::make_shared<Move>(source,
                                              destination,
                                              *segment->parentBus(),
                                              moveGuard);
                        instruction->addMove(movePtr);
                        procedure_->add(instruction);
                    }                
                }
                else {
                    // move is to FUPort or Special register port
                    
                    counter++;
                    Instruction* instruction =
                        new Instruction(NullInstructionTemplate::instance());
                    Terminal* source = 
                        new TerminalRegister(*sourcePort, i);
                    Terminal* destination = 
                        new TerminalFUPort(*destBaseFUPort);
                    RegisterGuard* guard;
                    findRegisterGuard(segment, guard);
                    MoveGuard* moveGuard = new MoveGuard(*guard);
                    auto movePtr = std::make_shared<Move>(source,
                                          destination,
                                          *segment->parentBus(),
                                          moveGuard);
                    instruction->addMove(movePtr);
                    procedure_->add(instruction);
                }
            }
            else { //if (destRF != 0)
                // move is to RF
                for (int j = 0; j < destRF->numberOfRegisters(); j++) {
                    counter++;
                    Instruction* instruction =
                        new Instruction(NullInstructionTemplate::instance());
                    Terminal* source = 
                        new TerminalRegister(*sourcePort, i);
                    Terminal* destination = 
                        new TerminalRegister(*destPort, j);
                    RegisterGuard* guard;
                    findRegisterGuard(segment, guard);
                    MoveGuard* moveGuard = new MoveGuard(*guard);
                    auto movePtr = std::make_shared<Move>(source,
                                          destination,
                                          *segment->parentBus(),
                                          moveGuard);
                    instruction->addMove(movePtr);
                    procedure_->add(instruction);
                }
            }
        }
    }
}


/**
 * Gererates a set of segments where the socket is connected.
 *
 * @param connections Segments where the socket is connected
 * is collected here.
 * @param socket Socket which connections are searched.
 */
void
ProgrammabilityValidator::listConnections(
    set<Segment*>& connections,
    const Socket* socket) {
    
    const Machine::BusNavigator& busNav =
	machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
	Bus* bus = busNav.item(i);
	if (socket->isConnectedTo(*bus)) {
	    for (int j = 0; j < bus->segmentCount(); j++) {
		Segment* segment = bus->segment(j);
		if (socket->isConnectedTo(*segment)) {
		    // we have found the bus and the segment where
                    // the socket is connected
		    connections.insert(segment);
		}
	    }
	}
    }
}


/**
 * Search and lists all the missing connections to the register file.
 *
 * @param rf The register file which connections are handled.
 * @param toConnections Missing inputs to the register file.
 * @param fromConnections Missing outputs from the register file.
 */
void
ProgrammabilityValidator::searchMissingRFConnections(
    const RegisterFile* rf,
    vector<pair<string, string> >& toConnections,
    vector<pair<string, string> >& fromConnections) {

    if (booleanRegister_ == NULL) {
        ProgrammabilityValidatorResults results;
        checkBooleanRegister(results);
    }

    map<const Socket*, Connections> rfConnections = 
        RFConnections[rf->name()];

    for (mapIter i = FUConnections.begin(); i != FUConnections.end(); i++) {
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {
            if ((*n).first->direction() == Socket::OUTPUT) {

                // one connection to register file is enough
                set<Segment*> outputIntersection;
                for (map<const Socket*, 
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs, 
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
                if (outputIntersection.size() == 0) {
                    toConnections.push_back(
                        pair<string, string>(
                            (*i).first,
                            (*n).first->name()));
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                set<Segment*> inputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs,
                            inputIntersection);
                        SetTools::intersection(
                            (*n).second.triggeredInputs, 
                            (*m).second.outputs,
                            inputIntersection);
                    }
                }
                if (inputIntersection.size() == 0) {
                    fromConnections.push_back(
                        pair<string, string>((*i).first,
                                             (*n).first->name()));
                }
            }
        }
    }
    
    for (mapIter i = GCUConnections.begin();
         i != GCUConnections.end(); i++) {

        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {
            if ((*n).first->direction() == Socket::OUTPUT) {

                // one connection to register file is enough
                set<Segment*> outputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
                if (outputIntersection.size() == 0) {
                    toConnections.push_back(
                        pair<string, string>(
                            (*i).first, (*n).first->name()));
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                set<Segment*> inputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs,
                            inputIntersection);
                        SetTools::intersection(
                            (*n).second.triggeredInputs,
                            (*m).second.outputs,
                            inputIntersection);
                    }
                }
                if (inputIntersection.size() == 0) {
                    fromConnections.push_back(
                        pair<string, string>(
                            (*i).first, (*n).first->name()));
                }
            }
        }
    }

    for (mapIter i = IMMConnections.begin();
         i != IMMConnections.end(); i++) {

        set<Segment*> outputIntersection;
        set<Segment*> inputIntersection;
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {
            if ((*n).first->direction() == Socket::OUTPUT) {
                
                // one connection to register file is enough
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
            }
        }
        if (outputIntersection.size() == 0) {
            toConnections.push_back(pair<string, string>((*i).first, "-"));
        }
    }


    for (mapIter i = RFConnections.begin(); i != RFConnections.end(); i++) {
        set<Segment*> outputIntersection;
        set<Segment*> inputIntersection;
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {

            if ((*n).first->direction() == Socket::OUTPUT) {
                
                // one connection to register file is enough
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {
                    
                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs,
                            inputIntersection);
                    }
                }
            }
        }
        if (outputIntersection.size() == 0) {
            toConnections.push_back(pair<string, string>((*i).first, "-"));
        }
        if (inputIntersection.size() == 0) {

            // Connection to the boolean register file is not needed
            if (booleanRegister_ != NULL && 
                (*i).first != booleanRegister_->name()) {
                fromConnections.push_back(
                    pair<string, string>((*i).first, "-"));
            }
        }
    }
}


/**
 * Prints the missing connections to the register file.
 *
 * @param rf The register file which connections are handled.
 */
void
ProgrammabilityValidator::printNotConnectedSockets(const RegisterFile* rf) {

    map<const Socket*, Connections> rfConnections =
        RFConnections[rf->name()];

    for (mapIter i = FUConnections.begin(); i != FUConnections.end(); i++) {
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {

            if ((*n).first->direction() == Socket::OUTPUT) {

                // one connection to register file is enough
                set<Segment*> outputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
                if (outputIntersection.size() == 0) {
                    cout << "Missing connection: " << (*i).first
                         << ": " << (*n).first->name()
                         << " -> " << rf->name() << ": " << endl;
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                set<Segment*> inputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs,
                            inputIntersection);
                        SetTools::intersection(
                            (*n).second.triggeredInputs,
                            (*m).second.outputs,
                            inputIntersection);
                    }
                }
                if (inputIntersection.size() == 0) {
                    cout << "Missing connection: " << rf->name() 
                         << " -> " << (*i).first 
                         << ": " << (*n).first->name() << endl;
                }
            }
        }
    }

    for (mapIter i = GCUConnections.begin();
         i != GCUConnections.end(); i++) {

        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {

            if ((*n).first->direction() == Socket::OUTPUT) {

                // one connection to register file is enough
                set<Segment*> outputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
                if (outputIntersection.size() == 0) {
                    cout << "Missing connection: " << (*i).first
                         << ": " << (*n).first->name()
                         << " -> " << rf->name() << endl;          
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                set<Segment*> inputIntersection;
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs, 
                            inputIntersection);
                        SetTools::intersection(
                            (*n).second.triggeredInputs, 
                            (*m).second.outputs,
                            inputIntersection);
                    }
                }
                if (inputIntersection.size() == 0) {
                    cout << "Missing connection: " << rf->name()
                         << " -> " << (*i).first 
                         << ": " << (*n).first->name() << endl;
                }
            }
        }
    }

    for (mapIter i = IMMConnections.begin();
         i != IMMConnections.end(); i++) {

        set<Segment*> outputIntersection;
        set<Segment*> inputIntersection;
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {

            if ((*n).first->direction() == Socket::OUTPUT) {
                
                // one connection to register file is enough
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
            }
        }
        if (outputIntersection.size() == 0) {
            cout << "Missing connection: " << (*i).first 
                 << " -> " << rf->name() << endl;                
        }
    }

    for (mapIter i = RFConnections.begin(); i != RFConnections.end(); i++) {
        set<Segment*> outputIntersection;
        set<Segment*> inputIntersection;
        for (map<const Socket*,
                 Connections>::iterator n = (*i).second.begin();
             n != (*i).second.end(); n++) {
            
            if ((*n).first->direction() == Socket::OUTPUT) {
                
                // one connection to register file is enough
                for (map<const Socket*, 
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::INPUT) {
                        SetTools::intersection(
                            (*n).second.outputs,
                            (*m).second.inputs,
                            outputIntersection);
                    }
                }
            } else if ((*n).first->direction() == Socket::INPUT) {
                for (map<const Socket*,
                         Connections>::iterator m = rfConnections.begin();
                     m != rfConnections.end(); m++) {

                    if ((*m).first->direction() == Socket::OUTPUT) {
                        SetTools::intersection(
                            (*n).second.inputs,
                            (*m).second.outputs, 
                            inputIntersection);
                    }
                }
            }
        }
        if (outputIntersection.size() == 0) {
            cout << "Missing connection: " << (*i).first 
                 << " -> " << rf->name() << endl;                
        }
        if (inputIntersection.size() == 0) {
            cout << "Missing connection: " << rf->name() 
                 << " -> " << (*i).first << endl;
        }
    }
}


/**
 * Adds new connections to all connections holding set.
 *
 * @param newConnections New connections that can be added.
 * @param connections All connections holding set.
 */
void 
ProgrammabilityValidator::addConnections(
    set<Segment*>& newConnections,
    set<Segment*>& connections) {
    
    set<Segment*>::iterator i = newConnections.begin();
    for (; i != newConnections.end(); i++) {
        connections.insert(*i);
    }
}


/**
 * Adds operations to connections.
 *
 * @param fu The function unit that is the parentunit for the operations.
 * @param opertations Set of FWOperations where new operations are added.
 */
void
ProgrammabilityValidator::addOperationConnections(
    const FunctionUnit* fu,
    set<HWOperation*>& operations) {

    for (int i = 0; i < fu->operationCount(); i++) {
        operations.insert(fu->operation(i));
    }
}


/**
 * Finds out the connections to the register file and stores 
 * those in the procedure.
 */
void
ProgrammabilityValidator::findGlobalRegisterConnections() {
    
    vector<ConnectionSet>::iterator connectionIter = directConnections.begin();
    for (; connectionIter != directConnections.end(); connectionIter++) {
        if ((*connectionIter).sourcePort->parentUnit() == 
            globalConnectionRegister_ ||
            (*connectionIter).destPort->parentUnit() == 
            globalConnectionRegister_) {
            
            addGlobalRegisterConnection(
                (*connectionIter).sourcePort,
                (*connectionIter).destPort,
                (*connectionIter).segment);
        }
    }
    addGlobalRegisterConnectionsToProgram();
}


/**
 * Adds global connection register connection to list of GCR connections.
 * Moves can be genetated from these connections.
 * 
 * @param sourcePort Source port of the move.
 * @param destPort Destionation port of the move.
 * @param segment The segment of the move.
 */
void
ProgrammabilityValidator::addGlobalRegisterConnection(
    const Port* sourcePort,
    const Port* destPort,
    Segment* segment) {

    pair<const Port*, const Port*>  connectionPair = 
        pair<const Port*, const Port*>(sourcePort, destPort);
    pair<portSetIter, bool> result = gcrConnections.insert(connectionPair);
    // If the insertion was made the connection
    // can be added and it is unique.
    if (result.second) {
        
        // Find out if the source or destination unit is Register File.
        // Only one connection per port to RF and from RF to port is needed.
        const Unit* destUnit = destPort->parentUnit();
        const RegisterFile* destRegister =
            dynamic_cast<const RegisterFile*>(destUnit);
        const Unit* sourceUnit = sourcePort->parentUnit();
        const RegisterFile* sourceRegister =
            dynamic_cast<const RegisterFile*>(sourceUnit);

        if (destRegister != NULL) {
            if (sourceRegister != NULL) {
                pair<const RegisterFile*,
                    const RegisterFile*> registerRegisterPair =
                    pair<const RegisterFile*, const RegisterFile*>(
                        sourceRegister, destRegister);
                pair<registerRegisterSetIter, bool> registerRegisterResult = 
                    gcrRegisterConnections.insert(registerRegisterPair);
                if (registerRegisterResult.second) {
                    ConnectionSet newSet;
                    newSet.sourcePort = sourcePort;
                    newSet.destPort = destPort;
                    newSet.segment = segment;
                    globalRegisterConnections.push_back(newSet);
                }
            } else {
                // destination is a register but source is not
                pair<const Port*, const RegisterFile*> portRegisterPair =
                    pair<const Port*, const RegisterFile*>(
                        sourcePort, destRegister);
                pair<portRegisterSetIter, bool> portRegisterResult = 
                    portGcrConnections.insert(portRegisterPair);
                if (portRegisterResult.second) {
                    ConnectionSet newSet;
                    newSet.sourcePort = sourcePort;
                    newSet.destPort = destPort;
                    newSet.segment = segment;
                    globalRegisterConnections.push_back(newSet);
                }
            }
        } else if (sourceRegister != NULL) {
            pair<const RegisterFile*, const Port*> registerPortPair =
                pair<const RegisterFile*, const Port*>(
                    sourceRegister, destPort);
            pair<registerPortSetIter, bool> registerPortResult = 
                gcrPortConnections.insert(registerPortPair);
            if (registerPortResult.second) {
                ConnectionSet newSet;
                newSet.sourcePort = sourcePort;
                newSet.destPort = destPort;
                newSet.segment = segment;
                globalRegisterConnections.push_back(newSet);
            }
        } else {
            // source or destination should be register
            assert(false);
        }
    }
}


/**
 * Prints connections from and to the global connection register.
 */
void
ProgrammabilityValidator::printRegisterConnections() {

    int counter = 0;
    for (unsigned int i = 0; i < globalRegisterConnections.size(); i++) {
        ConnectionSet setToBePrinted = globalRegisterConnections[i];
        printConnection(setToBePrinted.sourcePort,
                        setToBePrinted.destPort,
                        setToBePrinted.segment,
                        counter);
    }
    cout << "----------------------------------------" << endl;
    cout << "Total:     " << counter << " connections" << endl;
}


/**
 * Finds out all the connections in the machine.
 *
 * @param printConnections Flag for printing of connections.
 */
void
ProgrammabilityValidator::findConnections() {

    const Machine::FunctionUnitNavigator& fuNav =
	machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
	const FunctionUnit* fu = fuNav.item(i);
        map<const Socket*, Connections> fuConnections;

        // for every port we need to check which kind of socket 
        // the port is connected to 
	for (int j = 0; j < fu->portCount(); j++) {
	    const Port* port = fu->port(j);
            const BaseFUPort* baseFUPort = 
                dynamic_cast<const BaseFUPort*>(port);
	    const Socket* socket = port->inputSocket();
	    
	    // if not null, socket is an input socket
	    if (socket != NULL) {

                Connections connections;
                set<Segment*> fuInputs;
                
                // connections to the input socket are listed to fuInputs
                listConnections(fuInputs, socket);                
                
                if (baseFUPort->isTriggering()) {
                    addConnections(fuInputs, connections.triggeredInputs);
                    addOperationConnections(fu, connections.operations);
                } else {
                    addConnections(fuInputs, connections.inputs);
                }
                fuConnections[socket] = connections;
                inputs.push_back(pair<const Port*, set<Segment*> >(
                                     port, fuInputs));
            }
        }
        FUConnections[fu->name()] = fuConnections;
    }
    const Machine::RegisterFileNavigator& rfNav =
	machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        
	const RegisterFile* rf = rfNav.item(i);
        map<const Socket*, Connections> rfConnections;
	
	for (int j = 0; j < rf->portCount(); j++) {
	    const Port* port = rf->port(j);
	    const Socket* socket = port->inputSocket();
	    
	    // if not null, socket is an input socket
	    if (socket != NULL) {

                Connections connections;
		set<Segment*> rfInputs;
                listConnections(rfInputs, socket);
                addConnections(rfInputs, connections.inputs);
                connections.registers = rf->numberOfRegisters();
                rfConnections[socket] = connections;
                inputs.push_back(
		    pair<const Port*,
                    set<Segment*> >(port, rfInputs));
	    }
	}
	RFConnections[rf->name()] = rfConnections;
	
    }
    
    const ControlUnit* gcu = machine_.controlUnit();
    if (gcu != NULL) {
	map<const Socket*, Connections> gcuConnections;
	for (int j = 0; j < gcu->portCount(); j++) {
	    const Port* port = gcu->port(j);
            const BaseFUPort* baseFUPort = 
                dynamic_cast<const BaseFUPort*>(port);
	    const Socket* socket = port->inputSocket();
            
	    // if not null, socket is an input socket
	    if (socket != NULL) {
                Connections connections;
                set<Segment*> gcuInputs;
                listConnections(gcuInputs, socket);
                if (baseFUPort->isTriggering()) {
                    addConnections(gcuInputs, connections.triggeredInputs);
                    addOperationConnections(gcu, connections.operations);
                } else {
                    addConnections(gcuInputs, connections.inputs);
                }
                inputs.push_back(
                    pair<const Port*,
                    set<Segment*> >(port, gcuInputs));
                connections.registers = 0;
                gcuConnections[socket] = connections;
            }
        }
        GCUConnections[gcu->name()] = gcuConnections;
    }
    
    
    // now we look for output sockets

    for (int i = 0; i < fuNav.count(); i++) {
	const FunctionUnit* fu = fuNav.item(i);
	set<const Socket*> outputs;
	for (int j = 0; j < fu->portCount(); j++) {
	    const Port* port = fu->port(j);
	    const Socket* socket = port->outputSocket();
	    
	    // if not null, socket is an output socket
	    if (socket != NULL) {
		
		set<Segment*> outputConnections;
		listConnections(outputConnections, socket);
		// go through every input
		for (unsigned int k = 0; k < inputs.size(); k++) {
		    set<Segment*> intersection;
                    
                    // do an intersection between 
                    // inputs and socket connections
		    SetTools::intersection(inputs[k].second,
                                             outputConnections,
                                             intersection);

                    // go through all the connections of 
                    // the intersection result
                    map<const Socket*, Connections> socketConnections = 
                        FUConnections[fu->name()];
                    Connections connections = socketConnections[socket];

                    set<Segment*>::iterator iter = 
                        intersection.begin();
                    for (; iter != intersection.end(); iter++) {
                        
                        if (!ContainerTools::containsValue(
                                connections.outputs,
                                *iter)) {
                            connections.outputs.insert(*iter);
                        }
                        addDirectConnection(port, inputs[k].first, *iter);
		    }
                    socketConnections[socket] = connections;
                    FUConnections[fu->name()] = socketConnections;
		}
	    }
	}
    }
    for (int i = 0; i < rfNav.count(); i++) {
	const TTAMachine::RegisterFile* rf = rfNav.item(i);
	set<const Socket*> outputs;
	for (int j = 0; j < rf->portCount(); j++) {
	    const Port* port = rf->port(j);
	    const Socket* socket = port->outputSocket();

	    // if not null, socket is an output socket
	    if (socket != NULL) {
		set<Segment*> outputConnections;
		listConnections(outputConnections, socket);
                
		for (unsigned int k = 0; k < inputs.size(); k++) {
		    set<Segment*> intersection;
                    
                    
		    SetTools::intersection(inputs[k].second,
                                             outputConnections,
                                             intersection);
                    map<const Socket*, Connections> socketConnections =
                        RFConnections[rf->name()];
                    Connections connections = socketConnections[socket];

                    set<Segment*>::iterator iter = 
                        intersection.begin();
                    for (; iter != intersection.end(); iter++) {
                        
                        if (!ContainerTools::containsValue(
                                connections.outputs,
                                *iter)) {
                            connections.outputs.insert(*iter);
                        }
                        addDirectConnection(port, inputs[k].first, *iter);
		    }
                    socketConnections[socket] = connections;
                    RFConnections[rf->name()] = socketConnections;
                }
	    }
	}
    }
    
    const Machine::ImmediateUnitNavigator& immNav =
	machine_.immediateUnitNavigator();
    for (int i = 0; i < immNav.count(); i++) {
	const TTAMachine::ImmediateUnit* imm = immNav.item(i);
	
	set<const Socket*> outputs;
	for (int j = 0; j < imm->portCount(); j++) {
	    const Port* port = imm->port(j);
	    const Socket* socket = port->outputSocket();
	    
	    // if not null, socket is an output socket
	    if (socket != NULL) {
		set<Segment*> outputConnections;
		listConnections(outputConnections, socket);
		for (unsigned int k = 0; k < inputs.size(); k++) {
		    set<Segment*> intersection;
		    SetTools::intersection(inputs[k].second,
                                             outputConnections,
                                             intersection);
                    map<const Socket*, Connections> socketConnections =
                        IMMConnections[imm->name()];
                    Connections connections = socketConnections[socket];

                    set<Segment*>::iterator iter = 
                        intersection.begin();
                    for (; iter != intersection.end(); iter++) {
                        
                        if (!ContainerTools::containsValue(
                                connections.outputs,
                                *iter)) {

                            connections.outputs.insert(*iter);
                        }
                        addDirectConnection(port, inputs[k].first, *iter);
		    }
                    connections.registers = imm->numberOfRegisters();
                    socketConnections[socket] = connections;
                    IMMConnections[imm->name()] = socketConnections;
		}
	    }
	}
    }
    if (gcu != NULL) {
	set<const Socket*> outputs;
	for (int j = 0; j < gcu->portCount(); j++) {
	    const Port* port = gcu->port(j);
	    const Socket* socket = port->outputSocket();
	    
	    // if not null, socket is an output socket
	    if (socket != NULL) {
		set<Segment*> outputConnections;
		listConnections(outputConnections, socket);
		for (unsigned int k = 0; k < inputs.size(); k++) {
		    set<Segment*> intersection;
		    SetTools::intersection(inputs[k].second,
                                             outputConnections,
                                             intersection);
                    map<const Socket*, Connections> socketConnections = 
                        GCUConnections[gcu->name()];
                    Connections connections = socketConnections[socket];

                    set<Segment*>::iterator iter = 
                        intersection.begin();
                    for (; iter != intersection.end(); iter++) {

                        if (!ContainerTools::containsValue(
                                connections.outputs,
                                *iter)) {
                            connections.outputs.insert(*iter);
                        }
                        addDirectConnection(port, inputs[k].first, *iter);
		    }
                    socketConnections[socket] = connections;
                    GCUConnections[gcu->name()] = socketConnections;
		}
	    }
	}
    }
}


/**
 * Adds all connections found in the machine to program.
 */
void
ProgrammabilityValidator::addDirectConnectionsToProgram() {

    // add all collected source - destination - segment sets to program
    for (unsigned int i = 0; i < directConnections.size(); i++) {
        ConnectionSet setToBeAdded = directConnections[i];
        addConnectionToProgram(setToBeAdded.sourcePort,
                               setToBeAdded.destPort,
                               setToBeAdded.segment,
                               directCounter);
    }
}


/**
 * Adds global connection registers connections to program.
 */
void
ProgrammabilityValidator::addGlobalRegisterConnectionsToProgram() {

    // add all collected source - destination - segment sets to program
    for (unsigned int i = 0; i < globalRegisterConnections.size(); i++) {
        ConnectionSet setToBeAdded = globalRegisterConnections[i];
        addConnectionToProgram(setToBeAdded.sourcePort,
                               setToBeAdded.destPort,
                               setToBeAdded.segment,
                               gcrCounter);
    }
}


/**
 * Adds direct connection (unit to unit) to list of direct connections.
 * Moves can be genetated from these connections.
 * 
 * @param sourcePort Source port of the move.
 * @param destPort Destionation port of the move.
 * @param segment The segment of the move.
 */
void
ProgrammabilityValidator::addDirectConnection(
    const Port* sourcePort,
    const Port* destPort,
    Segment* segment) {

    pair<const Port*, const Port*>  connectionPair = 
        pair<const Port*, const Port*>(sourcePort, destPort);
    pair<portSetIter, bool> result = allConnections.insert(connectionPair);
    // If the insertion was made the connection
    // can be added and it is unique.
    if (result.second) {
        
        // Find out if the source or destination unit is Register File.
        // Only one connection per port to RF and from RF to port is needed.
        const Unit* destUnit = destPort->parentUnit();
        const RegisterFile* destRegister =
            dynamic_cast<const RegisterFile*>(destUnit);
        const Unit* sourceUnit = sourcePort->parentUnit();
        const RegisterFile* sourceRegister =
            dynamic_cast<const RegisterFile*>(sourceUnit);

        if (destRegister != NULL) {
            if (sourceRegister != NULL) {
                pair<const RegisterFile*,
                    const RegisterFile*> registerRegisterPair =
                    pair<const RegisterFile*, const RegisterFile*>(
                        sourceRegister, destRegister);
                pair<registerRegisterSetIter, bool> registerRegisterResult = 
                    registerRegisterConnections.insert(registerRegisterPair);
                if (registerRegisterResult.second) {
                    ConnectionSet newSet;
                    newSet.sourcePort = sourcePort;
                    newSet.destPort = destPort;
                    newSet.segment = segment;
                    directConnections.push_back(newSet);
                }
            } else {
                // destination is a register but source is not
                pair<const Port*, const RegisterFile*> portRegisterPair =
                    pair<const Port*, const RegisterFile*>(
                        sourcePort, destRegister);
                pair<portRegisterSetIter, bool> portRegisterResult = 
                    portRegisterConnections.insert(portRegisterPair);
                if (portRegisterResult.second) {
                    ConnectionSet newSet;
                    newSet.sourcePort = sourcePort;
                    newSet.destPort = destPort;
                    newSet.segment = segment;
                    directConnections.push_back(newSet);
                }
            }
        } else if (sourceRegister != NULL) {
            pair<const RegisterFile*, const Port*> registerPortPair =
                pair<const RegisterFile*, const Port*>(
                    sourceRegister, destPort);
            pair<registerPortSetIter, bool> registerPortResult = 
                registerPortConnections.insert(registerPortPair);
            if (registerPortResult.second) {
                ConnectionSet newSet;
                newSet.sourcePort = sourcePort;
                newSet.destPort = destPort;
                newSet.segment = segment;
                directConnections.push_back(newSet);
            }
        } else {
            ConnectionSet newSet;
            newSet.sourcePort = sourcePort;
            newSet.destPort = destPort;
            newSet.segment = segment;
            directConnections.push_back(newSet);
        }
    }
}


/**
 * Prints all connections found in the machine.
 */
void
ProgrammabilityValidator::printConnections() {

    int counter = 0;
    for (unsigned int i = 0; i < directConnections.size(); i++) {
        ConnectionSet setToBePrinted = directConnections[i];
        printConnection(setToBePrinted.sourcePort,
                        setToBePrinted.destPort,
                        setToBePrinted.segment,
                        counter);
    }
    cout << "---------------------------" << endl;
    cout << "Total: " << counter << " connections" << endl;
}


/**
 * Finds the global connection register out of the machine.
 *
 * The global connection register is register that has the best connecticity.
 *
 * @return Pointer to the global connection register.
 */
const RegisterFile*
ProgrammabilityValidator::findGlobalConnectionRegister() {

    const RegisterFile* gcRegister = 0;

    int maxConnectivity = 0;
    int maxInputs = 0;
    int maxOutputs = 0;
    
    const Machine::RegisterFileNavigator& rfNav =
	machine_.registerFileNavigator();
    const Machine::BusNavigator& busNav =
	machine_.busNavigator();

    int maxWidth = 0;

    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        int width = bus->width();

        // find out the biggest bus width of the machine
        if (width > maxWidth) {
            maxWidth = width;
        }
    }
    
    for (int i = 0; i < rfNav.count(); i++) {
	const RegisterFile* rf = rfNav.item(i);
        
        if (rf->width() >= maxWidth) {
            map<const Socket*, Connections> rfConns = 
                RFConnections[rf->name()];
            set<Segment*> intersection;
            
            int inputs = 0;
            int outputs = 0;
            for (map<const Socket*,
                     Connections>::iterator n = rfConns.begin();
                 n != rfConns.end(); n++) {
                
                inputs += (*n).second.inputs.size();
                outputs += (*n).second.outputs.size();
            }
            int sum = inputs + outputs;
            int connectivity = 0;
            if (inputs > outputs) {
                connectivity = sum - (inputs - outputs);
            } else {
                connectivity = sum - (outputs - inputs);
                }
            if (connectivity > maxConnectivity) {
                gcRegister = rf;
                maxConnectivity = connectivity;
                maxInputs = inputs;
                maxOutputs = outputs;
            } else if (connectivity == maxConnectivity) {
                if (sum > (maxInputs + maxOutputs)) {
                    gcRegister = rf;
                    maxConnectivity = connectivity;
                    maxInputs = inputs;
                    maxOutputs = outputs;
                }
            }
        }
    }
    globalConnectionRegister_ = const_cast<RegisterFile*>(gcRegister);
    return gcRegister;
}
