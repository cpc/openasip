/**
 * @file ProximToolbox.cc
 *
 * Implementation of ProximToolbox class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include "ProximToolbox.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximMainFrame.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximDisassemblyWindow.hh"
#include "NullProgram.hh"
#include "NullMachine.hh"
#include "ProximConstants.hh"
#include "ExpressionScript.hh"
#include "TclConditionScript.hh"
#include "ErrorDialog.hh"
#include "WxConversion.hh"
#include "SimulatorInterpreter.hh"

using std::string;
using std::vector;

/**
 * The Constructor.
 */
ProximToolbox::ProximToolbox() {
}


/**
 * Returns the simulated machine.
 *
 * Returns NullMachine, if a machine is not loaded in the simulator.
 *
 * @return Reference to the simulated machine.
 */
const TTAMachine::Machine&
ProximToolbox::machine() {

    TracedSimulatorFrontend* frontend = wxGetApp().simulation()->frontend();
    if (frontend == NULL) {
        return TTAMachine::NullMachine::instance();
    } else {
        return frontend->machine();
    }
}


/**
 * Returns the simulated program.
 *
 * Returns NullProgram, if a program is not loaded in the simulator.
 *
 * @return Reference to the simulated program.
 */
const TTAProgram::Program&
ProximToolbox::program() {

    TracedSimulatorFrontend* frontend = wxGetApp().simulation()->frontend();
    if (frontend == NULL) {
        return TTAProgram::NullProgram::instance();
    } else {
        return frontend->program();
    }
}


/**
 * Returns pointer to the application main frame.
 *
 * @return Application main frame, or NULL if the main frame does not exist.
 */
ProximMainFrame*
ProximToolbox::mainFrame() {
    wxWindow* topWindow = wxGetApp().GetTopWindow();
    if (topWindow != NULL) {
        return dynamic_cast<ProximMainFrame*>(topWindow);
    } else {
        return NULL;
    }
}


/**
 * Returns pointer to the machine state window.
 *
 * Returns NULL if the machine state window does not exist.
 *
 * @return Pointer to the machine state window.
 */
ProximMachineStateWindow*
ProximToolbox::machineStateWindow() {

    wxWindow* topWindow = wxGetApp().GetTopWindow();

    if (topWindow == NULL) {
        return NULL;
    }

    wxWindow* machineWin = topWindow->FindWindow(
        ProximConstants::ID_MACHINE_STATE_WINDOW);

    if (machineWin == NULL) {
        return NULL;
    }

    return dynamic_cast<ProximMachineStateWindow*>(machineWin);
}


/**
 * Returns pointer to the program disassembly window.
 *
 * Returns NULL if the disassembly window does not exist.
 *
 * @return Pointer to the disassembly window.
 */
ProximDisassemblyWindow*
ProximToolbox::disassemblyWindow() {

    wxWindow* topWindow = wxGetApp().GetTopWindow();

    if (topWindow == NULL) {
        return NULL;
    }

    wxWindow* disasmWin = topWindow->FindWindow(
        ProximConstants::ID_DISASSEMBLY_WINDOW);

    if (disasmWin == NULL) {
        return NULL;
    }

    return dynamic_cast<ProximDisassemblyWindow*>(disasmWin);
}


/**
 * Adds a new simulator window in it's own floating frame.
 *
 * @param window Window to add.
 * @param title Title of the frame.
 */
void
ProximToolbox::addFramedWindow(
    wxWindow* window, const wxString& title, bool stayOnTop,
    const wxSize& minSize) {

    // Create a new frame for the window.
    long style = wxDEFAULT_FRAME_STYLE;
    if (stayOnTop) {
	style = (style | wxFRAME_FLOAT_ON_PARENT);
    }

    wxFrame* frame = new wxFrame(
	mainFrame(), -1, title, wxDefaultPosition, wxDefaultSize, style);

    window->Reparent(frame);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(window, 1, wxGROW);
    sizer->SetSizeHints(window);
    frame->SetSizer(sizer);
    frame->Fit();
    frame->SetSizeHints(minSize.GetWidth(), minSize.GetHeight());
    frame->Show();
    return;
}

/**
 * Returns pointer to the simulator control language interpreter used by the
 * simulator backend.
 *
 * @return Simulator control language interpreter of the simulator.
 */
SimulatorInterpreter*
ProximToolbox::interpreter() {

    ProximSimulationThread* simulation = wxGetApp().simulation();
    if (simulation == NULL) {
        return NULL;
    }

    return simulation->interpreter();
}

/**
 * Returns pointer to the simulator's frontend.
 *
 * @return Simulator frontend.
 */
TracedSimulatorFrontend*
ProximToolbox::frontend() {
    if (wxGetApp().simulation() == NULL) {
        return NULL;
    } else {
        return wxGetApp().simulation()->frontend();
    }
}


/**
 * Returns reference to the line reader used by the simulator engine.
 *
 * @return Simulator linereader.
 */
ProximLineReader&
ProximToolbox::lineReader() {
    return wxGetApp().simulation()->lineReader();
}


/**
 * Tests expression script validity.
 *
 * An error dialog is displayed if the script is not legal.
 *
 * @param parent Parent window for the possible error dialog to display.
 * @param expression Expression script to test.
 */
bool
ProximToolbox::testExpression(
    wxWindow* parent, const std::string&  expression) {

    ExpressionScript expressionScript(interpreter(), expression);

    // Check condition script validity using script interpreter.
    try {
        expressionScript.execute();
    } catch (const ScriptExecutionFailure& sef) {
        // Condition erroneous. Display error dialog.
        wxString message = _T("Error in expression:\n");
        message.Append(WxConversion::toWxString(sef.errorMessage()));
        ErrorDialog dialog(parent, message);
        dialog.ShowModal();
        return false;
    }
    return true;
}

/**
 * Tests condition script validity.
 *
 * An error dialog is displayed if the script is not legal.
 *
 * @param parent Parent window for the possible error dialog to display.
 * @param condition Condition script to test.
 */
bool
ProximToolbox::testCondition(
    wxWindow* parent, const std::string& condition) {

    TclConditionScript conditionScript(interpreter(), condition);

    // Check condition script validity using script interpreter.
    try {
        conditionScript.execute();
    } catch (const ScriptExecutionFailure& sef) {
        // Condition erroneous. Display error dialog.
        wxString message = _T("Error in condition:\n");
        message.Append(WxConversion::toWxString(sef.errorMessage()));
        ErrorDialog dialog(parent, message);
        dialog.ShowModal();
        return false;
    }
    return true;
}
