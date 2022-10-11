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
/**
 * @file MachineValidator.cc
 *
 * Implementation of MachineValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "FUValidator.hh"

#include "Machine.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"

#include "Application.hh"
#include "MathTools.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

const string CALL = "CALL";
const string JUMP = "JUMP";

/**
 * The constructor.
 *
 * @param machine The machine to validate.
 */
MachineValidator::MachineValidator(const TTAMachine::Machine& machine) :
    machine_(machine) {
}


/**
 * The destructor.
 */
MachineValidator::~MachineValidator() {
}


/**
 * Validates the machine.
 *
 * @param errorsToCheck Contains the error codes for the errors to check.
 * @return Results of the validation.
 */
MachineValidatorResults*
MachineValidator::validate(const std::set<ErrorCode>& errorsToCheck) const {

    MachineValidatorResults* results = new MachineValidatorResults();
    for (std::set<ErrorCode>::const_iterator iter = errorsToCheck.begin();
         iter != errorsToCheck.end(); iter++) {
        ErrorCode code = *iter;
        if (code == GCU_MISSING) {
            checkGCUExists(*results);
        } else if (code == GCU_AS_MISSING) {
            checkGCUHasAddressSpace(*results);
        } else if (code == USED_IO_NOT_BOUND) {
            checkOperandBindings(*results);
        } else if (code == DIFFERENT_PORT_FOR_JUMP_AND_CALL) {
            checkJumpAndCallOperandBindings(*results);
        } else if (code == PC_PORT_MISSING) {
            checkProgramCounterPort(*results);
        } else if (code == RA_PORT_MISSING) {
            checkReturnAddressPort(*results);
        } else if (code == PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH) {
            checkRAPortHasSameWidthAsPCPort(*results);
        } else if (code == IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC) {
            checkIMemAddrWidth(*results);
        } else if (code == FU_PORT_MISSING) {
            checkFUConnections(*results);
        } else {
            assert(false);
        }
    }

    return results;
}


/**
 * Checks that the machine has a control unit.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkGCUExists(MachineValidatorResults& results) const {
    if (machine_.controlUnit() == NULL) {
        string errorMsg = "The machine does not have a GCU.";
        results.addError(GCU_MISSING, errorMsg);
    }
}


/**
 * Checks that the GCU has an address space.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkGCUHasAddressSpace(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }
    if (gcu->addressSpace() == NULL) {
        string errorMsg = "The GCU does not have an address space.";
        results.addError(GCU_AS_MISSING, errorMsg);
    }
}


/**
 * Checks that all the used operands of operations are bound to some port in
 * function units.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkOperandBindings(
    MachineValidatorResults& results) const {

    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        FUValidator::checkOperandBindings(*fu, results);
    }

    // check GCU too
    ControlUnit* gcu = machine_.controlUnit();
    if (gcu != NULL) {
        FUValidator::checkOperandBindings(*gcu, results);
    }
}


/**
 * Checks that the operands of JUMP and CALL operations are bound to the
 * same port in GCU.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkJumpAndCallOperandBindings(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }

    if (gcu->hasOperation(CALL) && gcu->hasOperation(JUMP)) {
        HWOperation* callOp = gcu->operation(CALL);
        HWOperation* jumpOp = gcu->operation(JUMP);
        FUPort* pcPort1 = callOp->port(1);
        FUPort* pcPort2 = jumpOp->port(1);

        if (pcPort1 != NULL && pcPort2 != NULL && pcPort1 != pcPort2) {
            string errorMsg =
                "Operands of JUMP and CALL operations are not bound to the "
                "same port in GCU.";
            results.addError(DIFFERENT_PORT_FOR_JUMP_AND_CALL, errorMsg);
        }
    }
}


/**
 * Checks that the GCU has a PC port if it has JUMP or CALL operation.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkProgramCounterPort(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }

    if (gcu->hasOperation(CALL)) {
        HWOperation* callOp = gcu->operation(CALL);
        FUPort* pcPort = callOp->port(1);
        if (pcPort == NULL) {
            string errorMsg =
                "Operand 1 of CALL operation is not bound to any port.";
            results.addError(PC_PORT_MISSING, errorMsg);
        }
    }

    if (gcu->hasOperation(JUMP)) {
        HWOperation* jumpOp = gcu->operation(JUMP);
        FUPort* pcPort = jumpOp->port(1);
        if (pcPort == NULL) {
            string errorMsg =
                "Operand 1 of JUMP operation is not bound to any port.";
            results.addError(PC_PORT_MISSING, errorMsg);
        }
    }
}


/**
 * Checks that the GCU has return address port if it has CALL operation.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkReturnAddressPort(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }

    if (gcu->hasOperation(CALL)) {
        if (!gcu->hasReturnAddressPort()) {
            string errorMsg =
                "GCU does not have return address port which is needed by "
                "CALL operation.";
            results.addError(RA_PORT_MISSING, errorMsg);
        }
    }
}


/**
 * Checks that the return address port has the same width as program counter
 * port if both exists.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkRAPortHasSameWidthAsPCPort(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }
    if (!gcu->hasReturnAddressPort()) {
        return;
    }

    SpecialRegisterPort* raPort = gcu->returnAddressPort();
    if (gcu->hasOperation(CALL)) {
        FUPort* pcPort = gcu->operation(CALL)->port(1);
        if (pcPort != NULL && pcPort->width() != raPort->width()) {
            string errorMsg = "PC and RA ports have unequal width.";
            results.addError(PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH, errorMsg);
            return;
        }
    }
    if (gcu->hasOperation(JUMP)) {
        FUPort* pcPort = gcu->operation(JUMP)->port(1);
        if (pcPort != NULL && pcPort->width() != raPort->width()) {
            string errorMsg = "PC and RA ports have unequal width.";
            results.addError(PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH, errorMsg);
            return;
        }
    }
}


/**
 * Checks that the address width of instruction memory is the same as PC and
 * RA port width.
 *
 * @param results Results of the validation are added to the given instance.
 */
void
MachineValidator::checkIMemAddrWidth(
    MachineValidatorResults& results) const {

    ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        return;
    }

    AddressSpace* imem = gcu->addressSpace();
    if (imem == NULL) {
        return;
    }

    int addrWidth = MathTools::requiredBits(imem->end());

    if (gcu->hasReturnAddressPort()) {
        SpecialRegisterPort* raPort = gcu->returnAddressPort();
        if (raPort->width() != addrWidth) {
            string errorMsg =
                "Return address port has different width than "
                "instruction memory address.";
            results.addError(
                IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC, errorMsg);
        }
    } else {
        FUPort* pcPort = NULL;
        if (gcu->hasOperation(JUMP)) {
            HWOperation* jumpOp = gcu->operation(JUMP);
            pcPort = jumpOp->port(1);
        }
        if (pcPort == NULL && gcu->hasOperation(CALL)) {
            HWOperation* callOp = gcu->operation(CALL);
            pcPort = callOp->port(1);
        }

        if (pcPort != NULL && pcPort->width() != addrWidth) {
            string errorMsg =
                "Program counter port has different width than instruction "
                "memory address.";
            results.addError(
                IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC, errorMsg);
        }
    }
}

/**
 * Checks that the every FU has all the input and output ports that it needs.
 *
 *
 * @param results Results of the validation are added to the given instance.
 */

void
MachineValidator::checkFUConnections(MachineValidatorResults& results) const {
    const Machine::FunctionUnitNavigator fuNav =
        machine_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int p = 0; p < fu.operationPortCount(); p++) {
            FUPort& port = *fu.operationPort(p);
            if (port.socketCount() == 0) {
                string errorMsg = "The machine has an FU with missing ports.";
                results.addError(FU_PORT_MISSING, errorMsg);
            }
        }
    }
}
