/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file OSEd.cc
 *
 * Definition of OSEd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include <vector>
#include <boost/format.hpp>

#include "OSEd.hh"
#include "OSEdConstants.hh"
#include "OSEdOptionsSerializer.hh"
#include "WxConversion.hh"
#include "ResultDialog.hh"
#include "OSEdTextGenerator.hh"
#include "OperationContainer.hh"
#include "Environment.hh"
#include "OSEdTreeView.hh"
#include "FileSystem.hh"

using std::cerr;
using std::endl;
using std::string;
using std::vector;
using boost::format;

IMPLEMENT_APP(OSEd)

/**
 * Constructor.
 */
OSEd::OSEd() : mainFrame_(NULL), options_(NULL) {
}

/**
 * Destructor.
 */
OSEd::~OSEd() {
}

/**
 * Initializes the application.
 *
 * Creates mainframe as well as reads the configure file. 
 *
 * @return True.
 */
bool
OSEd::OnInit() {
	
    mainFrame_ = new OSEdMainFrame(_T("Operation Set Editor"),
                                   wxPoint(50, 50), wxSize(900, 500));
    
    mainFrame_->updateMenuBar();
    
    options_ = new OSEdOptions();
    OSEdOptionsSerializer serializer;
    
    string confFile = Environment::confDirPath(OSEdConstants::CONF_FILE_NAME);
    
    serializer.setSourceFile(confFile);
    
    try {
        options_->loadState(serializer.readState());
    } catch (const Exception& e) {
        cerr << e.errorMessage() << endl
             << "Default options will be used." << endl;
        createDefaultOptions();
    }

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();

    OSEdTreeView* treeView = mainFrame_->treeView();
    vector<string> results = treeView->constructTree();
    // if some errors occurred while buildin the tree view, error message is
    // shown
    if (results.size() > 0) {
        format fmt = texts.text(OSEdTextGenerator::TXT_XML_RESULT_DIALOG_TITLE);
        ResultDialog result(mainFrame_, results, fmt.str());
        result.ShowModal();
    }

    mainFrame_->Show(true);
    SetTopWindow(mainFrame_);

    return true;    
}

/**
 * Deletes the application level dynamic objects not handled by wxWindows.
 *
 * This function is called when application is about to exit.
 *
 * @return 0.
 */
int
OSEd::OnExit() {
    OSEdTextGenerator::destroy();
    OperationContainer::destroy();
    delete options_;
    options_ = NULL;
    return 0;
}

/**
 * Returns the main frame of the application.
 *
 * @return The main frame of the application.
 */
OSEdMainFrame*
OSEd::mainFrame() const {
    return mainFrame_;
}

/**
 * Returns the options of the application.
 *
 * @return The options of the application.
 */
OSEdOptions*
OSEd::options() const {
	return options_;
}

/**
 * Creates the default options and writes them to the file.
 */
void
OSEd::createDefaultOptions() {
    options_->setEditor(OSEdConstants::DEFAULT_EDITOR);
    OSEdOptionsSerializer serializer;
    Environment::setNewConfFile(OSEdConstants::CONF_FILE_NAME);
    string confFile = 
        Environment::confDirPath(OSEdConstants::CONF_FILE_NAME);
    serializer.setDestinationFile(confFile);
    try {
        serializer.writeState(options_->saveState());
    } catch (const Exception& e) {
        cerr << "Writing options failed: " << e.errorMessage() << endl;
    }
}
