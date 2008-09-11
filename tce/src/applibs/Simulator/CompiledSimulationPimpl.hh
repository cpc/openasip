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
 * @file CompiledSimulationPimpl.hh
 *
 * Declaration of CompiledSimulationPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIMULATION_PIMPL_HH
#define COMPILED_SIMULATION_PIMPL_HH

#include <map>
#include <string>
#include <vector>
#include <set>

#include "CompiledSimulation.hh"
#include "CompiledSimCompiler.hh"
#include "PluginTools.hh"

class MemorySystem;
class SimulatorFrontend;
class CompiledSimController;

/// Type for the jump table
typedef std::vector<SimulateFunction> JumpTable;


class CompiledSimulationPimpl {
public:
    friend class CompiledSimulation;
    ~CompiledSimulationPimpl();
private:
    CompiledSimulationPimpl();
    /// Copying not allowed.
    CompiledSimulationPimpl(const CompiledSimulationPimpl&);
    /// Assignment not allowed.
    CompiledSimulationPimpl& operator=(const CompiledSimulationPimpl&);
    
    /// The memory system
    MemorySystem* memorySystem_;
    /// The simulator frontend
    SimulatorFrontend* frontend_;
    /// Simulation controller
    CompiledSimController* controller_;
    
    /// Type for symbol map: string = symbolname, SimValue* = value location
    typedef std::map<std::string, SimValue*> Symbols;
    /// A Symbol map for easily getting the SimValues out of the simulation
    Symbols symbols_;
    /// The jump table
    JumpTable jumpTable_;
    
    /// Program exit points in a set
    std::set<InstructionAddress> exitPoints_;
    
    /// The Compiled Simulation compiler
    CompiledSimCompiler compiler_;
    /// Plugintools used to load the compiled .so files
    PluginTools pluginTools_;
};


#endif
