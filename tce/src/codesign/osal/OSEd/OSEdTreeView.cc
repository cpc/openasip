/**
 * @file OSEdTreeView.cc
 *
 * Definition of OSEdTreeView class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdTreeView.hh"
#include "WxConversion.hh"
#include "OperationContainer.hh"
#include "MapTools.hh"
#include "OSEdConstants.hh"
#include "InformationDialog.hh"
#include "OSEd.hh"
#include "GUICommand.hh"
#include "OSEdTextGenerator.hh"
#include "Environment.hh"
#include "DropDownMenu.hh"
#include "OperationModule.hh"
#include "CommandRegistry.hh"
#include "Operation.hh"
#include "OSEdInfoView.hh"
#include "OperationIndex.hh"

using std::string;
using std::vector;
using boost::format;

BEGIN_EVENT_TABLE(OSEdTreeView, wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(OSEdConstants::CMD_TREE_ITEM, OSEdTreeView::onItemSelected)
    EVT_TREE_ITEM_ACTIVATED(OSEdConstants::CMD_TREE_ITEM, OSEdTreeView::onItemClicked)
    EVT_RIGHT_DOWN(OSEdTreeView::onDropDownMenu)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent. Parent window.
 * @param infoView Info view controlled by tree view.
 */
OSEdTreeView::OSEdTreeView(wxWindow* parent, OSEdInfoView* infoView) :
    wxTreeCtrl(parent, OSEdConstants::CMD_TREE_ITEM), infoView_(infoView) {
}

/**
 * Destructor.
 */
OSEdTreeView::~OSEdTreeView() {
}

/**
 * Constructs the tree structure.
 *
 * If errors occurs while constructing the tree structure, the error
 * messages are collected and returned.
 *
 * @return Results of erronous modules.
 */
vector<string>
OSEdTreeView::constructTree() {
   
    vector<string> results;
    OperationIndex& opIndex = OperationContainer::operationIndex();
    OperationSerializer& serializer =
        OperationContainer::operationSerializer();

    wxTreeItemId root = AddRoot(_T("root"));
    vector<string> paths = Environment::osalPaths();
    
    for (size_t i = 0; i < paths.size(); i++) {
        // add path
        string pathName = paths[i];
        wxTreeItemId path =
            AppendItem(root, WxConversion::toWxString(pathName));
        
        if (FileSystem::fileExists(pathName)) {
            SetItemBold(path);
        }
        
        paths_[pathName] = path;
        int modules = 0;
        try {
            modules = opIndex.moduleCount(pathName);
        } catch (const PathNotFound& p) {
            // no modules in this path
            continue;
        }
        for (int j = 0; j < modules; j++) {
            // add module
            OperationModule& mod = opIndex.module(j, pathName);
            wxTreeItemId module =
                AppendItem(path, WxConversion::toWxString(mod.name()));

            modules_.insert(std::pair<std::string, wxTreeItemId>(mod.name(), module));
            
            int operations = 0;

            // test that operation properties can be loaded
            try {
                operations = opIndex.operationCount(mod);
                serializer.setSourceFile(mod.propertiesModule());
                ObjectState* tree = serializer.readState();
                if (tree->childCount() > 0) {
                    Operation temp("", NullOperationBehavior::instance());
                    temp.loadState(tree->child(0));
                }
                delete tree;
            } catch (const Exception& e) {
                results.push_back(mod.propertiesModule() + ":\n");
                results.push_back(e.errorMessage() + "\n");
                results.push_back("\n");
            }
            
            for (int k = 0; k < operations; k++) {
                // add operation
                string opName = opIndex.operationName(k, mod);
                wxTreeItemId oper =
                    AppendItem(module, WxConversion::toWxString(opName));

                //operations_[opIndex.operationName(k, mod)] = oper;
                operations_.insert(std::pair<std::string, wxTreeItemId>(opIndex.operationName(k, mod), oper));
            }
            
        }
    }
    infoView_->pathView();
    return results;
}

/**
 * Handles the event when item is selected on tree.
 *
 * If selected item is a path, path view is shown. If selected
 * item is a module, module view is shown. If selected item is an operation,
 * operation view is shown.
 *
 * @param event The event to be handled.
 */
void
OSEdTreeView::onItemSelected(wxTreeEvent& event) {

    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTextGenerator& text = OSEdTextGenerator::instance();
    wxTreeItemId id = event.GetItem();

    if (isPath(id)) {
        string path = MapTools::keyForValue<string>(paths_, id);
        format fmt = text.text(OSEdTextGenerator::TXT_STATUS_PATH_SELECTED);
        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));
        infoView_->moduleView(path);
    } else if (isModule(id)) {
        string moduleName = MapTools::keyForValue<string>(modules_, id);
        format fmt = text.text(OSEdTextGenerator::TXT_STATUS_MODULE_SELECTED);
        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));
        string pathName = pathOfModule(id);
        infoView_->operationView(pathName, moduleName);
    } else if (isOperation(id)) {

        string opName = MapTools::keyForValue<string>(operations_, id);
        format fmt = 
            text.text(OSEdTextGenerator::TXT_STATUS_OPERATION_SELECTED);

        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));

        string modName = moduleOfOperation(id);
        string pathName = pathOfModule(GetItemParent(id));
        infoView_->operationPropertyView(opName, modName, pathName);
    } else {
        mainFrame->statusBar()->SetStatusText(_T(""));
        // root is selected
        infoView_->pathView();
    }
    wxGetApp().mainFrame()->updateMenuBar();
}

/**
 * Handles the event when item is selected on tree by double-clicking
 * with mouse.
 *
 * Double clicking an operation opens an operation property dialog, where
 * operation properties can be modified.
 * 
 * @param event Event to be handled.
 */
void
OSEdTreeView::onItemClicked(wxTreeEvent& event) {
    wxTreeItemId id = event.GetItem();
    if (isOperation(id)) {
        // operation is clicked, lets open the property dialog
        OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
        CommandRegistry* registry = mainFrame->registry();
        GUICommand* command = 
            registry->createCommand(OSEdConstants::CMD_PROPERTIES);
        command->setParentWindow(mainFrame);
        command->Do();
        delete command;
    }
}

/**
 * Handles the event when right mouse button is clicked.
 *
 * A different drop down menu is showed, depending on where mouse button is 
 * clicked.
 *
 * @param event Event to be handled.
 */
void
OSEdTreeView::onDropDownMenu(wxMouseEvent& event) {
   
    // coordinates of mouse click
    OSEdTextGenerator& text = OSEdTextGenerator::instance();
    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    wxPoint pos = event.GetPosition();
    int flags = wxTREE_HITTEST_ONITEMLABEL;
    wxTreeItemId id = HitTest(pos, flags);
    SelectItem(id);
    DropDownMenu* menu = NULL;
    if (isPath(id)) {

        format fmt = text.text(OSEdTextGenerator::TXT_STATUS_PATH_SELECTED);
        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));
        menu = new DropDownMenu(DropDownMenu::MENU_PATH);
        PopupMenu(menu, pos);
    } else if (isModule(id)) {

        format fmt = text.text(OSEdTextGenerator::TXT_STATUS_MODULE_SELECTED);
        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));
        menu = new DropDownMenu(DropDownMenu::MENU_MODULE);
        PopupMenu(menu, pos);
    } else if (isOperation(id)) {

        format fmt = 
            text.text(OSEdTextGenerator::TXT_STATUS_OPERATION_SELECTED);
        mainFrame->statusBar()->
            SetStatusText(WxConversion::toWxString(fmt.str()));
        menu = new DropDownMenu(DropDownMenu::MENU_OPERATION);
        PopupMenu(menu, pos);
    } else {
        mainFrame->statusBar()->SetStatusText(_T("Cannot open"));
    }
    delete menu;
}

/**
 * Returns true if tree node with given id is a path.
 *
 * @param id Id of the tree node.
 * @return True if id is a path.
 */
bool
OSEdTreeView::isPath(wxTreeItemId id) const {
    return MapTools::containsValue(paths_, id);
}

/**
 * Returns true if tree node with given id is a module.
 *
 * @param id Id of the tree node.
 * @return True if id is a module.
 */
bool
OSEdTreeView::isModule(wxTreeItemId id) const {
    return MapTools::containsValue(modules_, id);
}

/**
 * Return true if tree node with given id is an operation.
 *
 * @param id Id of the tree node.
 * @return True if id is an operation.
 */
bool
OSEdTreeView::isOperation(wxTreeItemId id) const {
    return MapTools::containsValue(operations_, id);
}

/**
 * Returns the selected operation.
 *
 * If no operation is selected, NULL is returned.
 *
 * @return The selected operation.
 */
Operation*
OSEdTreeView::selectedOperation() {
    
    Iter it = operations_.begin();
    string opName = "";
    string modName = "";
    string pathName = "";
    
    // get the name of the selected operation, its module and its path.
    while(it != operations_.end()) {
        if (IsSelected((*it).second)) {
            opName = (*it).first;
            wxTreeItemId opId = (*it).second;
            
            wxTreeItemId moduleId = GetItemParent(opId);
            wxTreeItemId pathId = GetItemParent(moduleId);
            
            modName = WxConversion::toString(GetItemText(moduleId));
            pathName = WxConversion::toString(GetItemText(pathId));
            
            break;
        }
        it++;
    }

    // if no operation was selected in a tree view, let's look from
    // the info view
    if (opName == "") {
        opName = infoView_->selectedOperation();
        if (opName == "") {
            return NULL;
        } else {

            modName = selectedModule();
            assert(modName != "");
            
            pathName = pathOfModule(selectedModuleId());
            assert(pathName != "");
        }
    }

    Operation* op = OperationContainer::operation(pathName, modName, opName);
    return op;
}

/**
 * Returns the id of selected operation.
 * 
 * @throw NotAvailable If no operation is selected.
 * @return The id of selected operation.
 */
wxTreeItemId
OSEdTreeView::selectedOperationId()
    throw (NotAvailable) {
    
    Iter it = operations_.begin();
    // first look from the tree view
    while (it != operations_.end()) {
        if (IsSelected((*it).second)) {
            return (*it).second;
        }
        it++;
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}

/**
 * Returns the module of an operation with a given id.
 *
 * @param id The id of operation.
 * @return The name of the module.
 */
string
OSEdTreeView::moduleOfOperation(wxTreeItemId id) {
    wxTreeItemId modId = GetItemParent(id);
    string moduleName = MapTools::keyForValue<string>(modules_, modId);
    return moduleName;
}

/**
 * Returns the id of the module of the operation with a given id.
 *
 * @param id The id of the operation.
 * @return The id of the module.
 */
wxTreeItemId
OSEdTreeView::moduleIdOfOperation(wxTreeItemId id) {
    return GetItemParent(id);
}

/**
 * Returns the path of the module with given id.
 *
 * @param id The id of the module.
 * @return The name of the path.
 */
string
OSEdTreeView::pathOfModule(wxTreeItemId id) {
    wxTreeItemId pathId = GetItemParent(id);
    std::string path = MapTools::keyForValue<string>(paths_, pathId);
    return path;
}

/**
 * Returns the id of the path in which module with given id belongs to.
 *
 * @param id Id of the module.
 * @return The id of the path.
 */
wxTreeItemId
OSEdTreeView::pathIdOfModule(wxTreeItemId id) {
    return GetItemParent(id);
}

/**
 * Returns the selected path.
 *
 * If not path is selected, an empty string is returned.
 *
 * @return The selected path, or an empty string.
 */
string
OSEdTreeView::selectedPath() {

    Iter it = paths_.begin();
    
    // first look from the tree view
    while (it != paths_.end()) {
        if (IsSelected((*it).second)) {
            return (*it).first;
        }
        it++;
    }

    // then info view
    string path = infoView_->selectedPath();
    return path;
}

/**
 * Returns the id of selected path.
 *
 * @throw NotAvailable If path is not selected.
 * @return Id of selected path.
 */
wxTreeItemId
OSEdTreeView::selectedPathId()
    throw (NotAvailable) {
    
    Iter it = paths_.begin();
    
    // first look from the tree view
    while (it != paths_.end()) {
        if (IsSelected((*it).second)) {
            return (*it).second;
        }
        it++;
    }
    
    // then look from the info view
    string path = infoView_->selectedPath();
    if (path != "") {
        try {
            wxTreeItemId id = paths_[path];
            return id;
        } catch (const KeyNotFound& k) {
            assert(false);
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}

/**
 * Returns the selected module.
 *
 * If no module is selected, an empty string is returned.
 * 
 * @return The selected module or an empty string.
 */
string
OSEdTreeView::selectedModule() {

    IterM it = modules_.begin();

    // first look from the tree view
    while (it != modules_.end()) {
        if (IsSelected((*it).second)) {
            return (*it).first;
        }
        it++;
    }

    // then look from the info view
    string module = infoView_->selectedModule();
    return module;
}

/**
 * Returns the id of selected module.
 *
 * @throw NotAvailable If no module is selected.
 * @return The id of selected module.
 */
wxTreeItemId
OSEdTreeView::selectedModuleId()
    throw (NotAvailable) {

    if (MapTools::containsValue(modules_, GetSelection())) {
        return GetSelection();
    }

    IterM it = modules_.begin();
    // first search the selected module from tree view
    while (it != modules_.end()) {
        if (IsSelected((*it).second)) {
            return (*it).second;
        }
        it++;
    }

    // no module in tree view selected, look from the info view
    string module = infoView_->selectedModule();
    if (module != "") {
        try {
            it = modules_.find(module);
            wxTreeItemId id = it->second;
            return id;
        } catch (const KeyNotFound& k) {
            assert(false);
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}

/**
 * Adds an item to the tree.
 *
 * @param parent Parent id.
 * @param item The item to be added.
 */
void
OSEdTreeView::addItem(wxTreeItemId parent, std::string item) {
    wxTreeItemId id = AppendItem(parent, WxConversion::toWxString(item));
    if (isPath(parent)) {
        modules_.insert(std::pair<std::string, wxTreeItemId>(item, id));
    } else if (isModule(parent)) {
        operations_.insert(std::pair<std::string, wxTreeItemId>(item, id));
        //operations_[item] = id;
    } else {
        assert(false);
    }
}

/**
 * Changes the text of the item.
 *
 * @param id Id of the item to be changed.
 * @param text New label.
 */
void
OSEdTreeView::changeText(wxTreeItemId id, const std::string& text) {
    
    if (isOperation(id)) {
        string key = MapTools::keyForValue<string>(operations_, id);
        Iter it = operations_.find(key);
        operations_.erase(it);
        operations_.insert(std::pair<std::string, wxTreeItemId>(text, id));
        //operations_[text] = id;
    } else if (isModule(id)) {
        string key = MapTools::keyForValue<string>(modules_, id);
        IterM it = modules_.find(key);
        modules_.erase(it);
        modules_.insert(std::pair<std::string, wxTreeItemId>(text, id));
    } else if (isPath(id)) {
        string key = MapTools::keyForValue<string>(paths_, id);
        Iter it = paths_.find(key);
        paths_.erase(it);
        paths_[text] = id;
    }
    SetItemText(id, WxConversion::toWxString(text));
}

/**
 * Removes an item from the tree view.
 *
 * Collapses the parent and set the parent selected.
 *
 * @param id The id of the item to be removed.
 */
void
OSEdTreeView::removeItem(wxTreeItemId id) {

    wxTreeItemId parent = GetItemParent(id);
    Collapse(parent);
    SelectItem(parent);
    DeleteChildren(id);
    Delete(id);

    // remove the item from the inner data structure also
    if (isOperation(id)) {
        string key = MapTools::keyForValue<string>(operations_, id);
        Iter it = operations_.find(key);
        operations_.erase(it);
    } else if (isModule(id)) {
        string key = MapTools::keyForValue<string>(modules_, id);
        IterM it = modules_.find(key);
        modules_.erase(it);
        Iter ot = operations_.begin();
        while (ot != operations_.end()) {
            if (GetItemParent((*ot).second) == id) {
                Iter next = ot;
                next++;
                operations_.erase(ot);
                ot = next;
            } else {
                ot++;
            }
        }
    } else if (isPath(id)) {
        // paths can not be erased
        assert(false);
    }
}

/**
 * Returns pointer to info view.
 *
 * @return Pointer to info view.
 */
OSEdInfoView*
OSEdTreeView::infoView() const {
    return infoView_;
}

/**
 * Updates the treeview.
 */
void
OSEdTreeView::update() {

    Iter it = paths_.begin();
    while (it != paths_.end()) {
        if (IsSelected((*it).second)) {
            infoView_->moduleView((*it).first);
            return;
        }
        it++;
    }

    IterM itm = modules_.begin();
    while (itm != modules_.end()) {
        if (IsSelected((*itm).second)) {
            wxTreeItemId pathId = GetItemParent((*itm).second);
            string pathName = WxConversion::toString(GetItemText(pathId));
            infoView_->operationView(pathName, (*itm).first);
            return;
        }
        itm++;
    }

    it = operations_.begin();
    while (it != operations_.end()) {
        if (IsSelected((*it).second)) {
            wxTreeItemId modId = GetItemParent((*it).second);
            wxTreeItemId pathId = GetItemParent(modId);

            string modName = WxConversion::toString(GetItemText(modId));
            string pathName = WxConversion::toString(GetItemText(pathId));
            
            infoView_->operationPropertyView((*it).first, modName, pathName);
            return;
        }
        it++;
    }

    infoView_->pathView();
}

/**
 * Returns true if a path is selected.
 *
 * @return True, if a path is selected.
 */
bool
OSEdTreeView::isPathSelected() const {
    if (MapTools::containsValue(paths_, GetSelection()) ||
        infoView_->selectedPath() != "") {

        return true;
    }
    return false;
}

/**
 * Returns true if a module is selected.
 *
 * @return True, if a module is selected.
 */
bool
OSEdTreeView::isModuleSelected() const {
    if (MapTools::containsValue(modules_, GetSelection()) ||
        infoView_->selectedModule() != "") {
        wxTreeItemId id = GetSelection();
        return true;
    }
    return false;
}

/**
 * Returns true if an operation is selected.
 *
 * @return True, if an operation is selected.
 */
bool
OSEdTreeView::isOperationSelected() const {
    if (MapTools::containsValue(operations_, GetSelection()) ||
        infoView_->selectedOperation() != "") {

        return true;
    }
    return false;
}

