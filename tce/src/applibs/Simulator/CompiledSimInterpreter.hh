/**
 * @file CompiledSimInterpreter.hh
 *
 * Declaration of CompiledSimInterpreter class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_INTERPRETER
#define COMPILED_SIM_INTERPRETER

#include "SimulatorInterpreter.hh"

/**
 * SimulatorInterpreter class specific to the compiled simulator
 *
 */
class CompiledSimInterpreter : public SimulatorInterpreter {
public:
    CompiledSimInterpreter(
        int argc, 
        char* argv[], 
        SimulatorInterpreterContext& context,
        LineReader& reader);
    virtual ~CompiledSimInterpreter();
};

#endif
