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
 * @file SimulatorCmdLineOptions.hh
 *
 * Declaration of SimulatorCmdLineOptions.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIM_CMDLINE_OPTIONS_HH
#define TTA_SIM_CMDLINE_OPTIONS_HH

#include <string>

#include "CmdLineOptions.hh"
#include "SimulatorFrontend.hh"

/**
 * Command line option class for Simulator.
 */
class SimulatorCmdLineOptions : public CmdLineOptions {
public:
    SimulatorCmdLineOptions();
    virtual ~SimulatorCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

    bool debugMode();
    std::string scriptString();
    
    std::string machineFile();
    std::string programFile();
    SimulatorFrontend::SimulationType backendType();
    
private:
    /// Copying not allowed.
    SimulatorCmdLineOptions(const SimulatorCmdLineOptions&);
    /// Assignment not allowed.
    SimulatorCmdLineOptions& operator=(const SimulatorCmdLineOptions&);
};

#endif
