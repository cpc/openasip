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
 * @file CompiledSimInterpreter.cc
 *
 * Definition of CompiledSimInterpreter class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
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
