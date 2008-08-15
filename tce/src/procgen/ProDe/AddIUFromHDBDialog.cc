/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file AddIUFromHDBDialog.cc
 *
 * Implementation of AddIUFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/dir.h>

#include "AddIUFromHDBDialog.hh"
#include "Model.hh"
#include "Machine.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "RFArchitecture.hh"
#include "WarningDialog.hh"
#include "MapTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using std::string;
using boost::format;
using namespace TTAMachine;
using namespace HDB;

BEGIN_EVENT_TABLE(AddIUFromHDBDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_LIST, AddIUFromHDBDialog::onListSelectionChange)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, AddIUFromHDBDialog::onListSelectionChange)
    EVT_BUTTON(ID_ADD, AddIUFromHDBDialog::onAdd)
    EVT_BUTTON(ID_CLOSE, AddIUFromHDBDialog::onClose)
END_EVENT_TABLE()


const int AddIUFromHDBDialog::DEFAULT_SIZE = 1;
const int AddIUFromHDBDialog::DEFAULT_WIDTH = 32;
const Machine::Extension AddIUFromHDBDialog::DEFAULT_EXTENSION_MODE =
    Machine::ZERO;

const wxString AddIUFromHDBDialog::HDB_FILE_FILTER = _T("*.hdb");

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent Machine of the immediate slots.
 */
AddIUFromHDBDialog::AddIUFromHDBDialog(
    wxWindow* parent,
    Model* model) :
    wxDialog(
        parent, -1, _T("HDB Immediate Units"),
        wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    model_(model) {        

    createContents(this, true, true);
    SetSize(400, 300);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));

    list_->InsertColumn(0, _T("Width"));
    list_->InsertColumn(1, _T("Size"));
    list_->InsertColumn(2, _T("Read ports"));
    //list_->InsertColumn(5, _T("Max Reads"));
    //list_->InsertColumn(6, _T("Max RW"));
    list_->InsertColumn(3, _T("Latency"));
    list_->InsertColumn(4, _T("ID"));
    list_->InsertColumn(5, _T("HDB"));

    // Disable conditional buttons.
    FindWindow(ID_ADD)->Disable();
}


/**
 * The Destructor.
 */
AddIUFromHDBDialog::~AddIUFromHDBDialog() {
    MapTools::deleteAllValues(iuArchitectures_);
}


/**
 * Transfers data from the HDBs to the dialog list.
 *
 * @return True, if the the data transfer was succesful.
 */
bool
AddIUFromHDBDialog::TransferDataToWindow() {

    MapTools::deleteAllValues(iuArchitectures_);
    list_->DeleteAllItems();


    const std::vector<string> hdbPaths = Environment::hdbPaths();
    std::vector<string>::const_iterator hdbIter = hdbPaths.begin();

    wxString paths;
    bool hdbsFound = false;

    // Read all hdb files found in the search directories.
    for(; hdbIter != hdbPaths.end(); hdbIter++) {

        wxString file;
        wxString hdbPath = WxConversion::toWxString(*hdbIter);
        paths.Append(hdbPath);
        paths.Append(_T("\n"));
        wxDir dir;

        if (dir.Exists(hdbPath) &&
            dir.Open(hdbPath) &&
            dir.GetFirst(&file, HDB_FILE_FILTER)) {

            do {
                string path = *hdbIter + FileSystem::DIRECTORY_SEPARATOR +
                    WxConversion::toString(file);

                if (loadHDB(path)) {
                    hdbsFound = true;
                }
            } while (dir.GetNext(&file));

        }

    }

    if (!hdbsFound) {
        wxString message = _T("No HDBs found in HDB search paths:\n");
        message.Append(paths);
        WarningDialog dialog(this, message);
        dialog.ShowModal();
    }

    list_->SetColumnWidth(5, wxLIST_AUTOSIZE);
    return wxDialog::TransferDataToWindow();
}

/**
 * Loads immediate units from a HDB to the dialog list.
 *
 * @param path Full path to the HDB file to load.
 * @return True, if the HDB was succesfully loaded.
 */
bool
AddIUFromHDBDialog::loadHDB(const std::string& path) {

    HDBManager* manager = NULL;

    try {
        manager = &HDBRegistry::instance().hdb(path);
    } catch (Exception& e) {
        wxString message = _T("Unable to open HDB '");
        message.Append(WxConversion::toWxString(path));
        message.Append(_T("':'"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        WarningDialog dialog(this, message);
        dialog.ShowModal();
        return false;
    }

    const std::set<RowID> rfArchIDs = manager->rfArchitectureIDs();
    std::set<RowID>::iterator iter = rfArchIDs.begin();

    // Read properties of all immediate units in the HDB and append
    // data in the list widget.
    for (; iter != rfArchIDs.end(); iter++) {

        RFArchitecture* arch = manager->rfArchitectureByID(*iter);

        if (arch->writePortCount() != 1 ||
            arch->bidirPortCount() != 0) {

            // Register file architecture must contain one read port and
            // no bidirectional ports or it's not listed as an immediate
            // unit architecture.
            continue;
        }

        iuArchitectures_.insert(
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
        list_->SetItem(0, 3, WxConversion::toWxString(arch->latency())); 
        list_->SetItem(0, 4, WxConversion::toWxString(*iter));
        list_->SetItem(0, 5, WxConversion::toWxString(path));
        list_->SetItemData(0, list_->GetItemCount() - 1);
    }

    return true;
}

/**
 * Enables and disables the delete button according to slot list selection.
 */
void
AddIUFromHDBDialog::onListSelectionChange(wxListEvent&) {
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
AddIUFromHDBDialog::onAdd(wxCommandEvent&) {

    long item = -1;
    item = list_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( item == -1 ) return;
    
    int id = list_->GetItemData(item);
    const RFArchitecture* arch =
        MapTools::valueForKey<RFArchitecture*>(iuArchitectures_, id);

    Machine* machine = model_->getMachine();

    // Set size and width.
    int size = DEFAULT_SIZE;
    int width = DEFAULT_WIDTH;

    if (!arch->hasParameterizedWidth()) {
        size = arch->width();
    }

    if (!arch->hasParameterizedSize()) {
        size = arch->size();
    }

    // Generate name for the new immediate unit.
    string name =
        "IU_" + Conversion::toString(size) + "x" + Conversion::toString(width);

    string iuName = name;
    int idx = 1;
    while (machine->immediateUnitNavigator().hasItem(iuName)) {
        iuName = name + "_" + Conversion::toString(idx);
        idx++;
    }

    int maxReads = arch->maxReads();
    int guardLatency = arch->guardLatency();

    // Create an immediate unit object and attach it to the machine.
    ImmediateUnit* iu = new ImmediateUnit(
        iuName, size, width, maxReads, guardLatency, DEFAULT_EXTENSION_MODE);

    for (int i = 0; i < arch->readPortCount(); i++) {
        string name = "r" + Conversion::toString(i);
        new RFPort(name, *iu);
    }

    model_->pushToStack();
    machine->addImmediateUnit(*iu);
    model_->notifyObservers();
    
}

/**
 * Closes the dialog when the close button is pressed.
 */
void
AddIUFromHDBDialog::onClose(wxCommandEvent&) {
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
AddIUFromHDBDialog::createContents(
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
