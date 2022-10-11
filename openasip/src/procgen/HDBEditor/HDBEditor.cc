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
 * @file HDBEditor.cc
 *
 * Definition of HDBEditor class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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

    Application::initialize(argc, WxConversion::toCStringArray(argc, argv));
	
    mainFrame_ = new HDBEditorMainFrame(_T("HDB Editor"),
                                        wxPoint(50, 50), wxSize(900, 500));
    
    // parse command line
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
#if wxCHECK_VERSION(3,0,0)
        { wxCMD_LINE_PARAM, "", "", "file", wxCMD_LINE_VAL_STRING,
          wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE, "", "", "", wxCMD_LINE_VAL_STRING, 0}
#else
        { wxCMD_LINE_PARAM, _T(""), _T(""), _T("file"), wxCMD_LINE_VAL_STRING,
          wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE, _T(""), _T(""), _T(""), wxCMD_LINE_VAL_STRING, 0}
#endif
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
