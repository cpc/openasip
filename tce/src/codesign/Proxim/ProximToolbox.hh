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
 * @file ProximToolbox.hh
 *
 * Declaration of ProximTools class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_TOOLS_HH
#define TTA_PROXIM_TOOLS_HH

#include <wx/wx.h>

class ProximMainFrame;
class ProximMachineStateWindow;
class ProximDisassemblyWindow;
class SimulatorInterpreter;
class TracedSimulatorFrontend;
class ProximLineReader;

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

/**
 * Set of helper functions to access Proxim windows and simulation.
 */
class ProximToolbox {
public:
    static const TTAMachine::Machine& machine();
    static const TTAProgram::Program& program();

    static ProximMainFrame* mainFrame();
    static ProximMachineStateWindow* machineStateWindow();
    static ProximDisassemblyWindow* disassemblyWindow();

    static void addFramedWindow(
        wxWindow* window, const wxString& title, bool stayOnTop = false,
        const wxSize& minSize = wxSize(100, 100));

    static SimulatorInterpreter* interpreter();
    static TracedSimulatorFrontend* frontend();
    static ProximLineReader& lineReader();

    static bool testExpression(
        wxWindow* parent, const std::string& expression);
    static bool testCondition(
        wxWindow* parent, const std::string& condition);

protected:
    /// Instantiation not allowed.
    ProximToolbox();
};

#endif
