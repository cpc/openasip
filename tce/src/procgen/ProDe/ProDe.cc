/**
 * @file ProDe.cc
 *
 * Implementation of the ProDe class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include <wx/docmdi.h>
#include <wx/mdi.h>
#include <wx/docview.h>
#include <wx/cmdline.h>
#include <wx/imagpng.h>
#include <wx/splash.h>
#include <boost/format.hpp>

#include "ProDe.hh"
#include "KeyboardShortcut.hh"
#include "ToolbarButton.hh"
#include "ProDeOptions.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "MainFrame.hh"
#include "CommandRegistry.hh"
#include "ProDeConstants.hh"
#include "Conversion.hh"
#include "ProDeOptionsSerializer.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "ProDeClipboard.hh"
#include "ProDeTextGenerator.hh"
#include "UserManualCmd.hh"

using std::string;
using std::cerr;
using std::endl;

IMPLEMENT_APP(ProDe)

/**
 * The constructor.
 */
ProDe::ProDe(): docManager_((wxDocManager*)NULL), options_(NULL) {
}



/**
 * Parses the command line and initializes the editor accordingly.
 *
 * Called only once by wxWindows when the program is executed. Command
 * line is parsed using wxCmdLineParser class. If no script is provided
 * with a "-s" option, main frame is created and set as the main
 * window. An instance of the wxDocManager will be created to handle
 * document templates, if the editor is ran in interactive mode.
 *
 * @return true if application was succesfully initialized, false otherwise.
 */
bool
ProDe::OnInit() {

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

    // create a document template for ADF files.
    docManager_ = new wxDocManager;
    (void) new wxDocTemplate((wxDocManager*) docManager_,
			     _T("Architecture Definition"),
                             _T("*.adf;*.xml;*.cfg"), _T(""),
			     _T("adf"), _T("Architecture Definition File"),
			     _T("ADF View"), CLASSINFO(MDFDocument),
			     CLASSINFO(MDFView));

    // Dummy doc template for cfgs.
    // Older versions of wxWidgets don't handle the *.adf;*.cfg file filter
    // correctly and .cfgs don't get associated to the correct document class
    // without this invisble template.
    (void) new wxDocTemplate((wxDocManager*) docManager_,
			     _T("Processor Configuration"),
                             _T("*.cfg"), _T(""),
			     _T("cfg"), _T("Architecture Definition File"),
			     _T("ADF View"), CLASSINFO(MDFDocument),
			     CLASSINFO(MDFView), wxTEMPLATE_INVISIBLE);

    // Dummy doc template for xmls.
    (void) new wxDocTemplate((wxDocManager*) docManager_,
			     _T("Architecture Definition File"),
                             _T("*.xml"), _T(""),
			     _T("xml"), _T("Architecture Definition File"),
			     _T("ADF View"), CLASSINFO(MDFDocument),
			     CLASSINFO(MDFView), wxTEMPLATE_INVISIBLE);
    
    // create a registry of commands
    commandRegistry_ = new CommandRegistry();

    // load image handler for pngs
    wxImage::AddHandler(new wxPNGHandler);

    // set configurations
    string configFile = Environment::confDirPath("") + "ProDe.conf";

    // Name of the CML Schema for options file.
    string schemaFile =
        Environment::dataDirPath("ProDe")+
        FileSystem::DIRECTORY_SEPARATOR + "confschema.xsd";

    ProDeOptionsSerializer reader;
    reader.setSourceFile(configFile);
    reader.setSchemaFile(schemaFile);
    reader.setUseSchema(true);
    bool erroneousOptions = false;
    try {
        ObjectState* optionsState = reader.readState();
        options_ = new ProDeOptions(optionsState);
        delete optionsState;
        options_->validate();
        options_->setFileName(configFile);
    } catch (Exception& e) {
        cerr << e.errorMessage() << endl
             << "Default options will be used." << endl;
        erroneousOptions = true;
    }

    if (erroneousOptions) {
        createDefaultOptions();
    } else {
        options_->clearModified();
    }

    // create the main frame window
    mainFrame_ =
	new MainFrame((wxDocManager*) docManager_, (wxFrame*) NULL,
		      ProDeConstants::EDITOR_NAME,
		      wxPoint(options_->xPosition(), options_->yPosition()),
		      wxSize(options_->windowWidth(),
			     options_->windowHeight()),
		      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

    mainFrame_->Show(TRUE);
    SetTopWindow(mainFrame_);

    // open documents given in command line
    for (unsigned int i = 0; i < parser.GetParamCount(); i++) {
        wxString file = parser.GetParam(i);
        wxDocument* doc = docManager_->CreateDocument(file,  wxDOC_SILENT);
        if (doc == NULL) {
            string fileName = WxConversion::toString(file);
            if (FileSystem::fileExists(fileName)) {
                std::cerr << "Improper file '"
                          << fileName
                          << "' couldn't be opened."
                          << std::endl;
            } else {
                std::cerr << "Cannot open file '"
                          << fileName
                          << "'."
                          << std::endl;
            }
        }
    }

    //mainFrame_->updateMenubar();
    //mainFrame_->updateToolbar();

    return true;
}


/**
 * Deletes the application level dynamic objects not handled by wxWindows.
 *
 * Called when the editor is about to exit, and all application
 * windows are already destroyed.
 */
int
ProDe::OnExit() {
    ProDeClipboard::destroy();
    ProDeTextGenerator::destroy();
    delete docManager_;
    delete options_;
    return 0;
}


/**
 * Returns pointer to the main frame of the editor.
 *
 * @return Pointer to the main frame of the editor.
 */
MainFrame*
ProDe::mainFrame() const {
    return mainFrame_;
}


/**
 * Returns editor options.
 *
 * @return Current editor options.
 */
ProDeOptions*
ProDe::options() const {
    return options_;
}


/**
 * Sets the editor options, and deletes old options.
 *
 * @param options ProDeOptions to set as new options.
 */
void
ProDe::setOptions(ProDeOptions* options) {
    if (options_ != NULL) {
	delete options_;
    }
    options_ = options;
    mainFrame_->createToolbar();
}


/**
 * Creates default options.
 */
void
ProDe::createDefaultOptions() {

    // kb shortcut: ctrl-N = New Document
    KeyboardShortcut* scNew = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_NEW_DOC, 0, true, false, int('N'));

    // kb shortcut: ctrl-O = Open Document
    KeyboardShortcut* scOpen = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_OPEN_DOC, 0, true, false, int('O'));

    // kb shortcut: ctrl-S = Save Document
    KeyboardShortcut* scSave = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_SAVE_DOC, 0, true, false, int('S'));

    // kb shortcut: ctrl-W = close document
    KeyboardShortcut* scClose = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_CLOSE_DOC, 0, true, false, int('W'));

    // kb shortcut: ctrl-P = print document
    KeyboardShortcut* scPrint = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_PRINT, 0, true, false, int('P'));

    // kb shortcut: ctrl-Q = quit
    KeyboardShortcut* scQuit = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_QUIT, 0, true, false, int('Q'));


    // kb shortcut: ctrl-C = copy
    KeyboardShortcut* scCopy = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_COPY, 0, true, false, int('C'));

    // kb shortcut: ctrl-X = cut
    KeyboardShortcut* scCut = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_CUT, 0, true, false, int('X'));

    // kb shortcut: ctrl-V = paste
    KeyboardShortcut* scPaste = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_PASTE, 0, true, false, int('V'));

    // kb shortcut: ctrl-Z = undo
    KeyboardShortcut* scUndo = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_UNDO, 0, true, false, int('Z'));

    // kb shortcut: ctrl-Y = redo
    KeyboardShortcut* scRedo = new KeyboardShortcut(
        ProDeConstants::CMD_NAME_REDO, 0, true, false, int('Y'));

    // kb shortcut: F1 = Help
    KeyboardShortcut* scUserManual = new KeyboardShortcut(
        UserManualCmd::COMMAND_NAME, 1, false, false, 0);

    // toolbar buttons
    ToolbarButton* buttonNew = new ToolbarButton(
        0, ProDeConstants::CMD_NAME_NEW_DOC);

    ToolbarButton* buttonOpen =	new ToolbarButton(
        1, ProDeConstants::CMD_NAME_OPEN_DOC);

    ToolbarButton* buttonSave =	new ToolbarButton(
        2, ProDeConstants::CMD_NAME_SAVE_DOC);

    ToolbarButton* buttonDelete = new ToolbarButton(
        4, ProDeConstants::CMD_NAME_DELETE_COMP);

    ToolbarButton* buttonModify = new ToolbarButton(
        5, ProDeConstants::CMD_NAME_MODIFY_COMP);

    ToolbarButton* buttonZoomIn = new ToolbarButton(
        7, ProDeConstants::CMD_NAME_ZOOM_IN);

    ToolbarButton* buttonZoomOut = new ToolbarButton(
        8, ProDeConstants::CMD_NAME_ZOOM_OUT);

    ToolbarButton* buttonZoomFit = new ToolbarButton(
        9, ProDeConstants::CMD_NAME_ZOOM_FIT_WIN);

    ToolbarButton* buttonOptions = new ToolbarButton(
        11, ProDeConstants::CMD_NAME_EDIT_OPTIONS);

    ToolbarButton* buttonHelp = new ToolbarButton(
        12, UserManualCmd::COMMAND_NAME);

    options_ = new ProDeOptions();

    options_->addKeyboardShortcut(scNew);
    options_->addKeyboardShortcut(scOpen);
    options_->addKeyboardShortcut(scSave);
    options_->addKeyboardShortcut(scClose);
    options_->addKeyboardShortcut(scQuit);
    options_->addKeyboardShortcut(scPrint);
    options_->addKeyboardShortcut(scCopy);
    options_->addKeyboardShortcut(scCut);
    options_->addKeyboardShortcut(scPaste);
    options_->addKeyboardShortcut(scUndo);
    options_->addKeyboardShortcut(scRedo);
    options_->addKeyboardShortcut(scUserManual);

    options_->addToolbarButton(buttonNew);
    options_->addToolbarButton(buttonOpen);
    options_->addToolbarButton(buttonSave);
    options_->addToolbarButton(buttonDelete);
    options_->addToolbarButton(buttonModify);
    options_->addToolbarButton(buttonZoomIn);
    options_->addToolbarButton(buttonZoomOut);
    options_->addToolbarButton(buttonZoomFit);
    options_->addToolbarButton(buttonOptions);
    options_->addToolbarButton(buttonHelp);
    options_->addSeparator(3);
    options_->addSeparator(6);
    options_->addSeparator(10);

    // default toolbar layout and visibility
    options_->setToolbarLayout(ProDeOptions::BOTH);
    options_->setToolbarVisibility(true);

    // default undo levels
    options_->setUndoStackSize(5);

    // set MainFrame size and position
    options_->setFullScreen(false);
    options_->setWindowSize(600, 500);
    options_->setWindowPosition(100, 50);
}


/**
 * Returns command registry of the editor.
 */
CommandRegistry*
ProDe::commandRegistry() const {
    return commandRegistry_;
}



/**
 * Returns document manager of the editor.
 */
wxDocManager*
ProDe::docManager() const {
    return docManager_;
}


/**
 * Returns path of toolbar icon files.
 *
 * @return ProDe icon files path.
 */
string
ProDe::bitmapsDirPath() {
    string path = Environment::bitmapsDirPath("ProDe") +
        FileSystem::DIRECTORY_SEPARATOR;
    return path;
}
