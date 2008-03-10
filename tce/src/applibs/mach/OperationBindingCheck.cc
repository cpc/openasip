/** 
 * @file OperationBindingCheck.cc
 *
 * Implementation of OperationBindingCheck class.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OperationBindingCheck.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"


using namespace TTAMachine;

OperationBindingCheck::OperationBindingCheck() : 
    MachineCheck("Check operation binding, port directions, triggers") {}

/**
 * Checks that machine's FU's have legal port and trigger bindings
 *
 * @param mach machine to check
 * @param results place where to put detailed error descriptions
 * @return true if everything is ok, false if something is wrong
 */
bool OperationBindingCheck::check(
    const TTAMachine::Machine& mach, MachineCheckResults& results) const {

    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        mach.functionUnitNavigator();

    bool retval = true;
    bool triggerok = true;
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        
        // find trigger port 
        FUPort* triggerPort = NULL;
        for (int j = 0; j < fu.operationPortCount(); j++) {
            FUPort* port = fu.operationPort(j);
            if (port->isTriggering()) {

                // check that we do not have multiple trigger ports.
                if ( triggerPort != NULL) {
                    results.addError(
                        *this, (boost::format("Multiple trigger ports are "
                                              "not supported, Used in FU: %s")
                                % fu.name()).str());
                    retval = false;
                    triggerok = false;
                }
                else {
                    // check trigger and fu ports are the same
                    triggerPort = port;
                    if (!port->isOpcodeSetting()) {
                        results.addError(
                            *this, (boost::format("Opcode setting and trigger "
                                                  "port differ in FU: ") 
                                    % fu.name()).str());
                        retval = false;
                        triggerok = false;
                    }
                }
            }
        }
        // check that we have a trigger port
        if ( triggerPort == NULL) {
            results.addError(
                *this, (boost::format("Trigger port not found in FU: %s")
                        % fu.name()).str());
            return false;
        }

        // check all operations
        for (int j = 0; j < fu.operationCount(); j++) {
            
            HWOperation& hwop = *fu.operation(j);
            bool triggerRead = false;

            // check read port bindins
            ExecutionPipeline& pipe = *hwop.pipeline();
            const ExecutionPipeline::OperandSet reads = pipe.readOperands();
            for (ExecutionPipeline::OperandSet::const_iterator iter =
                     reads.begin(); iter != reads.end(); iter++) {
                Port* readPort = hwop.port(*iter);
                if ( readPort->inputSocket() == NULL) {
                    std::string errorMsg = 
                        (boost::format(
                            "Operation %s in FU %s reads operand %d which "
                            "is not connected to input socket. "
                            "Check port bindings!")
                         % hwop.name() % fu.name() % *iter).str();

                    results.addError(*this, (errorMsg));
                    retval = false;
                } else {
                    // check if this was read from the trigger
                    if (readPort == triggerPort) {
                        triggerRead = true;
                    }
                }
            }

            // was trigger actually read?
            if ( triggerok && !triggerRead) {
                std::string errorMsg = 
                    (boost::format(
                         "Trigger not bound for operation: %s in FU %s") % 
                     hwop.name() % fu.name()).str();
                results.addError(*this, errorMsg);
                retval = false;
            }

            // check write port bidings
            const ExecutionPipeline::OperandSet writes = 
                pipe.writtenOperands();
            for (ExecutionPipeline::OperandSet::const_iterator iter =
                     writes.begin(); iter != writes.end(); iter++) {
                Port* writePort = hwop.port(*iter);
                if ( writePort->outputSocket() == NULL) {
                    results.addError(
                        *this, (boost::format("Operation %s in FU %s "
                                              "writes operand %d"
                                              "which is not connected to "
                                              "output socket. " 
                                              "Check port bindings!")
                                % hwop.name() % fu.name() % *iter).str());
                    retval = false;
                }
            }
        }
    }
    return retval;
}
