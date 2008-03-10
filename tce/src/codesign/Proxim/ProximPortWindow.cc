/**
 * @file ProximPortWindow.cc
 *
 * Definition of ProximPortWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#include <string>

#include "ProximPortWindow.hh"
#include "Proxim.hh"
#include "WxConversion.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximSimulationThread.hh"

#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "FUPort.hh"
#include "MachineState.hh"
#include "PortState.hh"
#include "FUState.hh"
#include "StateData.hh"
#include "SimValue.hh"

#include <wx/listctrl.h>

using namespace TTAMachine;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximPortWindow::ProximPortWindow(
    ProximMainFrame* parent, int id):
    ProximUnitWindow(parent, id) {

    valueList_->InsertColumn(0, _T("Port"), wxLIST_FORMAT_LEFT, 100);
    valueList_->InsertColumn(1, _T("Value"), wxLIST_FORMAT_RIGHT, 120);

    if (simulator_->isSimulationInitialized() ||
	simulator_->isSimulationStopped() ||
	simulator_->hasSimulationEnded()) {

	reinitialize();
    }
}


/**
 * Destructor.
 */
ProximPortWindow::~ProximPortWindow() {    
}


/**
 * Sets the available function unit selections in the function unit choice.
 */
void
ProximPortWindow::reinitialize() {

    unitChoice_->Clear();

    string gcuName = simulator_->machine().controlUnit()->name();
    unitChoice_->Append(WxConversion::toWxString(gcuName));

    const TTAMachine::Machine::FunctionUnitNavigator& navigator =
	simulator_->machine().functionUnitNavigator();

    for (int i = 0; i < navigator.count(); i++) {
	string fuName = navigator.item(i)->name();
	unitChoice_->Append(WxConversion::toWxString(fuName));
    }

    unitChoice_->SetSelection(0);

    update();
}

/**
 * Changes unit selection to function unit with given name.
 *
 * @param name Name of the function unit to select.
 */
void
ProximPortWindow::showFunctionUnit(const std::string& name) {
    unitChoice_->SetStringSelection(WxConversion::toWxString(name));
    update();
}

/**
 * Updates the value list with port values of the selected function unit.
 */
void
ProximPortWindow::update() {

    valueList_->DeleteAllItems();

    MachineState& machState = simulator_->machineState();
    const TTAMachine::Machine::FunctionUnitNavigator& navigator =
	simulator_->machine().functionUnitNavigator();

    int fuIndex = unitChoice_->GetSelection();
    FunctionUnit* fu = NULL;

    // FU Choice item with index 0 is the control unit.
    if (fuIndex == 0) {
	fu = simulator_->machine().controlUnit();
    } else {
	fu = navigator.item(fuIndex - 1);
    }

    // Append all ports of the function unit to the port list.
    for (int i = 0; i < fu->portCount(); i++) {

	string portName = fu->port(i)->name();
	const PortState& state = machState.portState(portName, fu->name());

	wxString value;

        wxString mode = modeChoice_->GetStringSelection();

        if (mode == MODE_UNSIGNED) {
	    value = WxConversion::toWxString(state.value().unsignedValue());
        } else if (mode == MODE_INT) {
            int intValue = state.value().intValue();
	    value = WxConversion::toWxString(intValue);
        } else if (mode == MODE_HEX) {
	    value = WxConversion::toWxString(
                Conversion::toHexString(state.value().unsignedValue()));
        } else if (mode == MODE_BIN) {
            int intValue = state.value().unsignedValue();
	    value = WxConversion::toWxString(
                Conversion::toBinString(intValue));
        }

	valueList_->InsertItem(i, WxConversion::toWxString(portName));
	valueList_->SetItem(i, 1, value);
    }
}
