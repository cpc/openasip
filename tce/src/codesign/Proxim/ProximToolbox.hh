/**
 * @file ProximToolbox.hh
 *
 * Declaration of ProximTools class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
