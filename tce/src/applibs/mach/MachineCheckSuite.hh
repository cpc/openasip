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
 * @file MachineCheckSuite.hh
 *
 * Declaration of MachineCheckSuite class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CHECK_SUITE_HH
#define TTA_MACHINE_CHECK_SUITE_HH

#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

class MachineCheck;
class MachineCheckResults;

/**
 * Container for a group machine checks that are meant to be run as
 * a test suite.
 */
class MachineCheckSuite {
public:
    virtual ~MachineCheckSuite();

    int checkCount() const;
    const MachineCheck& check(int index)
        throw (OutOfRange);

    bool run(
        const TTAMachine::Machine& machine,
        MachineCheckResults& results);

protected:
    MachineCheckSuite();
    void addCheck(MachineCheck* check);

private:
    /// Checks contained in the suite.
    std::vector<MachineCheck*> checks_;

};

#endif
