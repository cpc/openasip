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
#include "Application.hh"
#include "OSEdOptions.hh"

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

    Application::initialize(argc, WxConversion::toCStringArray(argc, argv));

    mainFrame_ = new OSEdMainFrame(_T("Operation Set Editor"),
                                   wxPoint(50, 50), wxSize(900, 500));
    
    mainFrame_->updateMenuBar();
    
    options_ = new OSEdOptions();
    OSEdOptionsSerializer serializer;
    
    string confFile = Environment::confPath(OSEdConstants::CONF_FILE_NAME);
    serializer.setSourceFile(confFile);
    
    try {
        options_->loadState(serializer.readState());
    } catch (const Exception& e) {
        cerr << "Config file not found, default options will be used." << endl
             << "OseD may use the editor in your $EDITOR or $VISUAL"
             << " environmental variable." << endl
             << "If these point to some non-graphical editor, "
             << "please change the editor from settings." << endl;
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
    string confFile = 
        Environment::userConfPath(OSEdConstants::CONF_FILE_NAME);
    serializer.setDestinationFile(confFile);
    try {
        serializer.writeState(options_->saveState());
    } catch (const Exception& e) {
        cerr << "Writing options failed: " << e.errorMessage() << endl;
    }
}

