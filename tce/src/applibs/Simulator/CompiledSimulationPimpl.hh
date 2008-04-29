/**
 * @file CompiledSimulationPimpl.hh
 *
 * Declaration of CompiledSimulationPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIMULATION_PIMPL_HH
#define COMPILED_SIMULATION_PIMPL_HH

#include <map>
#include <string>
#include <vector>

#include "CompiledSimulation.hh"

class MemorySystem;
class SimulatorFrontend;

/// Type for the jump table
typedef std::vector<SimulateFunction> JumpTable;


class CompiledSimulationPimpl {
public:
    friend class CompiledSimulation;
    ~CompiledSimulationPimpl();
private:
    CompiledSimulationPimpl();
    
    /// The memory system
    MemorySystem* memorySystem_;
    /// The simulator frontend
    SimulatorFrontend* frontend_;
    
    /// Type for symbol map: string = symbolname, SimValue* = value location
    typedef std::map<std::string, SimValue*> Symbols;
    /// A Symbol map for easily getting the SimValues out of the simulation
    Symbols symbols_;
    /// The jump table
    JumpTable jumpTable_;
};


#endif
