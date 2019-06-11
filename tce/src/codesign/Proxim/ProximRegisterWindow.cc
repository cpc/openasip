/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file ProximRegisterWindow.cc
 *
 * Definition of ProximRegisterWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */


#include <string>

#include "ProximRegisterWindow.hh"
#include "WxConversion.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximMainFrame.hh"

#include "Machine.hh"
#include "RegisterFile.hh"
#include "RegisterFileState.hh"
#include "MachineState.hh"
#include "RegisterState.hh"
#include "StateData.hh"
#include "SimValue.hh"
#include "LongImmediateUnitState.hh"
#include "LongImmediateRegisterState.hh"

#include "DisassemblyIntRegister.hh"
#include "DisassemblyFPRegister.hh"
#include "DisassemblyBoolRegister.hh"
#include "DisassemblyRegister.hh"

#include "Conversion.hh"

#include <wx/listctrl.h>

using std::string;
using namespace TTAMachine;

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
    modeChoice_->Enable();
    
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
 * Sets the registerfile displayed in the window.
 *
 * @param name Name of the register file.
 */
void
ProximRegisterWindow::showRegisterFile(const std::string& name) {

    unitChoice_->SetStringSelection(
        WxConversion::toWxString(RF_PREFIX + name));
    update();
}

/**
 * Sets the immediate unit dipslayed in the window.
 *
 * @param name Name of the immediate unit.
 */
void
ProximRegisterWindow::showImmediateUnit(const std::string& name) {

    unitChoice_->SetStringSelection(
        WxConversion::toWxString(IMM_PREFIX + name));
    update();
}
