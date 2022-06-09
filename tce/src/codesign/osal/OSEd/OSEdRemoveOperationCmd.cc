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
 * @file OSEdRemoveOperationCmd.cc
 *
 * Definition of OSEdRemoveOperationCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdRemoveOperationCmd.hh"
#include "OSEdConstants.hh"
#include "OSEdTextGenerator.hh"
#include "OperationContainer.hh"
#include "ConfirmDialog.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "OSEd.hh"
#include "OperationModule.hh"
#include "OperationSerializer.hh"
#include "OSEdTreeView.hh"
#include "Operation.hh"
#include "OperationIndex.hh"
#include "OSEdInfoView.hh"
#include "TCEString.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdRemoveOperationCmd::OSEdRemoveOperationCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_REMOVE_OPERATION, NULL) {
}

/**
 * Destructor.
 */
OSEdRemoveOperationCmd::~OSEdRemoveOperationCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdRemoveOperationCmd::id() const {
    return OSEdConstants::CMD_REMOVE_OPERATION;
}

/**
 * Creates a new command.
 *
 * @return New command. 
 */
GUICommand*
OSEdRemoveOperationCmd::create() const {
    return new OSEdRemoveOperationCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdRemoveOperationCmd::Do() {
    
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    Operation* op = treeView->selectedOperation();
    wxTreeItemId opId = treeView->selectedOperationId();
    string modName = treeView->moduleOfOperation(opId);
    wxTreeItemId modId = treeView->moduleIdOfOperation(opId);
    string pathName = treeView->pathOfModule(modId);

    OperationModule& module = OperationContainer::module(pathName, modName);

    format fmt = texts.text(OSEdTextGenerator::TXT_QUESTION_REMOVE_OPERATION);
    fmt % op->name();
    ConfirmDialog dialog(parentWindow(), WxConversion::toWxString(fmt.str()));

    if (dialog.ShowModal() == wxID_YES) {
        OperationSerializer& serializer = 
            OperationContainer::operationSerializer();
        
        serializer.setSourceFile(module.propertiesModule());
        ObjectState* root = NULL;
        try {
            root = serializer.readState();
            for (int i = 0; i < root->childCount(); i++) {
                ObjectState* child = root->child(i);
                if (child->stringAttribute("name") == op->name()) {
                    delete child;
                    break;
                }
            }
            serializer.setDestinationFile(module.propertiesModule());
            serializer.writeState(root);
            delete root;
        } catch (const Exception& e) {
            fmt = texts.text(
                OSEdTextGenerator::TXT_ERROR_CAN_NOT_REMOVE_OPERATION);
        
            fmt % op->name();
            ErrorDialog error(
                parentWindow(), WxConversion::toWxString(fmt.str()));
            error.ShowModal();
            delete root;
            delete op;
            return false;
        }
        
        OperationIndex& index = OperationContainer::operationIndex();
        index.refreshModule(pathName, modName);
        treeView->removeItem(opId);
        treeView->infoView()->operationView(pathName, modName);
    }
    delete op;
    return true;
}

/**
 * Returns true if command is enabled, otherwise false.
 *
 * Command is enabled when the path of the operation is writable.
 *
 * @return True if command is enabled, otherwise false.
 */
bool
OSEdRemoveOperationCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    if (treeView->isOperationSelected()) {
        wxTreeItemId opId = treeView->selectedOperationId();
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
 * Returns the icon of the command.
 *
 * @return Empty string (icons not used).
 */
string
OSEdRemoveOperationCmd::icon() const {
    return "";
}
