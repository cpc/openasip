/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file AddRFFromHDBDialog.cc
 *
 * Implementation of AddRFFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/dir.h>

#include "AddRFFromHDBDialog.hh"
#include "Model.hh"
#include "Machine.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "HDBManager.hh"
#include "RFArchitecture.hh"
#include "WarningDialog.hh"
#include "MapTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "HDBRegistry.hh"
#include "Conversion.hh"

using std::string;
using boost::format;
using namespace TTAMachine;
using namespace HDB;

BEGIN_EVENT_TABLE(AddRFFromHDBDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_LIST, AddRFFromHDBDialog::onListSelectionChange)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, AddRFFromHDBDialog::onListSelectionChange)
    EVT_BUTTON(ID_ADD, AddRFFromHDBDialog::onAdd)
    EVT_BUTTON(ID_CLOSE, AddRFFromHDBDialog::onClose)
END_EVENT_TABLE()


const int AddRFFromHDBDialog::DEFAULT_SIZE = 1;
const int AddRFFromHDBDialog::DEFAULT_WIDTH = 32;
const wxString AddRFFromHDBDialog::HDB_FILE_FILTER = _T("*.hdb");

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent Machine of the immediate slots.
 */
AddRFFromHDBDialog::AddRFFromHDBDialog(
    wxWindow* parent,
    Model* model) :
    wxDialog(
        parent, -1, _T("HDB Register Files"), wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    model_(model) {        

    createContents(this, true, true);
    SetSize(400, 300);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));

    list_->InsertColumn(0, _T("Width"));
    list_->InsertColumn(1, _T("Size"));
    list_->InsertColumn(2, _T("Read ports"));
    list_->InsertColumn(3, _T("Write ports"));
    list_->InsertColumn(4, _T("Bidir ports"));
    list_->InsertColumn(5, _T("Max Reads"));
    list_->InsertColumn(6, _T("Max RW"));
    list_->InsertColumn(7, _T("ID"));
    list_->InsertColumn(8, _T("HDB"));

    // Disable conditional buttons.
    FindWindow(ID_ADD)->Disable();
}


/**
 * The Destructor.
 */
AddRFFromHDBDialog::~AddRFFromHDBDialog() {
}


/**
 * Transfers data from the HDBs to the dialog list.
 *
 * @return True, if the data transfer was ssuccesful.
 */
bool
AddRFFromHDBDialog::TransferDataToWindow() {

    MapTools::deleteAllValues(rfArchitectures_);
    list_->DeleteAllItems();

    HDBRegistry& registry = HDBRegistry::instance();
    registry.loadFromSearchPaths();

    for (int i = 0; i < registry.hdbCount(); i++) {
        loadHDB(registry.hdb(i));
    }

    wxString errorMessage;
    for (int i = 0; i < registry.hdbErrorCount(); i++) {
        errorMessage.Append(
            WxConversion::toWxString(registry.hdbErrorMessage(i)));
        errorMessage.Append(_T("\n"));
        WarningDialog dialog(this, errorMessage);
        dialog.ShowModal();
    }

    if (registry.hdbCount() < 1) {
        wxString message = _T("No HDBs found in HDB search paths.");
        WarningDialog dialog(this, message);
        dialog.ShowModal();
    }

    list_->SetColumnWidth(8, wxLIST_AUTOSIZE);
    return wxDialog::TransferDataToWindow();
}

/**
 * Loads register files from a HDB to the dialog list.
 *
 * @param manager HDBManager managing the HDB to load.
 * @return True, if the HDB was succesfully loaded.
 */
bool
AddRFFromHDBDialog::loadHDB(const HDB::HDBManager& manager) {

    std::string path = manager.fileName();

    const std::set<RowID> rfArchIDs = manager.rfArchitectureIDs();
    std::set<RowID>::iterator iter = rfArchIDs.begin();

    // Read properties of all register files in the HDB and append
    // data to the register file list widget.
    for (; iter != rfArchIDs.end(); iter++) {

        RFArchitecture* arch = manager.rfArchitectureByID(*iter);

        if (arch->latency() != 1) {
            continue;
        }

        rfArchitectures_.insert(
            std::pair<int, RFArchitecture*>(list_->GetItemCount(), arch));

        if (arch->hasParameterizedWidth()) {
            list_->InsertItem(0, _T("param"));
        } else {
            list_->InsertItem(0, WxConversion::toWxString(arch->width()));
        }
        if (arch->hasParameterizedSize()) {
            list_->SetItem(0, 1, _T("param"));
        } else {
            list_->SetItem(0, 1, WxConversion::toWxString(arch->size()));
        }
        list_->SetItem(0, 2, WxConversion::toWxString(arch->readPortCount())); 
        list_->SetItem(
            0, 3, WxConversion::toWxString(arch->writePortCount())); 
        list_->SetItem(0, 4, WxConversion::toWxString(arch->bidirPortCount()));
        list_->SetItem(0, 5, WxConversion::toWxString(arch->maxReads())); 
        list_->SetItem(0, 6, WxConversion::toWxString(arch->maxWrites())); 
        list_->SetItem(0, 7, WxConversion::toWxString(*iter));
        list_->SetItem(0, 8, WxConversion::toWxString(path));
        list_->SetItemData(0, list_->GetItemCount() - 1);
    }

    return true;
}

/**
 * Enables and disables the delete button according to slot list selection.
 */
void
AddRFFromHDBDialog::onListSelectionChange(wxListEvent&) {
    if (list_->GetSelectedItemCount() == 1) {
	FindWindow(ID_ADD)->Enable();
    } else {
	FindWindow(ID_ADD)->Disable();
    }
}


/**
 * Adds a new register file to the machine when "Add" button is pressed.
 */
void
AddRFFromHDBDialog::onAdd(wxCommandEvent&) {

    long item = -1;
    item = list_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( item == -1 ) return;
    
    int id = list_->GetItemData(item);
    const RFArchitecture* arch =
        MapTools::valueForKey<RFArchitecture*>(rfArchitectures_, id);

    Machine* machine = model_->getMachine();

    int size = DEFAULT_SIZE;
    int width = DEFAULT_WIDTH;

    if (!arch->hasParameterizedWidth()) {
        size = arch->width();
    }

    if (!arch->hasParameterizedSize()) {
        size = arch->size();
    }

    // Generate name for the new register file.
    string name =
        "RF_" + Conversion::toString(size) + "x" + Conversion::toString(width);

    string rfName = name;
    int idx = 1;
    while (machine->registerFileNavigator().hasItem(rfName)) {
        rfName = name + "_" + Conversion::toString(idx);
        idx++;
    }

    RegisterFile* rf = new RegisterFile(
        rfName, size, width, arch->maxReads(), arch->maxWrites(), 
        arch->guardLatency(), RegisterFile::NORMAL);

    for (int i = 0; i < arch->readPortCount(); i++) {
        string name = "r" + Conversion::toString(i);
        new RFPort(name, *rf);
    }

    for (int i = 0; i < arch->writePortCount(); i++) {
        string name = "w" + Conversion::toString(i);
        new RFPort(name, *rf);
    }

    for (int i = 0; i < arch->bidirPortCount(); i++) {
        string name = "rw" + Conversion::toString(i);
        new RFPort(name, *rf);
    }

    model_->pushToStack();
    machine->addRegisterFile(*rf);
    model_->notifyObservers();
    
}

/**
 * Closes the dialog when the close button is pressed.
 */
void
AddRFFromHDBDialog::onClose(wxCommandEvent&) {
    Close();
}


/**
 * Creates the dialog contents.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return Top level sizer of the dialog contents.
 */
wxSizer*
AddRFFromHDBDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, ID_ADD, wxT("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item3 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_CLOSE, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
