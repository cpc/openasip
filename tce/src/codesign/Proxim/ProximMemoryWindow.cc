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
 * @file ProximMemoryWindow.cc
 *
 * Definition of ProximMemoryWindow class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#include <string>
#include <wx/statline.h>
#include <wx/spinctrl.h>

#include "ProximMemoryWindow.hh"
#include "MemoryControl.hh"
#include "WxConversion.hh"
#include "MemorySystem.hh"
#include "Proxim.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximSimulationThread.hh"
#include "Machine.hh"
#include "MemoryProxy.hh"
#include "Conversion.hh"

BEGIN_EVENT_TABLE(ProximMemoryWindow, ProximSimulatorWindow)
    EVT_SIMULATOR_STOP(0, ProximMemoryWindow::onSimulationStop)
    EVT_SIMULATOR_PROGRAM_LOADED(0, ProximMemoryWindow::onProgramLoaded)
    EVT_CHOICE(ID_AS_CHOICE, ProximMemoryWindow::onASChoice)
END_EVENT_TABLE()

using std::string;
using namespace TTAMachine;


/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximMemoryWindow::ProximMemoryWindow(
    ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id, wxDefaultPosition, wxSize(800,600)),
    memorySystem_(NULL),
    memoryControl_(NULL), asInfoText_(NULL) {

    createContents();
    simulator_ = wxGetApp().simulation()->frontend();

    if (simulator_->isSimulationInitialized() ||
	simulator_->isSimulationStopped() ||
	simulator_->hasSimulationEnded()) {

	loadProgramMemory();
    }
}


/**
 * Destructor.
 */
ProximMemoryWindow::~ProximMemoryWindow() {    
}


/**
 * Loads memory from the given address space to the memory window.
 *
 * @param as Address space of the memory to load.
 */
void
ProximMemoryWindow::loadMemory(const AddressSpace& as) {

    MemorySystem& memorySystem = simulator_->memorySystem();

    MemorySystem::MemoryPtr mem = memorySystem.memory(as);

    if (memoryControl_ == NULL) {
        memoryControl_ =
            new MemoryControl(this, mem.get());
        sizer_->Add(memoryControl_, 1, wxGROW);
        sizer_->Layout();
        Fit();
    } else {
        memoryControl_->setMemory(mem.get());
    }
    
    wxString asInfo = WxConversion::toWxString(as.name());
    asInfo.Prepend(_T("    "));
    asInfo.Append(_T(":   "));
    asInfo.Append(
        WxConversion::toWxString(Conversion::toHexString(as.start())));
                      
    asInfo.Append(_T(" - "));

    asInfo.Append(
        WxConversion::toWxString(Conversion::toHexString(as.end())));

    asInfo.Append(_T("    MAU: "));
    asInfo.Append(WxConversion::toWxString(as.width()));
    asInfoText_->SetLabel(asInfo);
}


/**
 * Event handler which is called when a new program is loaded in the simulator.
 */
void
ProximMemoryWindow::onProgramLoaded(const SimulatorEvent&) {
    loadProgramMemory();
}

/**
 * Resets the window when a new program or machine is being loaded in the
 * simulator.
 */
void
ProximMemoryWindow::reset() {
    asChoice_->Clear();
    asInfoText_->SetLabel(_T(""));
    if (memoryControl_ != NULL) {
        memoryControl_->Destroy();
        memoryControl_ = NULL;
    }
}

/**
 * Initializes simulated memory system in the window.
 */
void
ProximMemoryWindow::loadProgramMemory() {
    asChoice_->Clear();
    MemorySystem& memorySystem = simulator_->memorySystem();
    
    for (unsigned int i = 0; i < memorySystem.memoryCount(); i++) {
        string asName = memorySystem.addressSpace(i).name();
        asChoice_->Append(WxConversion::toWxString(asName));
    }
    asChoice_->SetSelection(0);
    wxCommandEvent dummy;
    onASChoice(dummy);
}


/**
 * Event handler for the address space choicer.
 *
 * Loads the selected address space in the memory control.
 */
void
ProximMemoryWindow::onASChoice(wxCommandEvent&) {
    MemorySystem& memorySystem = simulator_->memorySystem();
    const AddressSpace& as =
	memorySystem.addressSpace(asChoice_->GetSelection());
    loadMemory(as);
}


/**
 * Event handler for simulation stop.
 *
 * Refreshes the memory display.
 */
void
ProximMemoryWindow::onSimulationStop(const SimulatorEvent&) {

    memoryControl_->clearHighlights();

    MemorySystem& memorySystem = simulator_->memorySystem();
    MemoryProxy* mem = dynamic_cast<MemoryProxy*>(
        memorySystem.memory(asChoice_->GetSelection()).get());

    if (mem != NULL) {

        unsigned reads = mem->readAccessCount();
        for (unsigned i = 0; i < reads; i++) {
            MemoryProxy::MemoryAccess access = mem->readAccess(i);
            memoryControl_->highlight(
                access.first, access.second, *wxGREEN);
        }

        unsigned writes = mem->writeAccessCount();
        for (unsigned i = 0; i < writes; i++) {
            MemoryProxy::MemoryAccess access = mem->writeAccess(i);
            memoryControl_->highlight(
                access.first, access.second, *wxRED);
        }
    }
    memoryControl_->updateView();
}


/**
 * Creates the window contents.
 */
void
ProximMemoryWindow::createContents() {

    sizer_= new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *item1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *item2 = new wxStaticText(
	this, ID_TEXT_AS, wxT("Address space:"),
        wxDefaultPosition, wxDefaultSize, 0);

    item1->Add(item2, 0, wxALIGN_CENTER|wxALL, 5);

    wxString *strs3 = (wxString*) NULL;
    asChoice_ = new wxChoice(
        this, ID_AS_CHOICE, wxDefaultPosition, wxSize(150,-1), 0, strs3, 0);

    item1->Add(asChoice_, 0, wxALIGN_CENTER|wxALL, 5);
    sizer_->Add(item1, 0, wxALIGN_CENTER|wxALL, 5);

    wxStaticLine *item4 = new wxStaticLine(
        this, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL);

    sizer_->Add(item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    asInfoText_ = new wxStaticText(this, ID_TEXT_AS_INFO, wxT(""));
    sizer_->Add(asInfoText_, 0, wxGROW|wxALIGN_CENTER|wxALL, 5);

    this->SetSizer(sizer_);
    sizer_->SetSizeHints(this);

}
