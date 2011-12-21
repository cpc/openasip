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
 * @file OSEdPropertiesCmd.cc
 *
 * Definition of OSEdPropertiesCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <iostream>

#include "OSEdPropertiesCmd.hh"
#include "OSEdConstants.hh"
#include "OperationPropertyDialog.hh"
#include "OSEd.hh"
#include "Application.hh"
#include "ErrorDialog.hh"
#include "OperationContainer.hh"
#include "FileSystem.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "OSEdTreeView.hh"
#include "OperationSerializer.hh"
#include "OperationIndex.hh"
#include "Operation.hh"
#include "OperationModule.hh"
#include "TCEString.hh"
#include "ObjectState.hh"

using boost::format;
using std::string;
using std::cout;
using std::endl;

/**
 * Constructor.
 */
OSEdPropertiesCmd::OSEdPropertiesCmd() :
    GUICommand(OSEdConstants::CMD_NAME_PROPERTIES, NULL) {
}

/**
 * Destructor.
 */
OSEdPropertiesCmd::~OSEdPropertiesCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdPropertiesCmd::id() const {
    return OSEdConstants::CMD_PROPERTIES;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdPropertiesCmd::create() const {
    return new OSEdPropertiesCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful, false otherwise.
 */
bool
OSEdPropertiesCmd::Do() {

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTreeView* treeView = mainFrame->treeView();
    Operation* op = treeView->selectedOperation();
    string opName = op->name();
    wxTreeItemId opId = treeView->selectedOperationId();
    string modName = treeView->moduleOfOperation(opId);
    wxTreeItemId modId = treeView->moduleIdOfOperation(opId);
    string pathName = treeView->pathOfModule(modId);
    OperationModule& module = OperationContainer::module(pathName, modName);

    assert(&module != &NullOperationModule::instance());
	
    if (!FileSystem::fileIsWritable(module.propertiesModule())) {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_CAN_NOT_MODIFY);
        ErrorDialog error(parentWindow(), WxConversion::toWxString(fmt.str()));
        error.ShowModal();
        delete op;
        return false;
    } else {
        OperationPropertyDialog dialog(parentWindow(), op, module, pathName);
        ObjectState* orig = op->saveState();
        if (dialog.ShowModal() == wxID_OK) {
		
            ObjectState* mod = op->saveState();
            if (*mod != *orig) {
                // write operation properties to file
                OperationSerializer& serializer = 
                    OperationContainer::operationSerializer();
                serializer.setSourceFile(module.propertiesModule());
                ObjectState* root = NULL;
                try {
                    root = serializer.readState();
                    for (int i = 0; i < root->childCount(); i++) {
                        ObjectState* child = root->child(i);
                        if (child->stringAttribute("name") == opName) {
                            root->replaceChild(child, op->saveState());
                            break;
                        }
                    }
                    serializer.setDestinationFile(module.propertiesModule());
                    serializer.writeState(root);
                } catch (const Exception& s) {
                    format fmt = 
                        texts.text(OSEdTextGenerator::TXT_ERROR_CAN_NOT_MODIFY);
                    ErrorDialog eDialog(
                        parentWindow(), WxConversion::toWxString(fmt.str()));
                    eDialog.ShowModal();
                }
                delete root;
			
                // update the tree.
                if (opName != op->name()) {
                    OperationIndex& index = OperationContainer::operationIndex();
                    index.refreshModule(pathName, modName);
                    treeView->changeText(opId, op->name());
                }
            
                treeView->update();
            }
            delete mod;
        }
        delete orig;
        delete op;
        return true;
    }
    
    assert(false);
    return false;
}

/**
 * Returns true, if command is enabled.
 *
 * @return True, if command is enabled.
 */
bool
OSEdPropertiesCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    Operation* op = treeView->selectedOperation();
    if (op != NULL) {
        delete op;
        return true;
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
OSEdPropertiesCmd::icon() const {
    return "";
}
