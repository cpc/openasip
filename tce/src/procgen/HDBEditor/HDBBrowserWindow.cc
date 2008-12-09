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
 * @file HDBBrowserWindow.cc
 *
 * Implementation of HDBBrowserWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <wx/splitter.h>
#include <wx/progdlg.h>
#include "HDBBrowserWindow.hh"
#include "CachedHDBManager.hh"
#include "WxConversion.hh"
#include "FUImplementation.hh"
#include "FUArchitecture.hh"
#include "RFArchitecture.hh"
#include "FunctionUnit.hh"
#include "FUEntry.hh"
#include "RFEntry.hh"
#include "DBTypes.hh"
#include "HDBBrowserInfoPanel.hh"
#include "RFImplementation.hh"
#include "MapTools.hh"
#include "HDBEditorConstants.hh"
#include "HWOperation.hh"
#include "CostFunctionPlugin.hh"
#include "HDBToHtml.hh"

using namespace HDB;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(HDBBrowserWindow, wxPanel)
    EVT_TREE_SEL_CHANGED(ID_TREE_VIEW, HDBBrowserWindow::onItemSelected)
    EVT_TREE_ITEM_ACTIVATED(ID_TREE_VIEW, HDBBrowserWindow::onItemSelected)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_TREE_VIEW, HDBBrowserWindow::onRMBClick)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the browser panel.
 * @param id Window identifier.
 */
HDBBrowserWindow::HDBBrowserWindow(wxWindow* parent, wxWindowID id) :
    wxPanel(parent, id), manager_(NULL) {

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxVERTICAL);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    tree_ = new wxTreeCtrl(splitter, ID_TREE_VIEW);
    infoPanel_ = new HDBBrowserInfoPanel(splitter, -1);
    splitter->SetMinimumPaneSize(150);
    splitter->SplitVertically(tree_, infoPanel_);
    sizer->Add(splitter, 1, wxGROW);
    Fit();
    SetSizer(sizer);
}


/**
 * The destructor.
 */
HDBBrowserWindow::~HDBBrowserWindow() {
}


/**
 * Sets the HDB which is displayed in the browser.
 *
 * @param manager HDBManager managing the HDB to display.
 */
void
HDBBrowserWindow::setHDBManager(HDB::CachedHDBManager& manager) {
    manager_ = &manager;
    infoPanel_->setHDB(manager);
    update();
}

/**
 * Updates the treeview of the HDB.
 */
void
HDBBrowserWindow::update() {

    fuArchitectures_.clear();
    rfArchitectures_.clear();
    fuImplementations_.clear();
    rfImplementations_.clear();
    fuEntries_.clear();
    rfEntries_.clear();
    busEntries_.clear();
    socketEntries_.clear();
    costPlugins_.clear();

    tree_->DeleteAllItems();
    wxTreeItemId root = tree_->AddRoot(_T("HDB"));

    wxTreeItemId fus = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::FU_ARCHITECTURES));

    wxTreeItemId rfs = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::RF_ARCHITECTURES));

    wxTreeItemId fuEntries = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::FU_ENTRIES));

    wxTreeItemId rfEntries = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::RF_IU_ENTRIES));

    wxTreeItemId busEntries = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::BUS_ENTRIES));

    wxTreeItemId socketEntries = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::SOCKET_ENTRIES));

    wxTreeItemId costPlugins = tree_->AppendItem(
        root, WxConversion::toWxString(HDBToHtml::COST_PLUGINS));

    std::set<RowID>::iterator iter;
 

    std::set<RowID> fuArchIds = manager_->fuArchitectureIDs();
    std::set<RowID> rfArchIds = manager_->rfArchitectureIDs();
    std::set<RowID> fuIDs = manager_->fuEntryIDs();
    std::set<RowID> rfIDs = manager_->rfEntryIDs();
    std::set<RowID> busIDs = manager_->busEntryIDs();
    std::set<RowID> socketIDs = manager_->socketEntryIDs();
    std::set<RowID> pluginIDs = manager_->costFunctionPluginIDs();

    int entries = fuArchIds.size() + rfArchIds.size() + fuIDs.size() +
        rfIDs.size() + busIDs.size() + socketIDs.size() + pluginIDs.size();

    int c = 0;

    wxProgressDialog dialog(
        _T("Loading HDB"), _T("Loading HDB"), entries, this,
        wxPD_APP_MODAL|wxPD_AUTO_HIDE);

    dialog.SetSize(300,100);

    int ids = 0;
    int cur = 0;
    wxString message;

    // FU architectures
    for (iter = fuArchIds.begin(); iter != fuArchIds.end(); iter++) {

        const FUArchitecture& arch = manager_->fuArchitectureByIDConst(*iter);
        const FunctionUnit& fu = arch.architecture();

        wxTreeItemId id = tree_->AppendItem(fus, fuArchLabel(fu));
        fuArchitectures_[*iter] = id;
        c++;
        cur++;
        message = _T("Loading FU Architectures... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    cur = 0;
    ids = rfArchIds.size();
    // RF architectures
    for (iter = rfArchIds.begin(); iter != rfArchIds.end(); iter++) {
        wxString archLabel = WxConversion::toWxString(*iter);
        wxTreeItemId id = tree_->AppendItem(rfs, archLabel);
        rfArchitectures_[*iter] = id;
        c++;
        cur++;
        message = _T("Loading RF Architectures... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    // FU Entries
    cur = 0;
    ids = fuIDs.size();
    for (iter = fuIDs.begin(); iter != fuIDs.end(); iter++) {
        const FUEntry* fuEntry = manager_->fuByEntryID(*iter);
        wxString fuEntryLabel = WxConversion::toWxString(*iter);
        wxTreeItemId entryTreeID = tree_->AppendItem(fuEntries, fuEntryLabel);
        fuEntries_[*iter] = entryTreeID;

        if (fuEntry->hasArchitecture()) {
            const FUArchitecture& arch = fuEntry->architecture();

            int archID = arch.id();
            wxTreeItemId archTreeID = fuArchitectures_[archID];

            if (fuEntry->hasImplementation()) {
                const FUImplementation& imp = fuEntry->implementation();
                wxString impName = WxConversion::toWxString(imp.moduleName());
                wxTreeItemId id = tree_->AppendItem(archTreeID, impName);
                fuImplementations_[imp.id()] = id;
            }
        }

        delete fuEntry;
        c++;
        cur++;
        message = _T("Loading FU entries... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    // RF Entries
    cur = 0;
    ids = rfIDs.size();
    for (iter = rfIDs.begin(); iter != rfIDs.end(); iter++) {
        const RFEntry* rfEntry = manager_->rfByEntryID(*iter);
        wxString rfEntryLabel = WxConversion::toWxString(*iter);
        wxTreeItemId entryTreeID = tree_->AppendItem(rfEntries, rfEntryLabel);
        rfEntries_[*iter] = entryTreeID;

        if (rfEntry->hasArchitecture()) {
            const RFArchitecture& arch = rfEntry->architecture();
            int archID = arch.id();
            wxTreeItemId archTreeID = rfArchitectures_[archID];

            if (rfEntry->hasImplementation()) {
                const RFImplementation& imp = rfEntry->implementation();
                wxString impName = WxConversion::toWxString(imp.moduleName());
                wxTreeItemId id = tree_->AppendItem(archTreeID, impName);
                rfImplementations_[imp.id()] = id;
            }
        }
        delete rfEntry;
        c++;
        cur++;
        message = _T("Loading RF entries... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    // Bus Entries
    cur = 0;
    ids = busIDs.size();
    for (iter = busIDs.begin(); iter != busIDs.end(); iter++) {
        wxString busEntryLabel = WxConversion::toWxString(*iter);
        wxTreeItemId entryTreeID =
            tree_->AppendItem(busEntries, busEntryLabel);

        busEntries_[*iter] = entryTreeID;
        c++;
        cur++;
        message = _T("Loading bus entries... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    // Socket Entries
    cur = 0;
    ids = socketIDs.size();
    for (iter = socketIDs.begin(); iter != socketIDs.end(); iter++) {
        wxString socketEntryLabel = WxConversion::toWxString(*iter);
        wxTreeItemId entryTreeID =
            tree_->AppendItem(socketEntries, socketEntryLabel);

        socketEntries_[*iter] = entryTreeID;
        cur++;
        message = _T("Loading socket entries... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }
    
    // Cost estimation plugin data
    cur = 0;
    ids = pluginIDs.size();
    for (iter = pluginIDs.begin(); iter != pluginIDs.end(); iter++) {

        const CostFunctionPlugin* plugin =
            manager_->costFunctionPluginByID(*iter);

        wxString pluginName = WxConversion::toWxString(plugin->name());
        wxTreeItemId id = tree_->AppendItem(costPlugins, pluginName);
        costPlugins_[*iter] = id;
        delete plugin;
        c++;
        cur++;
        message = _T("Loading cost estimation plugins... (");
        message.Append(WxConversion::toWxString(cur));
        message.Append(_T(" / "));
        message.Append(WxConversion::toWxString(ids));
        message.Append(_T(")"));
        dialog.Update(c, message);
    }

    tree_->Expand(root);
    infoPanel_->clear();
}

/**
 * Function for checking if a FU architecture is selected in the tree view.
 *
 * @return True, if a FU architecture is selected.
 */
bool
HDBBrowserWindow::isFUArchitectureSelected() {
    return MapTools::containsValue(fuArchitectures_, tree_->GetSelection());
}

/**
 * Function for checking if a RF architecture is selected in the tree view.
 *
 * @return True, if a RF architecture is selected.
 */
bool
HDBBrowserWindow::isRFArchitectureSelected() {
    return MapTools::containsValue(rfArchitectures_, tree_->GetSelection());
}


/**
 * Function for checking if a FU implementation is selected in the tree view.
 *
 * @return True, if a FU implementation is selected.
 */
bool
HDBBrowserWindow::isFUImplementationSelected() {
    return MapTools::containsValue(
        fuImplementations_, tree_->GetSelection());
}


/**
 * Function for checking if a RF implementation is selected in the tree view.
 *
 * @return True, if a RF implementation is selected.
 */
bool
HDBBrowserWindow::isRFImplementationSelected() {
    return MapTools::containsValue(
        rfImplementations_, tree_->GetSelection());
}

/**
 * Function for checking if a FU entry is selected in the tree view.
 *
 * @return True, if a FU entry is selected.
 */
bool
HDBBrowserWindow::isFUEntrySelected() {
    return MapTools::containsValue(fuEntries_, tree_->GetSelection());
}

/**
 * Function for checking if a RF entry is selected in the tree view.
 *
 * @return True, if a RF entry is selected.
 */
bool
HDBBrowserWindow::isRFEntrySelected() {
    return MapTools::containsValue(rfEntries_, tree_->GetSelection());
}

/**
 * Function for checking if a Bus entry is selected in the tree view.
 *
 * @return True, if a bus entry is selected.
 */
bool
HDBBrowserWindow::isBusEntrySelected() {
    return MapTools::containsValue(busEntries_, tree_->GetSelection());
}

/**
 * Function for checking if a socket entry is selected in the tree view.
 *
 * @return True, if a socket entry is selected.
 */
bool
HDBBrowserWindow::isSocketEntrySelected() {
    return MapTools::containsValue(socketEntries_, tree_->GetSelection());
}

/**
 * Function for checking if a cost function plugin is selected in the tree
 * view.
 *
 * @return True, if a cost function plugin is selected.
 */
bool
HDBBrowserWindow::isCostFunctionPluginSelected() {
    return MapTools::containsValue(costPlugins_, tree_->GetSelection());
}


/**
 * Returns rowID of the FU architecture selected in the treeview.
 *
 * @return ID of the selected FU architecture, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedFUArchitecture() {
    if (!isFUArchitectureSelected()) {
        return -1;
    }

    RowID id = MapTools::keyForValue<RowID>(
        fuArchitectures_, tree_->GetSelection());

    return id;
}

/**
 * Returns rowID of the RF architecture selected in the treeview.
 *
 * @return ID of the selected RF architecture, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedRFArchitecture() {
    if (!isRFArchitectureSelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        rfArchitectures_, tree_->GetSelection());

    return id;
}


/**
 * Returns rowID of the FU implementation selected in the treeview.
 *
 * @return ID of the selected FU implementation, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedFUImplementation() {
    if (!isFUImplementationSelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        fuImplementations_, tree_->GetSelection());

    return id;
}


/**
 * Returns rowID of the RF implementation selected in the treeview.
 *
 * @return ID of the selected RF implementation, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedRFImplementation() {
    if (!isRFImplementationSelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        rfImplementations_, tree_->GetSelection());

    return id;
}

/**
 * Returns rowID of the FU entry selected in the tree view.
 *
 * @return ID of the selected FU entry, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedFUEntry() {
    if (!isFUEntrySelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        fuEntries_, tree_->GetSelection());

    return id;
}

/**
 * Returns rowID of the RF entry selected in the tree view.
 *
 * @return ID of the selected RF entry, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedRFEntry() {
    if (!isRFEntrySelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        rfEntries_, tree_->GetSelection());

    return id;
}

/**
 * Returns rowID of the bus entry selected in the tree view.
 *
 * @return ID of the selected bus entry, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedBusEntry() {
    if (!isBusEntrySelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        busEntries_, tree_->GetSelection());

    return id;
}

/**
 * Returns rowID of the socket entry selected in the tree view.
 *
 * @return ID of the selected socket entry, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedSocketEntry() {
    if (!isSocketEntrySelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        socketEntries_, tree_->GetSelection());

    return id;
}


/**
 * Returns rowID of the cost function plugin selected in the treeview.
 *
 * @return ID of the selected cost function plugin, or -1 if none is selected.
 */
RowID
HDBBrowserWindow::selectedCostFunctionPlugin() {
    if (!isCostFunctionPluginSelected()) {
        return -1;
    }
    RowID id = MapTools::keyForValue<RowID>(
        costPlugins_, tree_->GetSelection());

    return id;
}

/**
 * Selects the given FU Entry.
 *
 * @param id RowID of the 
 */
void
HDBBrowserWindow::selectFUEntry(int id) {

    if (!MapTools::containsKey(fuEntries_, id)) {
        return;
    }

    tree_->SelectItem(fuEntries_[id]);
}

/**
 * Selects the given RF Entry.
 *
 * @param id RowID of the RF entry to select.
 */
void
HDBBrowserWindow::selectRFEntry(int id) {

    if (!MapTools::containsKey(rfEntries_, id)) {
        return;
    }

    tree_->SelectItem(rfEntries_[id]);
}

/**
 * Selects the given Bus Entry.
 *
 * @param id RowID of the bus entry to select.
 */
void
HDBBrowserWindow::selectBusEntry(int id) {

    if (!MapTools::containsKey(busEntries_, id)) {
        return;
    }

    tree_->SelectItem(busEntries_[id]);
}

/**
 * Selects the given Socket Entry.
 *
 * @param id RowID of the socket entry to select.
 */
void
HDBBrowserWindow::selectSocketEntry(int id) {

    if (!MapTools::containsKey(socketEntries_, id)) {
        return;
    }

    tree_->SelectItem(socketEntries_[id]);
}

/**
 * Selects the given FU architecture.
 *
 * @param id RowID of the 
 */
void
HDBBrowserWindow::selectFUArchitecture(int id) {

    if (!MapTools::containsKey(fuArchitectures_, id)) {
        return;
    }

    tree_->SelectItem(fuArchitectures_[id]);
}

/**
 * Selects the given RF architecture.
 *
 * @param id RowID of the 
 */
void
HDBBrowserWindow::selectRFArchitecture(int id) {

    if (!MapTools::containsKey(rfArchitectures_, id)) {
        return;
    }

    tree_->SelectItem(rfArchitectures_[id]);
}

/**
 * Selects the given FU implementation.
 *
 * @param id RowID of the FU implementation.
 */
void
HDBBrowserWindow::selectFUImplementation(int id) {

    if (!MapTools::containsKey(fuImplementations_, id)) {
        return;
    }

    tree_->SelectItem(fuImplementations_[id]);
}

/**
 * Selects the given RF implementation.
 *
 * @param id RowID of the RF implementation.
 */
void
HDBBrowserWindow::selectRFImplementation(int id) {

    if (!MapTools::containsKey(rfImplementations_, id)) {
        return;
    }

    tree_->SelectItem(rfImplementations_[id]);
}

/**
 * Selects the given cost function plugin.
 *
 * @param id RowID of the cost function plugin.
 */
void
HDBBrowserWindow::selectCostFunctionPlugin(int id) {

    if (!MapTools::containsKey(costPlugins_, id)) {
        return;
    }

    tree_->SelectItem(costPlugins_[id]);
}

/**
 * Event handler for the tree-view selection changes.
 */
void
HDBBrowserWindow::onItemSelected(wxTreeEvent&) {

    infoPanel_->clear();

    if (isFUArchitectureSelected()) {
        infoPanel_->displayFUArchitecture(selectedFUArchitecture());
    } else  if (isRFArchitectureSelected()) {
        infoPanel_->displayRFArchitecture(selectedRFArchitecture());
    } else if (isFUImplementationSelected()) {
        infoPanel_->displayFUImplementation(selectedFUImplementation());
    } else if (isRFImplementationSelected()) {
        infoPanel_->displayRFImplementation(selectedRFImplementation());
    } else if (isFUEntrySelected()) {
        infoPanel_->displayFUEntry(selectedFUEntry());
    } else if (isRFEntrySelected()) {
        infoPanel_->displayRFEntry(selectedRFEntry());
    } else if (isBusEntrySelected()) {
        infoPanel_->displayBusEntry(selectedBusEntry());
    } else if (isSocketEntrySelected()) {
        infoPanel_->displaySocketEntry(selectedSocketEntry());
    } else if (isCostFunctionPluginSelected()) {
        infoPanel_->displayCostFunctionPlugin(selectedCostFunctionPlugin());
    }
}


/**
 * Event handler for right mouse button clicks in the treeview.
 *
 * Pops up a context menu.
 */
void
HDBBrowserWindow::onRMBClick(wxTreeEvent& event) {
    tree_->SelectItem(event.GetItem());
    wxMenu* popupMenu = new wxMenu();
    if (isFUArchitectureSelected()) {
        popupMenu->Append(
            HDBEditorConstants::COMMAND_ADD_FU_IMPLEMENTATION,
            _T("Add &Implementation..."));
    }
    if (isRFArchitectureSelected()) {
        popupMenu->Append(
            HDBEditorConstants::COMMAND_ADD_RF_IMPLEMENTATION,
            _T("Add &Implementation..."));
    }
    if (isFUEntrySelected() || isRFEntrySelected()) {
        popupMenu->Append(
            HDBEditorConstants::COMMAND_SET_COST_PLUGIN,
            _T("Set Cost Function Plugin"));
    }

    popupMenu->AppendSeparator();
    popupMenu->Append(HDBEditorConstants::COMMAND_MODIFY, _T("&Modify..."));
    popupMenu->Append(HDBEditorConstants::COMMAND_DELETE, _T("&Delete"));
    PopupMenu(popupMenu, event.GetPoint());
}

/**
 * Returns label string for an function unit architecture.
 *
 * @param fu Function unit object to create the label from.
 * @return Label for the fu architecture.
 */
wxString
HDBBrowserWindow::fuArchLabel(const FunctionUnit& fu) const {

    // Operation names are put in a set which sorts them alphbetically.
    std::set<std::string> opNames;
    for (int i = 0; i < fu.operationCount(); i++) {
        const HWOperation* op = fu.operation(i);
        std::string opName =
            op->name() + "(" + Conversion::toString(op->latency()) + ")";
        
        opNames.insert(opName);
    }

    wxString archName;
    std::set<std::string>::const_iterator oIter = opNames.begin();
    for (; oIter != opNames.end(); oIter++) {
        archName.Append(WxConversion::toWxString(*oIter));
        archName.Append(_T(" "));
    }

    return archName;
}

/**
 * Opens a link generated by HDBToHtml in the browser window.
 *
 * @param link Link to open.
 */
void
HDBBrowserWindow::openLink(const wxString& link) {

    const wxString fuArchPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::FU_ARCHITECTURES) +
        _T("/");

    const wxString rfArchPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::RF_ARCHITECTURES) +
        _T("/");

    const wxString fuEntryPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::FU_ENTRIES) + _T("/");

    const wxString rfEntryPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::RF_ENTRIES) + _T("/");

    const wxString busEntryPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::BUS_ENTRIES) + _T("/");

    const wxString socketEntryPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::SOCKET_ENTRIES) +
        _T("/");

    const wxString fuImplPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::FU_IMPLEMENTATIONS) +
        _T("/");

    const wxString rfImplPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::RF_IMPLEMENTATIONS) +
        _T("/");

    const wxString costPluginPrefix =
        _T("/") + WxConversion::toWxString(HDBToHtml::COST_PLUGINS) + _T("/");

    wxString rest;
    if (link.StartsWith(fuArchPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectFUArchitecture(id);
    } else if (link.StartsWith(rfArchPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectRFArchitecture(id);
    } else if (link.StartsWith(fuEntryPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectFUEntry(id);
    } else if (link.StartsWith(rfEntryPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectRFEntry(id);
    } else if (link.StartsWith(busEntryPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectBusEntry(id);
    } else if (link.StartsWith(socketEntryPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectSocketEntry(id);
    } else if (link.StartsWith(fuImplPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectFUImplementation(id);
    } else if (link.StartsWith(rfImplPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectRFImplementation(id);
    } else if (link.StartsWith(costPluginPrefix, &rest)) {
        int id = Conversion::toInt(WxConversion::toString(rest));
        selectCostFunctionPlugin(id);
    }
}
