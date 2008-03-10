/**
 * @file OSEdModifyBehaviorCmd.cc
 *
 * Definition of OSEdModifyBehaviorCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdModifyBehaviorCmd.hh"
#include "OSEdConstants.hh"
#include "OperationContainer.hh"
#include "FileSystem.hh"
#include "OSEd.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "OSEdTextGenerator.hh"
#include "CommandThread.hh"
#include "OperationModule.hh"
#include "OSEdTreeView.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdModifyBehaviorCmd::OSEdModifyBehaviorCmd() :
    GUICommand(OSEdConstants::CMD_NAME_MODIFY_BEHAVIOR, NULL) {
}

/**
 * Destructor.
 */
OSEdModifyBehaviorCmd::~OSEdModifyBehaviorCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdModifyBehaviorCmd::id() const {
    return OSEdConstants::CMD_MODIFY_BEHAVIOR;
}

/**
 * Creates a new command.
 *
 * @return A new command. 
 */
GUICommand*
OSEdModifyBehaviorCmd::create() const {
    return new OSEdModifyBehaviorCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdModifyBehaviorCmd::Do() {

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdOptions* options = wxGetApp().options();
    string editor = options->editor();
    if (editor == "") {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_NO_EDITOR);
        ErrorDialog error(parentWindow(), WxConversion::toWxString(fmt.str()));
        error.ShowModal();
        return false;
    } else if (!FileSystem::fileExists(editor)) {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_OPEN);
        fmt % editor;
        ErrorDialog error(parentWindow(), WxConversion::toWxString(fmt.str()));
        error.ShowModal();
        return false ;
    } else {
    
        OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
        wxTreeItemId opId = treeView->selectedOperationId();
        string modName = treeView->moduleOfOperation(opId);
        wxTreeItemId modId = treeView->moduleIdOfOperation(opId);
        string pathName = treeView->pathOfModule(modId);
        
        OperationModule& module = OperationContainer::module(pathName, modName);
        string cmd = "";
        if (module.hasBehaviorSource()) {
            string code = module.behaviorSourceModule();
            cmd = editor + " " + code;
        } else {
            // new behavior is added for the module
            string code = module.propertiesModule();
            size_t pos = code.find_last_of(".");
            code.erase(pos);
            code += ".cc";

            // copy template file as new file
            string dir = Environment::dataDirPath(
                WxConversion::toString(OSEdConstants::APPLICATION_NAME));
          
            string templateFile = dir + FileSystem::DIRECTORY_SEPARATOR +
                OSEdConstants::BEHAVIOR_TEMPLATE_FILE_NAME;
           
            FileSystem::copy(templateFile, code);
            cmd = editor + " " + code;
        }
        // we are using detached thread that will be automatically cleaned 
        CommandThread* thread = new CommandThread(cmd);
        thread->Create();
        thread->Run();
        return true;
    }

    assert(false);
    return false;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enable when path of the selected operation is writable.
 *
 * @return True if command is enabled.
 */
bool
OSEdModifyBehaviorCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    if (treeView->isOperationSelected()) {
        wxTreeItemId opId = treeView->selectedOperationId();
        string mod = treeView->moduleOfOperation(opId);
        wxTreeItemId modId = treeView->moduleIdOfOperation(opId);
        string path = treeView->pathOfModule(modId);

        if (FileSystem::fileIsWritable(path)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
 * Returns the icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdModifyBehaviorCmd::icon() const {
    return "";
}
