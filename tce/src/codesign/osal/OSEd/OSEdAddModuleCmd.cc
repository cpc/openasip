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
 * @file OSEdAddModuleCmd.cc
 *
 * Definition of OSEdAddModuleCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdAddModuleCmd.hh"
#include "OSEdConstants.hh"
#include "OSEd.hh"
#include "ErrorDialog.hh"
#include "OperationContainer.hh"
#include "Application.hh"
#include "AddModuleDialog.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"
#include "OSEdTreeView.hh"
#include "OperationSerializer.hh"
#include "OperationIndex.hh"
#include "OperationModule.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdAddModuleCmd::OSEdAddModuleCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_ADD_MODULE, NULL) {
}

/**
 * Destructor.
 */
OSEdAddModuleCmd::~OSEdAddModuleCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdAddModuleCmd::id() const {
    return OSEdConstants::CMD_ADD_MODULE;
}

/**
 * Creates a new command.
 *
 * @return New command.
 */
GUICommand*
OSEdAddModuleCmd::create() const {
    return new OSEdAddModuleCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdAddModuleCmd::Do() {
	
    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTreeView* treeView = mainFrame->treeView();
    string path = treeView->selectedPath();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    wxTreeItemId pathId = treeView->selectedPathId();
    OperationIndex& index = OperationContainer::operationIndex();
    
    if (!FileSystem::fileExists(path)) {
        if (!FileSystem::createDirectory(path)) {
            format fmt = 
                texts.text(OSEdTextGenerator::TXT_ERROR_CAN_NOT_CREATE_MOD);
            ErrorDialog error(
                parentWindow(), WxConversion::toWxString(fmt.str()));
            error.ShowModal();
            return false;
        }
        index.addPath(path);
        treeView->SetItemBold(pathId);
    }

    AddModuleDialog dialog(parentWindow(), path);
    if (dialog.ShowModal() == wxID_OK) {
             
        OperationModule* module = new OperationModule(dialog.name(), path);
        OperationSerializer& serializer = 
            OperationContainer::operationSerializer();
        serializer.setDestinationFile(module->propertiesModule());
        ObjectState* root = new ObjectState("osal");
        try {
            serializer.writeState(root);
        } catch (const Exception& e) {
            format fmt = texts.text(
                OSEdTextGenerator::TXT_ERROR_CAN_NOT_CREATE_MOD);
        
            ErrorDialog error(parentWindow(), 
                              WxConversion::toWxString(fmt.str()));
            error.ShowModal();
            delete root;
            return false;
        }
        delete root;
        index.addModule(module, path);
        treeView->addItem(pathId, module->name());
        treeView->update();
    }
	
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * @return True if command is enabled.
 */
bool
OSEdAddModuleCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    string path = treeView->selectedPath();
    if (path != "") {
        if (FileSystem::fileExists(path)) {
            string dumbFile = path + FileSystem::DIRECTORY_SEPARATOR +
                "dumb12234";
            if (FileSystem::createFile(dumbFile)) {
                FileSystem::removeFileOrDirectory(dumbFile);
                return true;
            } else {
                return false;
            }
        } else {
            if (FileSystem::createDirectory(path)) {
                FileSystem::removeFileOrDirectory(path);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

/**
 * Return icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdAddModuleCmd::icon() const {
    return "";
}
