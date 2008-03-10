/**
 * @file OSEdRemoveModuleCmd.cc
 *
 * Definition of OSEdRemoveModuleCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <iostream>

#include "OSEdRemoveModuleCmd.hh"
#include "OSEdConstants.hh"
#include "ConfirmDialog.hh"
#include "OSEdTextGenerator.hh"
#include "OperationContainer.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "OperationModule.hh"
#include "OSEd.hh"
#include "OSEdTreeView.hh"
#include "OSEdInfoView.hh"
#include "OperationIndex.hh"

using std::string;
using boost::format;
using std::cout;
using std::endl;

/**
 * Constructor.
 */
OSEdRemoveModuleCmd::OSEdRemoveModuleCmd() :
    GUICommand(OSEdConstants::CMD_NAME_REMOVE_MODULE, NULL) {
}

/**
 * Destructor.
 */
OSEdRemoveModuleCmd::~OSEdRemoveModuleCmd() {
}

/**
 * Returns the id of the command.
 * 
 * @return Id of the command.
 */
int
OSEdRemoveModuleCmd::id() const {
    return OSEdConstants::CMD_REMOVE_MODULE;
}

/**
 * Returns a new command.
 *
 * @return A new command.
 */
GUICommand*
OSEdRemoveModuleCmd::create() const {
    return new OSEdRemoveModuleCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdRemoveModuleCmd::Do() {
    
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    string modName = treeView->selectedModule();
    wxTreeItemId modId = treeView->selectedModuleId();
    string path = treeView->pathOfModule(modId);
    
    OperationModule& module = OperationContainer::module(path, modName);

    format fmt = texts.text(OSEdTextGenerator::TXT_QUESTION_REMOVE_MODULE);
    fmt % modName;
    ConfirmDialog dialog(parentWindow(), WxConversion::toWxString(fmt.str()));
    
    if (dialog.ShowModal() == wxID_YES) {
        bool removed = 
            FileSystem::removeFileOrDirectory(module.propertiesModule());
        assert(removed == true);

        if (module.hasBehaviorSource()) {
            fmt = texts.text(
                OSEdTextGenerator::TXT_QUESTION_REMOVE_BEHAVIOR_FILE);
            
            fmt % FileSystem::fileOfPath(module.behaviorSourceModule());
            
            wxString confText = WxConversion::toWxString(fmt.str());
            ConfirmDialog confirm(parentWindow(), confText);
        
            if (confirm.ShowModal() == wxID_YES) {
                FileSystem::removeFileOrDirectory(module.behaviorSourceModule());
            }
        }

        if (module.definesBehavior()) {
            FileSystem::removeFileOrDirectory(module.behaviorModule());
        }
        
        OperationIndex& index = OperationContainer::operationIndex();
        index.removeModule(path, modName);
        treeView->removeItem(modId);
        treeView->infoView()->moduleView(path);
    }
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * @return True if command is enabled.
 */
bool
OSEdRemoveModuleCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    if (treeView->isModuleSelected()) {
        wxTreeItemId modId = treeView->selectedModuleId();
        string path = treeView->pathOfModule(modId);
        if (path != "") {
            return FileSystem::fileIsWritable(path);
        } else {
            return false;
        }
    } else {
        return false;
    }

    assert(false);
    return false;
}

/**
 * Returns icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdRemoveModuleCmd::icon() const {
    return "";
}
