/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file SimulatorCLI.hh
 *
 * Declaration of SimulatorCLI class
 *
 * @author Pekka Jääskeläinen 2012 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_CLI
#define TTA_SIMULATOR_CLI

class SimulatorFrontend;
class SimulatorCmdLineOptions;
class TCEString;
class SimulatorInterpreterContext;
class LineReader;
class SimulatorInterpreter;
class RuntimeErrorReporter;

/**
 * Command Line Interface for controlling the simulation engine using
 * the simulator control scripting language (tcl).
 */
class SimulatorCLI {
public:
    SimulatorCLI(
        SimulatorFrontend& frontend);
    virtual ~SimulatorCLI();

    void run();
    void interpreteAndPrintResults(const TCEString& scriptString);

    SimulatorInterpreter& interpreter() { return *interpreter_; }
private:
    SimulatorFrontend& simFront_;
    SimulatorInterpreterContext* context_;
    LineReader* reader_;
    SimulatorInterpreter* interpreter_;
    SimulatorCmdLineOptions* options_;
    RuntimeErrorReporter* errorReporter_;
};

#endif

