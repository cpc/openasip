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
 * @file HDBEditor.cc
 *
 * Definition of HDBEditor class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/cmdline.h>

#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"

IMPLEMENT_APP(HDBEditor)

/**
 * Constructor.
 */
HDBEditor::HDBEditor() : mainFrame_(NULL) {
}

/**
 * Destructor.
 */
HDBEditor::~HDBEditor() {
}

/**
 * Initializes the application.
 *
 * Creates mainframe as well as reads the configure file. 
 *
 * @return True.
 */
bool
HDBEditor::OnInit() {
	
    mainFrame_ = new HDBEditorMainFrame(_T("HDB Editor"),
                                        wxPoint(50, 50), wxSize(900, 500));
    
    // parse command line
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        { wxCMD_LINE_PARAM, _T(""), _T(""), _T("file"), wxCMD_LINE_VAL_STRING,
          wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE, _T(""), _T(""), _T(""), wxCMD_LINE_VAL_STRING, 0}
    };

    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    int parserStatus = parser.Parse();
    if (parserStatus != 0) {
        return false;
    }

    // open documents given in command line
    for (unsigned int i = 0; i < parser.GetParamCount(); i++) {
        wxString file = parser.GetParam(i);
        std::string fileName = WxConversion::toString(file);
        if (!FileSystem::fileExists(fileName)) {
	    std::cerr << "File '" << fileName << "' not found." << std::endl;
        } else {
	    mainFrame_->setHDB(fileName);
        }
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
HDBEditor::OnExit() {
    return 0;
}


/**
 * Returns the main frame of the application.
 *
 * @return The main frame of the application.
 */
HDBEditorMainFrame&
HDBEditor::mainFrame() const {
    return *mainFrame_;
}


/**
 * Returns pointer to the browser window of the application main frame.
 *
 * @return Browser window of the mainframe.
 */
HDBBrowserWindow*
HDBEditor::browser() const {
    return mainFrame_->browser();
}
