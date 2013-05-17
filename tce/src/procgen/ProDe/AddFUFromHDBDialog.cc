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
 * @file AddFUFromHDBDialog.cc
 *
 * Implementation of AddFUFromHDBDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/dir.h>

#include "AddFUFromHDBDialog.hh"
#include "Model.hh"
#include "Machine.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "HDBManager.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "WarningDialog.hh"
#include "ErrorDialog.hh"
#include "MapTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "FUPort.hh"
#include "ExecutionPipeline.hh"
#include "HDBRegistry.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;
using namespace TTAMachine;
using namespace HDB;

BEGIN_EVENT_TABLE(AddFUFromHDBDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_LIST, AddFUFromHDBDialog::onListSelectionChange)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, AddFUFromHDBDialog::onListSelectionChange)
    EVT_BUTTON(ID_ADD, AddFUFromHDBDialog::onAdd)
    EVT_BUTTON(ID_CLOSE, AddFUFromHDBDialog::onClose)
END_EVENT_TABLE()


const wxString AddFUFromHDBDialog::HDB_FILE_FILTER = _T("*.hdb");

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent Machine of the immediate slots.
 */
AddFUFromHDBDialog::AddFUFromHDBDialog(
    wxWindow* parent,
    Model* model) :
    wxDialog(
        parent, -1, _T("HDB Function Units"),
        wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    model_(model) {        

    createContents(this, true, true);
    SetSize(400, 300);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));

    list_->InsertColumn(0, _T("Latency"));
    list_->InsertColumn(1, _T("Operations"));
    list_->InsertColumn(2, _T("Impls"));
    list_->InsertColumn(3, _T("HDB ID"));
    list_->InsertColumn(4, _T("HDB"));

    // Disable conditional buttons.
    FindWindow(ID_ADD)->Disable();
}


/**
 * The Destructor.
 */
AddFUFromHDBDialog::~AddFUFromHDBDialog() {
    MapTools::deleteAllValues(fuArchitectures_);
}


/**
 * Transfers data from the HDBs to the dialog list widget.
 */
bool
AddFUFromHDBDialog::TransferDataToWindow() {

    MapTools::deleteAllValues(fuArchitectures_);
    list_->DeleteAllItems();

    HDBRegistry& registry = HDBRegistry::instance();
    registry.loadFromSearchPaths();

    for (int i = 0; i < registry.hdbCount(); i++) {
        loadHDB(registry.hdb(i));
    }

    wxString message;
    for (int i = 0; i < registry.hdbErrorCount(); i++) {
        message.Append(WxConversion::toWxString(registry.hdbErrorMessage(i)));
        message.Append(_T("\n"));
        WarningDialog dialog(this, message);
        dialog.ShowModal();
    }

    if (registry.hdbCount() < 1) {
        wxString message = _T("No HDBs found in HDB search paths.");
        WarningDialog dialog(this, message);
        dialog.ShowModal();
    }

    list_->SetColumnWidth(1, 200);
    list_->SetColumnWidth(4, wxLIST_AUTOSIZE);
    return wxDialog::TransferDataToWindow();
}

/**
 * Loads register files from a HDB to the dialog list.
 *
 * @param manager HDB manager to load.
 * @return True, if the HDB was succesfully loaded.
 */
bool
AddFUFromHDBDialog::loadHDB(const HDBManager& manager) {

    const std::set<RowID> fuArchIDs = manager.fuArchitectureIDs();
    std::set<RowID>::iterator iter = fuArchIDs.begin();

    std::string path = manager.fileName();

    // Read properties of all function units found in the HDB and append
    // data to the dialog FU list widget.
    for (; iter != fuArchIDs.end(); iter++) {

        FUArchitecture* arch = manager.fuArchitectureByID(*iter);
        FunctionUnit& fu = arch->architecture();

        fuArchitectures_.insert(
            std::pair<int, FUArchitecture*>(list_->GetItemCount(), arch));

        list_->InsertItem(0, _T(""));
        if (fu.operationCount() > 0) {
            wxString operations;
            int minLatency = fu.operation(0)->latency();
            int maxLatency = fu.operation(0)->latency();
            for (int i = 0; i < fu.operationCount(); i++) {
                if (i > 0) {
                    operations.Append(_T(", "));
                }
                operations.Append(
                    WxConversion::toWxString(fu.operation(i)->name()));
                operations.Append(_T("("));
                operations.Append(
                    WxConversion::toWxString(fu.operation(i)->latency()));
                operations.Append(_T(")"));

                if (fu.operation(i)->latency() > maxLatency) {
                    maxLatency = fu.operation(i)->latency();
                }
                if (fu.operation(i)->latency() < minLatency) {
                    minLatency = fu.operation(i)->latency();
                }
            }
            wxString latency = WxConversion::toWxString(minLatency);
            if (maxLatency != minLatency) {
                latency.Append(_T(".."));
                latency.Append(WxConversion::toWxString(maxLatency));
            }
                
            list_->SetItem(0, 0, latency);
            list_->SetItem(0, 1, operations);
        }
        list_->SetItem(0, 3, WxConversion::toWxString(*iter));
        list_->SetItem(0, 4, WxConversion::toWxString(path));
        list_->SetItemData(0, list_->GetItemCount() - 1);
    }

    return true;
}

/**
 * Enables and disables the delete button according to slot list selection.
 */
void
AddFUFromHDBDialog::onListSelectionChange(wxListEvent&) {
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
AddFUFromHDBDialog::onAdd(wxCommandEvent&) {

    long item = -1;
    item = list_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( item == -1 ) return;
    
    int id = list_->GetItemData(item);
    const FUArchitecture* arch =
        MapTools::valueForKey<FUArchitecture*>(fuArchitectures_, id);

    // Copy the architecture of the selected function unit from HDB.
    // Copying is done by saving and loading the FU state to an ObjectState
    // object, because FunctionUnit class doesn't have a copy constructor.
    ObjectState* fuState = arch->architecture().saveState();
    FunctionUnit* fu = NULL;
    try {
        fu = new FunctionUnit(fuState);
        delete fuState;
    } catch (Exception& e) {
        wxString message = _T("Error while adding function unit:\n");
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
    }

    int inIndex = 1;
    int outIndex = 1;
    int ioIndex = 1;
    int unusedIndex = 1;

    // Rename ports.
    for (int p = 0; p < fu->portCount(); p++) {
        FUPort* port = dynamic_cast<FUPort*>(fu->port(p));
        bool read = false;
        bool write = false;

        // Check from the operation pipelines if the port is read or written.
        for (int o = 0; o < fu->operationCount(); o++) {
            const HWOperation* operation = fu->operation(o);
            const ExecutionPipeline* pipeline = operation->pipeline();
            for (int cycle = 0; cycle < operation->latency(); cycle++) {
                if (pipeline->isPortRead(*port, cycle)) read = true;
                if (pipeline->isPortWritten(*port, cycle)) write = true;
            }
        }

        // Generate new name for the port according to if it's read/written.
        std::string portName = "";
        if (read && write) {
            portName = "io" + Conversion::toString(ioIndex);
            ioIndex++;
        } else if (read) {
            portName = "in" + Conversion::toString(inIndex);
            inIndex++;
        } else if (write) {
            portName = "out" + Conversion::toString(outIndex);
            outIndex++;
        } else {
            portName = "unused" + Conversion::toString(unusedIndex);
            unusedIndex++;
        }
        if (port->isTriggering()) {
            portName = portName + "t";
        }
        port->setName(portName);
    }

    // Rename the function unit. All operation names are appended to the
    // function unit name.
    std::string name;
    for (int i = 0; i < fu->operationCount(); i++) {
        if (i > 0) name += "_";
        name += fu->operation(i)->name();
    }

    Machine* machine = model_->getMachine();
    std::string fuName = name;
    int i = 1;
    while (machine->functionUnitNavigator().hasItem(fuName)) {        
        fuName = name + "_" + Conversion::toString(i);
        i++;
    }

    fu->setName(fuName);
    model_->pushToStack();
    machine->addFunctionUnit(*fu);
    model_->notifyObservers();

}

/**
 * Closes the dialog when the close button is pressed.
 */
void
AddFUFromHDBDialog::onClose(wxCommandEvent&) {
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
AddFUFromHDBDialog::createContents(
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
