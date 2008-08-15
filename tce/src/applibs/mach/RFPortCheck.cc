/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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

