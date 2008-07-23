/**
 * @file BasicMachineCheckSuite.cc
 *
 * Implementation of BasicMachineCheckSuite class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BasicMachineCheckSuite.hh"
#include "MachineCheck.hh"
#include "MachineCheckResults.hh"
#include "SequenceTools.hh"

#include "OperationBindingCheck.hh"
#include "RFPortCheck.hh"
#include "RegisterQuantityCheck.hh"

/**
 * The Constructor.
 */
BasicMachineCheckSuite::BasicMachineCheckSuite() : MachineCheckSuite() {    
    addCheck(new OperationBindingCheck);
    addCheck(new RFPortCheck);
    addCheck(new RegisterQuantityCheck);
}

/**
 * The Destructor.
 */
BasicMachineCheckSuite::~BasicMachineCheckSuite() {
}
