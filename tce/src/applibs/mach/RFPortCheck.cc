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
 * @file RFPortCheck.hh
 *
 * Implementation of RFPortCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pekka.jaaskelainen-no.spam-tut.fi)
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

