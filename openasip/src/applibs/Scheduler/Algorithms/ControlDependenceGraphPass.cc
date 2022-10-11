/*
    Copyright 2002-2008 Tampere University.  All Rights
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
 * @file ControlDependenceGraphPass.cc
 *
 * Definition of ControlDependenceGraphPass class.
 *
 * @author Vladimír Guzma 2008 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ControlDependenceGraphPass.hh"
#include "Application.hh"
#include "ControlDependenceGraph.hh"
#include "Machine.hh"
#include "BasicBlockPass.hh"

/**
 * Constructor.
 */
ControlDependenceGraphPass::ControlDependenceGraphPass(InterPassData& data) :
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
ControlDependenceGraphPass::~ControlDependenceGraphPass() {
}

/**
 * Handles a single control dependence graph.
 *
 * The pass should work with any kind of control dependence graph, it should not
 * assume the CDG represents a whole procedure, for example.
 *
 * @param cdg The control dependence graph to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (cdg might
 * still get modified).
 */
void
ControlDependenceGraphPass::handleControlDependenceGraph(
    ControlDependenceGraph& cdg, const TTAMachine::Machine& targetMachine) {
    //Trivial handling so far, just compute analysis info for serialization
    // Throws exception in case there is indirect jump
    cdg.nodeCount();
    try {
        cdg.analyzeCDG();
    } catch (const InvalidData& e) {
        // CFG had indirect jump
    } catch (const Exception& e) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }
    targetMachine.machineTester();
}
