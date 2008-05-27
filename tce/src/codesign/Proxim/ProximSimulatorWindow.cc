/**
 * @file ProximSimulatorWindow.cc
 *
 * Implementation of ProximSimulatorWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximSimulatorWindow.hh"
#include "ProximMainFrame.hh"
#include "ProximToolbox.hh"


BEGIN_EVENT_TABLE(ProximSimulatorWindow, wxPanel)
    EVT_SIMULATOR_COMMAND(-1, ProximSimulatorWindow::onSimulatorBusy)
    EVT_SIMULATOR_COMMAND_DONE(-1, ProximSimulatorWindow::onSimulatorDone)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param mainFrame ProximMainFrame parent of the window.
 */
ProximSimulatorWindow::ProximSimulatorWindow(
    ProximMainFrame* mainFrame, wxWindowID id, wxPoint pos, wxSize size,
        long style) :
    wxPanel(mainFrame, id, pos, size, style) {

    mainFrame->addSubWindow(this);
}


/**
 * The Destructor.
 */
ProximSimulatorWindow::~ProximSimulatorWindow() {
    ProximMainFrame* mainFrame = ProximToolbox::mainFrame();
    if (mainFrame != NULL) {
        mainFrame->removeSubWindow(this);
    }
}


/**
 * Disables the window when the simulator is busy.
 */
void
ProximSimulatorWindow::onSimulatorBusy(SimulatorEvent&) {
    Disable();
}

/**
 * Enables the window when the simulator is not busy.
 */
void
ProximSimulatorWindow::onSimulatorDone(SimulatorEvent&) {
    Enable();
}

/**
 * Called before the simulator thread destroys any of it's data structures
 * accessed by GUI.
 */
void
ProximSimulatorWindow::reset() {
    GetParent()->Close();
}
