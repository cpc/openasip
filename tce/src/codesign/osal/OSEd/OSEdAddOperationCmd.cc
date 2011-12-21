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
 * @file OSEdAddOperationCmd.cc
 *
 * Definition of OSEdAddOperationCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "OSEdAddOperationCmd.hh"
#include "OSEdConstants.hh"
#include "OSEd.hh"
#include "ErrorDialog.hh"
#include "OperationContainer.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "OperationBehavior.hh"
#include "OperationPropertyDialog.hh"
#include "OSEdTreeView.hh"
#include "OperationSerializer.hh"
#include "OperationIndex.hh"
#include "OperationModule.hh"
#include "Operation.hh"
#include "TCEString.hh"
#include "ObjectState.hh"

using std::string;

/**
 * Constructor.
 */
OSEdAddOperationCmd::OSEdAddOperationCmd() :
    GUICommand(OSEdConstants::CMD_NAME_ADD_OPERATION, NULL) {
}

/**
 * Destructor.
 */
OSEdAddOperationCmd::~OSEdAddOperationCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdAddOperationCmd::id() const {
    return OSEdConstants::CMD_ADD_OPERATION;
}

/**
 * Creates a new command.
 *
 * @return New command.
 */
GUICommand*
OSEdAddOperationCmd::create() const {
    return new OSEdAddOperationCmd();
}

/**
 * Executes the command.
 */
bool
OSEdAddOperationCmd::Do() {
	
    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTreeView* treeView = mainFrame->treeView();
    string modName = treeView->selectedModule();

    wxTreeItemId modId = treeView->selectedModuleId();
    string pathName = treeView->pathOfModule(modId);
    OperationIndex& index = OperationContainer::operationIndex();
    OperationModule& module = OperationContainer::module(pathName, modName);
		
    assert(&module != &NullOperationModule::instance());
	
    OperationPropertyDialog dialog(parentWindow(), NULL, module, pathName);
    
    if (dialog.ShowModal() == wxID_OK) {
        Operation* op = dialog.operation();
        
        OperationSerializer& serializer = 
            OperationContainer::operationSerializer();
        serializer.setDestinationFile(module.propertiesModule());
        ObjectState* root = new ObjectState("");
		
        for (int k = 0; k < index.operationCount(module); k++) {
            
            string opName = index.operationName(k, module);
            Operation* oper = OperationContainer::operation(
                pathName, modName, opName);
            
            root->addChild(oper->saveState());
            delete oper;
        }
        
        root->addChild(op->saveState());
		
        try {
            serializer.writeState(root);
            delete root;
        } catch (const SerializerException& s) {
            wxString msg = WxConversion::toWxString(s.errorMessage());
            ErrorDialog eDialog(parentWindow(), msg);
            eDialog.ShowModal();
            delete root;
            delete op;
            return false;
        }

        index.refreshModule(pathName, modName);
        treeView->addItem(modId, op->name());
        treeView->update();                
    }

    return true;
}

/**
 * Returns true, if command is enabled.
 *
 * Operation can be added, if path is writable.
 *
 * @return True if command is enabled.
 */
bool
OSEdAddOperationCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    string modName = treeView->selectedModule();
    if (modName != "") {
        wxTreeItemId modId = treeView->selectedModuleId();
        string pathName = treeView->pathOfModule(modId);
        string tempFile = pathName + FileSystem::DIRECTORY_SEPARATOR +
            "temp12341";
        if (FileSystem::createFile(tempFile)) {
            FileSystem::removeFileOrDirectory(tempFile);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

/**
 * Returns the icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdAddOperationCmd::icon() const {
    return "";
}
