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
 * @file AddressSpaceCheck.hh
 *
 * Declaration of AddressSpaceCheck class that checks two things:
 * 1) if the function units' operations access memory
 * 2) if there is a memory address space available for them
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef ADDRESS_SPACE_CHECK_HH
#define ADDRESS_SPACE_CHECK_HH

#include "MachineCheck.hh"

namespace TTAMachine {
    class Machine;
}

class OperationPool;

/**
 * MachineCheck to check if the FUs' operations access memory and
 * there is a memory address space available for them.
 */
class AddressSpaceCheck : public MachineCheck {
public:
    AddressSpaceCheck(OperationPool& operationPool);
    virtual ~AddressSpaceCheck();

    virtual bool check(const TTAMachine::Machine& mach,
                       MachineCheckResults& results) const;
    
private:
    OperationPool& operationPool_;

};

#endif
