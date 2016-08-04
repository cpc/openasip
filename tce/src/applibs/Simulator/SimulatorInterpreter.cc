/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SimulatorInterpreter.cc
 *
 * Implementation of SimulatorInterpreter class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
#include "MemWriteCommand.hh"

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
    addCustomCommand(new QuitCommand("quit"));
    addCustomCommand(new QuitCommand("exit"));
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
    addCustomCommand(new MemWriteCommand());
    addCustomCommand(new WatchCommand());
    addCustomCommand(new CommandsCommand());
    addCustomCommand(new SymbolAddressCommand());
    
    context.simulatorFrontend().setOutputStream(lineReader()->outputStream());
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
