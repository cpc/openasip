/**
 * @file CompiledSimInterpreter.cc
 *
 * Definition of CompiledSimInterpreter class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "CompiledSimInterpreter.hh"
#include "CompiledSimSettingCommand.hh"

/**
 * The constructor
 * @param argc argument count
 * @param argv argument list
 * @param context simulator interpreter context
 * @param reader command line reader
 */
CompiledSimInterpreter::CompiledSimInterpreter(
        int argc, 
        char* argv[], 
        SimulatorInterpreterContext& context,
        LineReader& reader) : SimulatorInterpreter(
        argc, argv, context, reader) {
            
    // remove unsupported commands
    removeCustomCommand("bp");
    removeCustomCommand("tbp");
    removeCustomCommand("condition");
    removeCustomCommand("enablebp");
    removeCustomCommand("disablebp");
    removeCustomCommand("deletebp");
    removeCustomCommand("ignore");
    removeCustomCommand("setting");
    addCustomCommand(new CompiledSimSettingCommand());
}

/**
 * Default destructor
 */
CompiledSimInterpreter::~CompiledSimInterpreter() {
}
