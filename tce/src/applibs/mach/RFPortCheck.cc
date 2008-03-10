/**
 * @file RFPortCheck.hh
 *
 * Implementation of RFPortCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "MachineCheck.hh"
#include "RFPortCheck.hh"
#include "RegisterFile.hh"
#include "Machine.hh"

using namespace TTAMachine;

/**
 * The constructor.
 */
RFPortCheck::RFPortCheck() : MachineCheck("Sanity checks for RF ports.") {
}

/**
 * The destructor.
 */
RFPortCheck::~RFPortCheck() {
}

/**
 * Sanity checks the RF ports of the target machine.
 *
 * @param mach Machine to be checked.
 * @param results Check results.
 * @return True if the check passed
 */
bool
RFPortCheck::check(
    const TTAMachine::Machine& mach, 
    MachineCheckResults& results) const {

    bool checksOk = true;
    Machine::RegisterFileNavigator RFs = mach.registerFileNavigator();    
    for (int i = 0; i < RFs.count(); i++) {
        const TTAMachine::RegisterFile& rf = *RFs.item(i);
        int inputPorts = 0;
        const int ports = rf.portCount();
        for (int p = 0; p < ports; ++p) {
            const TTAMachine::RFPort* port = rf.port(p);
            if (port->isInput()) {
                ++inputPorts;
                break;
            }
        }
        if (inputPorts == 0) {
            results.addError(
                *this, 
                (boost::format(
                    "Register file '%s' does not have any input ports.").
                 str()));
            checksOk = false;
        }        
    } 

    /// @todo Check if there is no read ports and the register file has
    /// at least one register which is not a predicate reg (read by GCU).
    
    return checksOk;
}

