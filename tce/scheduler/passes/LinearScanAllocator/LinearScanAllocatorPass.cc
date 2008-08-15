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
 * @file LinearScanAllocator.cc
 *
 * Implementation of LinearScanAllocator class
 *
 * Linear Scan register allocator.
 * This version Spills variables after allocation, not before.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <set>
#include <list>

//#include <boost/graph/strong_components.hpp>

//#include "AssocTools.hh"

#include "LinearScanAllocatorCore.hh"
#include "LinearScanAllocatorPass.hh"

#include "Program.hh"

/*
#include "AddressSpaceMapper.hh"

#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "DataDependenceGraphBuilder.hh"
#include "ControlFlowGraph.hh"

#include "LinearScanAllocatorCore.hh"
#include "StackManager.hh"
#include "RegisterMap.hh"
*/

using namespace TTAProgram;
using namespace TTAMachine;
using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

SCHEDULER_PASS(LinearScanAllocatorPass)

LinearScanAllocatorPass::LinearScanAllocatorPass() {}

LinearScanAllocatorPass::~LinearScanAllocatorPass() {}

void
LinearScanAllocatorPass::start() 
    throw (Exception) {

    for (int i = 0; i < program_->procedureCount(); i++) {
        Procedure& proc = program_->procedure(i);

        allocatorCore_.initialize(*target_,*program_,interPassData());
        allocatorCore_.allocateProcedure(proc);
    }
//    AddressSpaceMapper::mapAddressSpace(*program_,*target_);
}

/**
 * A short description of the module, usually the module name,
 * in this case "LinearScanAllocatorPass".
 *
 * @return The description as a string.
 */   
std::string
LinearScanAllocatorPass::shortDescription() const {
    return "Startable: LinearScanAllocatorPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
LinearScanAllocatorPass::longDescription() const {
    std::string answer = "Startable: LinearScanAllocatorPass.";
    answer += " Implements Linear scan register allocator.";
    return answer;
}
