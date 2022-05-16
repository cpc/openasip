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
 * @file AddFUFromHDBDialog.cc
 *
 * Implementation of AddFUFromHDBDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <sstream>

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/textctrl.h>
#include <wx/imaglist.h>
#include <wx/srchctrl.h>

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
#include "ProDeConstants.hh"

#if !wxCHECK_VERSION(3, 0, 0)
    typedef long int wxIntPtr;
#endif

using std::string;
using boost::format;
using namespace TTAMachine;
using namespace HDB;

BEGIN_EVENT_TABLE(AddFUFromHDBDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_LIST, AddFUFromHDBDialog::onListSelectionChange)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, AddFUFromHDBDialog::onListSelectionChange)
    EVT_BUTTON(ID_ADD, AddFUFromHDBDialog::onAdd)
    EVT_BUTTON(ID_CLOSE, AddFUFromHDBDialog::onClose)
    EVT_TEXT(ID_FILTER_TEXTCTRL, AddFUFromHDBDialog::onFilterChange)
    EVT_LIST_COL_CLICK(ID_LIST, AddFUFromHDBDialog::onColumnClick)
    EVT_TIMER(ID_FILTER_TIMER, AddFUFromHDBDialog::onFilterTimeOut)
END_EVENT_TABLE()


const wxString AddFUFromHDBDialog::HDB_FILE_FILTER = _T("*.hdb");

int wxCALLBACK
FUListCompareASC(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData) {

    ListItemData* lid1 = (ListItemData*)item1;
    ListItemData* lid2 = (ListItemData*)item2;
    int sortColumn = (int)sortData;

    if (sortColumn == 0) {
        return lid1->latency - lid2->latency;
    } else if (sortColumn == 1) {
        return lid1->operations.Cmp(lid2->operations);
    } else if (sortColumn == 3) {
        return lid1->hdbId - lid2->hdbId;
    } else if (sortColumn == 4) {
        return lid1->path.Cmp(lid2->path);
    }

    return 0;
}

int wxCALLBACK
FUListCompareDESC(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData) {

    ListItemData* lid1 = (ListItemData*)item1;
    ListItemData* lid2 = (ListItemData*)item2;
    int sortColumn = (int)sortData;

    if (sortColumn == 0) {
        return lid2->latency - lid1->latency;
    } else if (sortColumn == 1) {
        return lid2->operations.Cmp(lid1->operations);
    } else if (sortColumn == 3) {
        return lid2->hdbId - lid1->hdbId;
    } else if (sortColumn == 4) {
        return lid2->path.Cmp(lid1->path);
    }

    return 0;
}

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
    model_(model),
    filterTimer_(this, ID_FILTER_TIMER),
    sortColumn_(1), sortASC_(true) {

    createContents(this, true, true);
    SetSize(500, 400);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));

    list_->InsertColumn(0, _T("Latency"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
    list_->InsertColumn(
                    1, _T("Operations"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
    list_->InsertColumn(2, _T("Impls"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
    list_->InsertColumn(3, _T("HDB ID"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
    list_->InsertColumn(4, _T("HDB"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

    string iconPath =
        Environment::iconDirPath() + FileSystem::DIRECTORY_SEPARATOR;

    wxImageList *imageList = new wxImageList(13, 17);
    imageList->Add(wxIcon(
        WxConversion::toWxString(iconPath + ProDeConstants::ICON_SORT_DESC)));
    imageList->Add(wxIcon(
        WxConversion::toWxString(iconPath + ProDeConstants::ICON_SORT_ASC)));
    list_->SetImageList(imageList, wxIMAGE_LIST_SMALL);

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

    if (filterCtrl_) {
        std::string tmp = WxConversion::toString(filterCtrl_->GetValue());
        std::istringstream rawFilterRules(tmp.c_str());
        filterPatterns_.clear();
        std::string keyword;
        while (rawFilterRules >> keyword) {
            filterPatterns_.push_back(keyword);
        }
    }

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

        if (!acceptToList(path, *arch, filterPatterns_)) {
            continue;
        }

        fuArchitectures_.insert(
            std::pair<int, FUArchitecture*>(list_->GetItemCount(), arch));

        ListItemData* lid = new ListItemData;

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
            lid->latency = minLatency;
            list_->SetItem(0, 1, operations);
            lid->operations = operations;
        }
        list_->SetItem(0, 3, WxConversion::toWxString(*iter));
        lid->hdbId = *iter;
        list_->SetItem(0, 4, WxConversion::toWxString(path));
        lid->path = WxConversion::toWxString(path);
        lid->id = list_->GetItemCount() - 1;
        list_->SetItemData(0, (long)lid);
    }
    // default sorting column is "Operations"
    list_->SortItems(FUListCompareASC, 1);
    setColumnImage(1, sortASC_);

    return true;
}


/**
 * Returns true if the FU architecture should not be viewed.
 *
 * @param filterList The list of all keywords (uppercase), that the FU
 *                   architecture should contain.
 */
bool
AddFUFromHDBDialog::acceptToList(
    const std::string hdbFilePath,
    const HDB::FUArchitecture& arch,
    const std::vector<std::string>& filterList) {

    if (filterList.empty()) {
        return true;
    }

    // Construct string from where the keywords are searched.
    std::string archStr;
    const FunctionUnit& fu = arch.architecture();
    for (int i = 0; i < fu.operationCount(); i++) {
        const HWOperation& oper = *fu.operation(i);

        // Operation with latency as viewed in the list.
        archStr += oper.name()
            + "(" + Conversion::toString(oper.latency()) + ") ";

    }

    for (auto& c : archStr) c = tolower(c);

    for (const string& keyword : filterList) {
        if (keyword.size() > 0 && keyword.front() == '!') {
            if (keyword.size() < 2) {
                continue;
            } else if (archStr.find(keyword.substr(1)) != std::string::npos ||
                hdbFilePath.find(keyword.substr(1)) != std::string::npos) {
                return false;
            }
        } else if (archStr.find(keyword) == std::string::npos &&
            hdbFilePath.find(keyword) == std::string::npos) {
            return false;
        }
    }

    return true;
}


/**
 * Updates FU architecture list view accordingly to new filter rule.
 */
void
AddFUFromHDBDialog::onFilterChange(wxCommandEvent&) {
    if (filterTimer_.Start(250, /*oneShot = */ true)) {
        return;
    }

    // Timer could not be started for some reason.
    // Do filtering immediately instead.
    AddFUFromHDBDialog::TransferDataToWindow();
}


void
AddFUFromHDBDialog::onFilterTimeOut(wxTimerEvent&) {
    AddFUFromHDBDialog::TransferDataToWindow();
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
    
    ListItemData* lid = (ListItemData*)list_->GetItemData(item);
    int id = lid->id;
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

    filterCtrl_ = new wxSearchCtrl(parent,
        ID_FILTER_TEXTCTRL, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    filterCtrl_->SetDescriptiveText(
        wxT("Filter operations or HDBs. '!PATTERN' to exclude."));
    item0->Add(filterCtrl_, 0, wxGROW|wxALL, 5);

    wxButton *item2 = new wxButton( parent, ID_ADD, wxT("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add(item2, 0, wxALIGN_RIGHT | wxALL, 5);

    wxStaticLine *item3 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add(item3, 0, wxGROW | wxALL, 5);

    wxButton *item4 = new wxButton( parent, ID_CLOSE, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add(item4, 0, wxALIGN_RIGHT | wxALL, 5);

    if (set_sizer) {
        parent->SetSizer( item0 );
        if (call_fit) {
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}


/**
 * Sorts HDB FU list according to clicked column.
 */
void
AddFUFromHDBDialog::onColumnClick(wxListEvent& event) {

    int clickedColumn = event.GetColumn();

    if (clickedColumn == sortColumn_) {
        sortASC_ = !sortASC_;
    } else {
        sortASC_ = true;
        setColumnImage(sortColumn_, -1);    // removes arrow from old column
        sortColumn_ = clickedColumn;
    }

    setColumnImage(clickedColumn, sortASC_);

    if (sortASC_) {
        list_->SortItems(FUListCompareASC, clickedColumn);
    } else {
        list_->SortItems(FUListCompareDESC, clickedColumn);
    }
}


/**
 * Sets sorting arrow image on selected column
 *
 * @param col Column index to set the image
 * @param image Image index in wxImageList
 */
void
AddFUFromHDBDialog::setColumnImage(int col, int image) {
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    list_->SetColumn(col, item);
}
