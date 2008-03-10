/**
 * @file SimulatorInterpreter.cc
 *
 * Implementation of SimulatorInterpreter class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SimulatorInterpreter.hh"
#include "TclInterpreter.hh"

// commands of the simulator control language
#include "HelpCommand.hh"
#include "ProgCommand.hh"
#include "MachCommand.hh"
#include "ConfCommand.hh"
#include "QuitCommand.hh"
#include "RunCommand.hh"
#include "StepiCommand.hh"
#include "DisassembleCommand.hh"
#include "UntilCommand.hh"
#include "SettingCommand.hh"
#include "ResumeCommand.hh"
#include "InfoCommand.hh"
#include "BPCommand.hh"
#include "TBPCommand.hh"
#include "ConditionCommand.hh"
#include "IgnoreCommand.hh"
#include "DeleteBPCommand.hh"
#include "EnableBPCommand.hh"
#include "DisableBPCommand.hh"
#include "NextiCommand.hh"
#include "KillCommand.hh"
#include "MemDumpCommand.hh"
#include "WatchCommand.hh"
#include "CommandsCommand.hh"
#include "SymbolAddressCommand.hh"

/**
 * Constructor.
 *
 * Initializes the interpreter. argc and argv variables are set to be
 * visible as interpreter variables.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param context The context data for the interpreter.
 * @param reader LineReader used by the interpreter.
 */
SimulatorInterpreter::SimulatorInterpreter(
	int argc, 
	char* argv[], 
	SimulatorInterpreterContext& context,
	LineReader& reader) : TclInterpreter(), quitted_(false) {

    initialize(argc, argv, &context, &reader);

    addCustomCommand(new HelpCommand());
    addCustomCommand(new ProgCommand());
    addCustomCommand(new MachCommand());
    addCustomCommand(new ConfCommand());
    addCustomCommand(new QuitCommand());
    addCustomCommand(new RunCommand());
    addCustomCommand(new StepiCommand());
    addCustomCommand(new DisassembleCommand());
    addCustomCommand(new UntilCommand());
    addCustomCommand(new SettingCommand());
    addCustomCommand(new ResumeCommand());
    addCustomCommand(new InfoCommand(
        context.simulatorFrontend().isCompiledSimulation()));
    addCustomCommand(new BPCommand());
    addCustomCommand(new TBPCommand());
    addCustomCommand(new ConditionCommand());
    addCustomCommand(new IgnoreCommand());
    addCustomCommand(new DeleteBPCommand());
    addCustomCommand(new EnableBPCommand());
    addCustomCommand(new DisableBPCommand());    
    addCustomCommand(new NextiCommand());
    addCustomCommand(new KillCommand());
    addCustomCommand(new MemDumpCommand());
    addCustomCommand(new WatchCommand());
    addCustomCommand(new CommandsCommand());
    addCustomCommand(new SymbolAddressCommand());
}

/**
 * Destructor.
 *
 * Finalizes the internal structures of the interpreter.
 */
SimulatorInterpreter::~SimulatorInterpreter() {
    finalize();
}


/**
 * Returns true in case interpreter has received the quit command.
 *
 * @param True if quite command has been given.
 */
bool 
SimulatorInterpreter::isQuitCommandGiven() const {
    return quitted_;
}

/**
 * Sets the quit command to be given.
 */
void 
SimulatorInterpreter::setQuitCommandGiven() {
    quitted_ = true;
}
