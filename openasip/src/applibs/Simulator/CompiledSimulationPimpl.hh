/*
    Copyright (c) 2002-2009 Tampere University.

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

    CompiledSimulationPimpl(const CompiledSimulationPimpl&) = delete;
    CompiledSimulationPimpl&
    operator=(const CompiledSimulationPimpl&) = delete;

protected:
    CompiledSimulationPimpl();

private:
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
