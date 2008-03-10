/**
 * @file HDBEditor.cc
 *
 * Definition of HDBEditor class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
