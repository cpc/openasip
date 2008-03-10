/**
 * @file ProximRegisterWindow.cc
 *
 * Definition of ProximRegisterWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#include <string>

#include "ProximRegisterWindow.hh"
#include "WxConversion.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximMainFrame.hh"

#include "Machine.hh"
#include "UniversalMachine.hh"
#include "RegisterFile.hh"
#include "UnboundedRegisterFile.hh"

#include "MachineState.hh"
#include "RegisterState.hh"
#include "UnboundRegisterFileState.hh"
#include "StateData.hh"
#include "SimValue.hh"
#include "LongImmediateUnitState.hh"
#include "LongImmediateRegisterState.hh"

#include "DisassemblyIntRegister.hh"
#include "DisassemblyFPRegister.hh"
#include "DisassemblyBoolRegister.hh"
#include "DisassemblyRegister.hh"

#include <wx/listctrl.h>

using std::string;
using namespace TTAMachine;

const wxString ProximRegisterWindow::UNIVERSAL_INT_RF = _T("int RF");
const wxString ProximRegisterWindow::UNIVERSAL_DOUBLE_RF = _T("double RF");
const wxString ProximRegisterWindow::UNIVERSAL_BOOL_RF = _T("bool RF");

const std::string ProximRegisterWindow::RF_PREFIX = "RF: ";
const std::string ProximRegisterWindow::IMM_PREFIX = "IMM: ";

/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximRegisterWindow::ProximRegisterWindow(
    ProximMainFrame* parent, int id):
    ProximUnitWindow(parent, id) {

    valueList_->InsertColumn(0, _T("Register"), wxLIST_FORMAT_LEFT, 100);
    valueList_->InsertColumn(1, _T("Value"), wxLIST_FORMAT_RIGHT, 120);

    modeChoice_->Disable();

    if (simulator_->isSimulationInitialized() ||
	simulator_->isSimulationStopped() ||
	simulator_->hasSimulationEnded()) {

	reinitialize();
    }
}


/**
 * Destructor.
 */
ProximRegisterWindow::~ProximRegisterWindow() {    
}


/**
 * Sets the available register file selections in the register file choice.
 */
void
ProximRegisterWindow::reinitialize() {

    unitChoice_->Clear();

    if (simulator_->isSequentialSimulation()) {
        // Append universal machine register names.
        unitChoice_->Append(UNIVERSAL_INT_RF);
        unitChoice_->Append(UNIVERSAL_DOUBLE_RF);
        unitChoice_->Append(UNIVERSAL_BOOL_RF);
        unitChoice_->SetSelection(0);
        update();
        return;
    }

    // parallel simulation
    const Machine::RegisterFileNavigator& rfNavigator =
	simulator_->machine().registerFileNavigator();

    for (int i = 0; i < rfNavigator.count(); i++) {
	string rfName = RF_PREFIX + rfNavigator.item(i)->name();
	unitChoice_->Append(WxConversion::toWxString(rfName));
    }

    const Machine::ImmediateUnitNavigator& immNavigator =
	simulator_->machine().immediateUnitNavigator();
    for (int i = 0; i < immNavigator.count(); i++) {
	string rfName = IMM_PREFIX + immNavigator.item(i)->name();
	unitChoice_->Append(WxConversion::toWxString(rfName));
    }

    unitChoice_->SetSelection(0);
    update();
}


/**
 * Event handler for the register file choicer.
 *
 * When the choice selection changes, corresponding register file registers
 * are loaded in the register list.
 */
void
ProximRegisterWindow::update() {

    int rfIndex = unitChoice_->GetSelection();

    modeChoice_->Disable();
    
    // Universal machine choices:
    if (simulator_->isSequentialSimulation()) {

	if (rfIndex == 0) {
	    modeChoice_->Enable();
	    loadUniversalIntegerRF();
	}
	if (rfIndex == 1) {
	    loadUniversalFloatRF();
	}
	if (rfIndex == 2) {
	    loadUniversalBoolRF();
	}
	return;

    } else {
        modeChoice_->Enable();
    }

    // Machine is not a universal machine.
    const Machine::RegisterFileNavigator& rfNavigator =
	simulator_->machine().registerFileNavigator();

    if (rfIndex < rfNavigator.count()) {
        loadRegisterFile(*rfNavigator.item(rfIndex));
    } else {
        const Machine::ImmediateUnitNavigator& immNavigator =
            simulator_->machine().immediateUnitNavigator();

        rfIndex = rfIndex - rfNavigator.count();
        loadImmediateUnit(*immNavigator.item(rfIndex));
    }
}

/**
 * Loads register file registers to the register list.
 *
 * @param rf Register file to display.
 */
void
ProximRegisterWindow::loadRegisterFile(const RegisterFile& rf) {
    
    valueList_->DeleteAllItems();
    
    RegisterFileState& rfState =
	simulator_->machineState().registerFileState(rf.name());

    // Append all registers to the register list.
    int row = 0;
    for (unsigned i = 0; i < rfState.registerCount(); i++) {
	wxString value;
	const RegisterState& state = rfState.registerState(i);

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
            int intValue = state.value().intValue();
	    value = WxConversion::toWxString(
                Conversion::toBinString(intValue));
        }

	DisassemblyRegister r(rf.name(), i);
	valueList_->InsertItem(
	    row, WxConversion::toWxString(r.toString()));

	valueList_->SetItem(row, 1, value);
	row++;
    }
}

/**
 * Loads register file registers to the register list.
 *
 * @param imm Immediate Unit to display.
 */
void
ProximRegisterWindow::loadImmediateUnit(const ImmediateUnit& imm) {
    
    valueList_->DeleteAllItems();
    
    LongImmediateUnitState& immState =
	simulator_->machineState().longImmediateUnitState(imm.name());

    // Append all registers to the register list.
    int row = 0;
    for (int i = 0; i < immState.immediateRegisterCount(); i++) {
	wxString value;

	const LongImmediateRegisterState& state =
            immState.immediateRegister(i);

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
            int intValue = static_cast<int>(state.value().unsignedValue());
	    value = WxConversion::toWxString(
                Conversion::toBinString(intValue));
        }

	DisassemblyRegister r(imm.name(), i);
	valueList_->InsertItem(
	    row, WxConversion::toWxString(r.toString()));

	valueList_->SetItem(row, 1, value);
	row++;
    }
}


/**
 * Loads universal machine integer registerfile to the register list.
 *
 * Only registers that are actually used by the simualted program are appended
 * to the list.
 */
void
ProximRegisterWindow::loadUniversalIntegerRF() {

    valueList_->DeleteAllItems();

    string rfName = dynamic_cast<const UniversalMachine&>(
	simulator_->machine()).integerRegisterFile().name();

    UnboundRegisterFileState& rfState =
	dynamic_cast<UnboundRegisterFileState&>(
	    simulator_->machineState().registerFileState(rfName));

    // Append all used registers to the register list.
    int row = 0;
    for (unsigned i = 0; i < rfState.registerCount(); i++) {
	wxString value;
	if (rfState.isRegisterInUse(i)) {
	    const RegisterState& state = rfState.registerState(i);
            wxString mode = modeChoice_->GetStringSelection();
            if (mode == MODE_UNSIGNED) {
                value = WxConversion::toWxString(
                    state.value().unsignedValue());

            } else if (mode == MODE_INT) {
                int intValue = state.value().intValue();

                value = WxConversion::toWxString(intValue);
            } else if (mode == MODE_HEX) {
                value = WxConversion::toWxString(
                    Conversion::toHexString(state.value().unsignedValue()));

            } else if (mode == MODE_BIN) {
                int intValue = static_cast<int>(state.value().unsignedValue());
                value = WxConversion::toWxString(
                    Conversion::toBinString(intValue));
            }
	    DisassemblyIntRegister r(i);
	    valueList_->InsertItem(
		row, WxConversion::toWxString(r.toString()));
	    valueList_->SetItem(row, 1, value);
	    row++;
	}
    }
}


/**
 * Loads universal machine floating point registerfile to the register list.
 *
 * Only registers that are actually used by the simualted program are appended
 * to the list.
 */
void
ProximRegisterWindow::loadUniversalFloatRF() {

    valueList_->DeleteAllItems();

    string rfName = dynamic_cast<const UniversalMachine&>(
	simulator_->machine()).doubleRegisterFile().name();
    UnboundRegisterFileState& rfState =
	dynamic_cast<UnboundRegisterFileState&>(
	    simulator_->machineState().registerFileState(rfName));

    // Append all used registers to the register list.
    int row = 0;
    for (unsigned i = 0; i < rfState.registerCount(); i++) {
	if (rfState.isRegisterInUse(i)) {
	    const RegisterState& state = rfState.registerState(i);
	    wxString value = WxConversion::toWxString(
                static_cast<FloatWord>(state.value().floatWordValue()));

	    DisassemblyFPRegister r(i);
	    valueList_->InsertItem(
		row, WxConversion::toWxString(r.toString()));

	    valueList_->SetItem(row, 1, value);
	    row++;
	}
    }
}


/**
 * Loads universal machine bool register to the register list.
 */
void
ProximRegisterWindow::loadUniversalBoolRF() {
    valueList_->DeleteAllItems();

    const StateData& state = simulator_->findBooleanRegister();
    bool boolValue = bool(state.value().uIntWordValue());
    wxString value;
    if (boolValue) {
	value = _T("true");
    } else {
	value = _T("false");
    }
    DisassemblyBoolRegister reg;
    valueList_->InsertItem(0, WxConversion::toWxString(reg.toString()));
    valueList_->SetItem(0, 1, value);
}


/**
 * Sets the registerfile dipslayed in the window.
 *
 * @param name Name of the register file.
 */
void
ProximRegisterWindow::showRegisterFile(const std::string& name) {

    if (simulator_->isSequentialSimulation()) {

        const UniversalMachine& machine =
            dynamic_cast<const UniversalMachine&>(simulator_->machine());

        if (name == machine.integerRegisterFile().name()) {
            unitChoice_->SetStringSelection(UNIVERSAL_INT_RF);
        } else if (name == machine.doubleRegisterFile().name()) {
            unitChoice_->SetStringSelection(UNIVERSAL_DOUBLE_RF);
        } else if (name == machine.booleanRegisterFile().name()) {
            unitChoice_->SetStringSelection(UNIVERSAL_BOOL_RF);
        }
    } else {
        unitChoice_->SetStringSelection(
            WxConversion::toWxString(RF_PREFIX + name));
    }
    update();
}

/**
 * Sets the immediate unit dipslayed in the window.
 *
 * @param name Name of the immediate unit.
 */
void
ProximRegisterWindow::showImmediateUnit(const std::string& name) {

    assert(!simulator_->isSequentialSimulation());

    unitChoice_->SetStringSelection(
        WxConversion::toWxString(IMM_PREFIX + name));

    update();
}
