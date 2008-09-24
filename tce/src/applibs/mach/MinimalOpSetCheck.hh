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
 * @file MinimalOpSetCheck.hh
 *
 * Declaration of MinimalOpSetCheck class that checks and fixes machines
 * operation set.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MINIMAL_OPSET_CHECK_HH
#define MINIMAL_OPSET_CHECK_HH

#include <set>
#include <vector>
#include <string>

#include "MachineCheck.hh"
#include "MachineCheckResults.hh"
#include "Machine.hh"

/**
 * MachineCheck to check is the machine fully connected.
 */
class MinimalOpSetCheck : public MachineCheck {

public:
    MinimalOpSetCheck();
    virtual ~MinimalOpSetCheck();

    virtual bool check(
        const TTAMachine::Machine& machine, MachineCheckResults& results) const;
    virtual bool check(
        const TTAMachine::Machine& machine) const;

    bool checkWithIgnore(
        const TTAMachine::Machine& machine,
        const std::set<std::string>& ignoreFUName) const;

    void missingOperations(
        const TTAMachine::Machine& machine,
        std::vector<std::string>& missingOps) const;

    void buildMinimalOpSet(const TTAMachine::Machine* machine = NULL);
    std::set<std::string> minimalOpSet() const;

    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& machine) const
        throw (InvalidData);

private:
    // minimal opset
    std::set<std::string> minimalOpSet_;
};

#endif

