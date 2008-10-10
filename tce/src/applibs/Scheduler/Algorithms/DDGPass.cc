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
 * 
 * @file DDGPass
 * 
 * Definition of a DDGPass class.
 *
  * @author Heikki Kultala 2007 (hkultala-no.spam-cs.tut.fi)
 * @note rating:red
 */

#include "DDGPass.hh"
#include "DataDependenceGraph.hh"
#include "Machine.hh"
#include "SimpleResourceManager.hh"

/**
 * Constructor.
 */
DDGPass::DDGPass(InterPassData& data) :
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
DDGPass::~DDGPass() {
}

/**
 * Handless a given DDG.
 *
 * @param ddg DDG to handle
 * @param rm Resource manager that is to be used.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (basicBlock
 * might still get modified).
 */
void
DDGPass::handleDDG(
    DataDependenceGraph& ddg,
    SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    ddg.nodeCount();
    rm.largestCycle();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
}
