/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file ProximMainFrame.cc
 *
 * Implementation of ProximMainFrame class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/progdlg.h>
#include <boost/format.hpp>

#include "ProximMainFrame.hh"
#include "ProximConstants.hh"
#include "ErrorDialog.hh"
#include "WarningDialog.hh"
#include "GUICommand.hh"
#include "Proxim.hh"
#include "WxConversion.hh"
#include "ContainerTools.hh"
#include "Exception.hh"
#include "ProximToolbox.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "GUIOptions.hh"

#include "ProximOpenMachineCmd.hh"
#include "ProximOpenProgramCmd.hh"
#include "ProximCDCmd.hh"
#include "ProximRunCmd.hh"
#include "ProximStepICmd.hh"
#include "ProximNextICmd.hh"
#include "ProximResumeCmd.hh"
#include "ProximKillCmd.hh"
#include "ProximQuitCmd.hh"
#include "ProximClearConsoleCmd.hh"
#include "ProximAboutCmd.hh"
#include "ProximNewPortWindowCmd.hh"
#include "ProximNewRegisterWindowCmd.hh"
#include "ProximSimulatorSettingsCmd.hh"
#include "ProximOptionsCmd.hh"
#include "ProximExecuteFileCmd.hh"
#include "HighlightTopExecCountsCmd.hh"
#include "HighlightExecPercentageCmd.hh"
#include "ClearDisassemblyHighlightCmd.hh"
#include "UserManualCmd.hh"
#include "ProximFindCmd.hh"

#include "ConsoleWindow.hh"
#include "ProximDisassemblyWindow.hh"
#include "ProximMemoryWindow.hh"
#include "ProximRegisterWindow.hh"
#include "ProximControlWindow.hh"
#include "ProximPortWindow.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximBreakpointWindow.hh"
#include "ProximSimulatorWindow.hh"
#include "ProximCmdHistoryWindow.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximStopDialog.hh"
#include "ProximSimulationThread.hh"
#include "MemorySystem.hh"
#include "ProximDebuggerWindow.hh"

BEGIN_EVENT_TABLE(ProximMainFrame, wxFrame)
    EVT_KEY_DOWN(ProximMainFrame::onKeyEvent)
    EVT_CLOSE(ProximMainFrame::onClose)
    EVT_MENU_RANGE(ProximConstants::COMMAND_FIRST,
        ProximConstants::COMMAND_LAST,
        ProximMainFrame::onCommandEvent)
    EVT_MENU_RANGE(ProximConstants::COMMAND_TOGGLE_CONSOLE_WIN,
        ProximConstants::COMMAND_TOGGLE_BREAKPOINT_WIN,
        ProximMainFrame::onToggleWindow)
    EVT_MENU(UserManualCmd::COMMAND_ID, ProximMainFrame::onCommandEvent)
    EVT_COMMAND_RANGE(ProximConstants::COMMAND_FIRST,
        ProximConstants::COMMAND_LAST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        ProximMainFrame::onCommandEvent)
    EVT_UPDATE_UI_RANGE(ProximConstants::COMMAND_FIRST,
        ProximConstants::COMMAND_LAST,
        ProximMainFrame::updateCommand)

    EVT_UPDATE_UI_RANGE(ProximConstants::COMMAND_TOGGLE_CONSOLE_WIN,
        ProximConstants::COMMAND_TOGGLE_BREAKPOINT_WIN,
        ProximMainFrame::updateToggleItem)

    EVT_SIMULATOR_START(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_STOP(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_RUN(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_OUTPUT(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_ERROR(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_COMMAND(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_COMMAND_DONE(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_LOADING_PROGRAM(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_PROGRAM_LOADED(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_LOADING_MACHINE(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_MACHINE_LOADED(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_RUNTIME_ERROR(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_RUNTIME_WARNING(0, ProximMainFrame::onSimulatorEvent)
    EVT_SIMULATOR_TERMINATED(0, ProximMainFrame::onSimulatorTerminated)
    EVT_SIMULATOR_RESET(0, ProximMainFrame::onReset)
END_EVENT_TABLE()

/**
 * The constructor.
 *
 * @param title Frame title.
 * @param pos Frame position on screen.
 * @param size Frame size.
 */
ProximMainFrame::ProximMainFrame(
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size):
    wxFrame((wxFrame*)NULL, -1, title, pos, size),
    topSplitter_(NULL), toolbar_(NULL), stopDialogInstantiated_(false),
    isReset_(false) {

    CommandRegistry& cmdRegistry = wxGetApp().commandRegistry();
    cmdRegistry.addCommand(new ProximOpenMachineCmd());
    cmdRegistry.addCommand(new ProximOpenProgramCmd());
    cmdRegistry.addCommand(new ProximCDCmd());
    cmdRegistry.addCommand(new ProximRunCmd());
    cmdRegistry.addCommand(new ProximStepICmd());
    cmdRegistry.addCommand(new ProximNextICmd());
    cmdRegistry.addCommand(new ProximResumeCmd());
    cmdRegistry.addCommand(new ProximKillCmd());
    cmdRegistry.addCommand(new ProximQuitCmd());
    cmdRegistry.addCommand(new ProximClearConsoleCmd());
    cmdRegistry.addCommand(new ProximAboutCmd());
    cmdRegistry.addCommand(new ProximNewPortWindowCmd());
    cmdRegistry.addCommand(new ProximNewRegisterWindowCmd());
    cmdRegistry.addCommand(new ProximSimulatorSettingsCmd());
    cmdRegistry.addCommand(new ProximOptionsCmd());
    cmdRegistry.addCommand(new ProximExecuteFileCmd());
    cmdRegistry.addCommand(new HighlightTopExecCountsCmd());
    cmdRegistry.addCommand(new HighlightExecPercentageCmd());
    cmdRegistry.addCommand(new ClearDisassemblyHighlightCmd());
    cmdRegistry.addCommand(new UserManualCmd());
    cmdRegistry.addCommand(new ProximFindCmd());

    resetMutex_ = new wxMutex();
    resetCondition_ = new wxCondition(*resetMutex_);

    initialize();

}


/**
 * The destructor.
 */
ProximMainFrame::~ProximMainFrame() {
    delete resetCondition_;
}


/**
 * Initializes the frame instance.
 *
 * Creates the sizer managing the window layout, and creates an instance
 * of the default windows.
 */
void
ProximMainFrame::initialize() {
 
    createMenubar();

    wxStatusBar* statusbar = CreateStatusBar();
    int widths[3] = {-1, 200, 200};
    statusbar->SetFieldsCount(3, widths);

    sizer_ = new wxBoxSizer(wxVERTICAL);

    ProximDisassemblyWindow* disasmWindow = new ProximDisassemblyWindow(
        this, ProximConstants::ID_DISASSEMBLY_WINDOW);
    
    dockWindow(disasmWindow);
    dockWindow(new ConsoleWindow(this, ProximConstants::ID_CONSOLE_WINDOW));
    SetSizer(sizer_);

    createToolbar();

    SetSizeHints(400, 300);
}

/**
 * Closes the GUI when the simulator engine is terminated.
 */
void
ProximMainFrame::onSimulatorTerminated(SimulatorEvent&) {
    Destroy();
}


/**
 * Creates the proxim main frame menubar.
 */
void
ProximMainFrame::createMenubar() {

    wxMenu* fileMenu = new wxMenu();
    wxMenu* viewMenu = new wxMenu();
    wxMenu* editMenu = new wxMenu();
    wxMenu* commandMenu = new wxMenu();
    wxMenu* sourceMenu = new wxMenu();
    wxMenu* programMenu = new wxMenu();
    wxMenu* dataMenu = new wxMenu();
    wxMenu* helpMenu = new wxMenu();
    wxMenu* highlightMenu = new wxMenu();

    // File menu
    fileMenu->Append(
        ProximConstants::COMMAND_OPEN_MACHINE,
        menuAccelerator(ProximConstants::COMMAND_OPEN_MACHINE).Prepend(
            _T("Open &Machine...")));

    fileMenu->Append(
        ProximConstants::COMMAND_OPEN_PROGRAM,
        menuAccelerator(ProximConstants::COMMAND_OPEN_PROGRAM).Prepend(
            _T("Open &Program...")));

    fileMenu->Append(
        ProximConstants::COMMAND_CD,
        menuAccelerator(ProximConstants::COMMAND_CD).Prepend(
            _T("&Change Directory...")));

    fileMenu->AppendSeparator();

    fileMenu->Append(
        ProximConstants::COMMAND_QUIT,
        menuAccelerator(ProximConstants::COMMAND_QUIT).Prepend(
            _T("&Quit")));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));

    // View menu  
    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_CONSOLE_WIN,
        _T("&Simulator Console"));

    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_DISASM_WIN,
        _T("Program &Disassembly"));
    
    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_MEMORY_WIN,
        _T("&Memory Window"));
    
    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_CONTROL_WIN,
        _T("Simulator &Control"));

    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_MACHINE_WIN,
        _T("&Machine Window"));

    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_BREAKPOINT_WIN,
        _T("&Breakpoint Window"));

    viewMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_DEBUGGER_WIN,
        _T("&Debugger Window"));

    viewMenu->AppendSeparator();
    viewMenu->Append(
        ProximConstants::COMMAND_NEW_REGISTER_WINDOW,
        menuAccelerator(ProximConstants::COMMAND_NEW_REGISTER_WINDOW).Prepend(
            _T("New &Register Window")));

    viewMenu->Append(
        ProximConstants::COMMAND_NEW_PORT_WINDOW,
        menuAccelerator(ProximConstants::COMMAND_NEW_PORT_WINDOW).Prepend(
            _T("New &Port Window")));

    viewMenu->Check(ProximConstants::COMMAND_TOGGLE_CONSOLE_WIN, true);
    viewMenu->Check(ProximConstants::COMMAND_TOGGLE_DISASM_WIN, true);
    viewMenu->Enable(ProximConstants::COMMAND_TOGGLE_CONSOLE_WIN, false);
    viewMenu->Enable(ProximConstants::COMMAND_TOGGLE_DISASM_WIN, false);

    menuBar->Append(viewMenu, _T("&View"));

    editMenu->Append(
        ProximConstants::COMMAND_SIMULATOR_SETTINGS,
        menuAccelerator(ProximConstants::COMMAND_SIMULATOR_SETTINGS).Prepend(
            _T("Simulator Settings...")));

    editMenu->Append(
        ProximConstants::COMMAND_EDIT_OPTIONS,
        menuAccelerator(ProximConstants::COMMAND_EDIT_OPTIONS).Prepend(
            _T("Options...")));

    // Edit menu
    menuBar->Append(editMenu, _T("&Edit"));

    // Command menu
    commandMenu->Append(
    ProximConstants::COMMAND_CLEAR_CONSOLE,
        menuAccelerator(ProximConstants::COMMAND_CLEAR_CONSOLE).Prepend(
            _T("&Clear Console")));

    commandMenu->Append(
        ProximConstants::COMMAND_EXECUTE_FILE,
        menuAccelerator(ProximConstants::COMMAND_EXECUTE_FILE).Prepend(
            _T("&Execute File...")));

    commandMenu->Append(
        ProximConstants::COMMAND_FIND,
        menuAccelerator(ProximConstants::COMMAND_FIND).Prepend(
            _T("&Find in disassembly...")));

    commandMenu->AppendSeparator();
    commandMenu->AppendCheckItem(
        ProximConstants::COMMAND_TOGGLE_CMD_HISTORY_WIN,
        _T("Command &History Window"));

    menuBar->Append(commandMenu, _T("&Command"));

    // Source menu
    menuBar->Append(sourceMenu, _T("&Source"));
    sourceMenu->Append(17000, _T("&Profile Data"), highlightMenu);

    highlightMenu->Append(
        ProximConstants::COMMAND_CLEAR_DISASSEMBLY_HIGHLIGHT,
        menuAccelerator(
            ProximConstants::COMMAND_CLEAR_DISASSEMBLY_HIGHLIGHT).Prepend(
                _T("None")));

    highlightMenu->Append(
        ProximConstants::COMMAND_HIGHLIGHT_TOP_EXEC_COUNTS,
        menuAccelerator(
            ProximConstants::COMMAND_HIGHLIGHT_TOP_EXEC_COUNTS).Prepend(
                _T("Highlight Top Execution Counts")));

    highlightMenu->Append(
        ProximConstants::COMMAND_HIGHLIGHT_EXEC_PERCENTAGE,
        menuAccelerator(
            ProximConstants::COMMAND_HIGHLIGHT_EXEC_PERCENTAGE).Prepend(
                _T("Highlight Move Executions")));

    // Program menu
    programMenu->Append(
        ProximConstants::COMMAND_RUN,
        menuAccelerator(ProximConstants::COMMAND_RUN).Prepend(_T("&Run")));

    programMenu->AppendSeparator();

    programMenu->Append(
        ProximConstants::COMMAND_STEPI,
        menuAccelerator(ProximConstants::COMMAND_STEPI).Prepend(
            _T("&Step Instruction")));

    programMenu->Append(
        ProximConstants::COMMAND_NEXTI,
        menuAccelerator(ProximConstants::COMMAND_NEXTI).Prepend(
            _T("&Next Instruction")));

    programMenu->Append(
        ProximConstants::COMMAND_RESUME,
        menuAccelerator(ProximConstants::COMMAND_RESUME).Prepend(
            _T("R&esume")));

    programMenu->AppendSeparator();
    programMenu->Append(
        ProximConstants::COMMAND_KILL,
        menuAccelerator(ProximConstants::COMMAND_KILL).Prepend(_T("&Kill")));

    menuBar->Append(programMenu, _T("&Program"));

    // Data menu
    menuBar->Append(dataMenu, _T("&Data"));

    // Help menu
    helpMenu->Append(
        UserManualCmd::COMMAND_ID,
        menuAccelerator(UserManualCmd::COMMAND_ID).Prepend(
            _T("&User Manual...")));

    helpMenu->Append(
        ProximConstants::COMMAND_ABOUT,
        menuAccelerator(ProximConstants::COMMAND_ABOUT).Prepend(
            _T("&About...")));

    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);
    
    updateMemoryWindowMenuItem();
}

/**
 * Creates the main frame toolbar.
 */
void
ProximMainFrame::createToolbar() {

    if (toolbar_ != NULL) {
        delete toolbar_;
        toolbar_ = NULL;
    }

    CommandRegistry& registry = wxGetApp().commandRegistry();
    GUIOptions& options = wxGetApp().options();
    wxString iconPath = WxConversion::toWxString(Environment::iconDirPath());

    toolbar_ = options.createToolbar(this, registry, iconPath);

    SetToolBar(toolbar_);
    toolbar_->Show(options.toolbarVisibility());
}


/**
 * Returns a menu accelerator string for a command.
 *
 * The returned string can be appended to the menu item text to set the
 * keyboard shortcut defined in the options for the command.
 *
 * @param id Command ID.
 * @return Accelerator string.
 */
wxString
ProximMainFrame::menuAccelerator(int id) {

    const CommandRegistry& reg = wxGetApp().commandRegistry();
    const GUIOptions& options = wxGetApp().options();
    std::string cmdName = reg.commandName(id);
    const KeyboardShortcut* sc = options.keyboardShortcut(cmdName);

    if (sc != NULL) {
        wxString accel = _T("\t");
        if (sc->ctrl()) {
            accel.Append(_T("Ctrl-"));
            accel.Append(WxConversion::toWxString(sc->key()));
            return accel;
        } else if (sc->alt()) {
            accel.Append(_T("Alt-"));
            accel.Append(WxConversion::toWxString(sc->key()));
            return accel;
        } else if (sc->fKey() > 0) {
            accel.Append(_T("F"));
            accel.Append(WxConversion::toWxString(sc->fKey()));
            return accel;
        } else {
            // Invalid shortcut.
            return _T("");
        }
    }

    return _T("");
}


/**
 * Handles command events from the menubar.
 *
 * Commands are handled using Proxim command registry. A command is
 * requested from the registry with the command event ID. If a command
 * is found in the registry, the command will be executed. Otherwise
 * an errordialog is displayed.
 *
 * @param event Command event to handle.
 */
void
ProximMainFrame::onCommandEvent(wxCommandEvent& event) {

    GUICommand* command =
        wxGetApp().commandRegistry().createCommand(event.GetId());

    if (command == NULL) {
        ErrorDialog dialog(
            this, _T("No handler found for the command event"));
        dialog.ShowModal();
        return;
    }

    command->setParentWindow(this);
    command->Do();
    delete command;
}


/**
 * Docks a simulation subwindow to the mainframe.
 *
 * @param window Sub window to add.
 */
void
ProximMainFrame::dockWindow(wxWindow* window) {

    // If the frame is empty, the window is added directly to the sizer
    // w/o a splitterwindow.
    if (topSplitter_ == NULL) {
        topSplitter_ = window;
        sizer_->Add(window);
        return;
    }

    // The frame already contained window(s). A new splitter window is created,
    // and the old contents of the frame is set as the top half of the
    // splitter window, and the new window as the bottom half of the
    // splitter. Finally, the old contents of the frame is replaced with the
    // new splitter window.
    sizer_->Detach(topSplitter_);
    wxSplitterWindow* newSplitter = new wxSplitterWindow(this, wxHORIZONTAL);
    newSplitter->SetMinimumPaneSize(25);
    window->Reparent(newSplitter);
    topSplitter_->Reparent(newSplitter);
    newSplitter->SplitHorizontally(topSplitter_, window);
    topSplitter_ = newSplitter;
    sizer_->Add(newSplitter, 1, wxGROW);
    sizer_->Layout();
    newSplitter->SetSashPosition(topSplitter_->GetSize().GetHeight() / 2);
}



/**
 * Handles simulator events.
 *
 * The events are passed to all subwindows registered as simulator listeners.
 *
 * @param event Simulator event to handle.
 */
void
ProximMainFrame::onSimulatorEvent(SimulatorEvent& event) {

    updateSimulationStatus();

    WXTYPE eventType = event.GetEventType();

    if (eventType == SimulatorEvent::EVT_SIMULATOR_COMMAND) {
        wxSetCursor(*wxHOURGLASS_CURSOR);
        wxString status = _T("Executing command '");
        status.Append(WxConversion::toWxString(event.data()));
        status.Append(_T("'."));
        GetStatusBar()->SetStatusText(status);
        GetMenuBar()->Disable();

        // If a toolbar button is disabled and re-enabled when a mouse cursor
        // is over the button, the button can't be clicked until the cursor
        // is moved away from the button and back over it. This prevents
        // stepi button to work when clicked repeatedly. Stepi
        // commands do not disable toolbar buttons when executed to avoid
        // this problem.
        std::string command = event.data();
        if (command != ProximConstants::SCL_STEP_INSTRUCTION) {
            GetToolBar()->Disable();
        }
    }
    
    if (eventType == SimulatorEvent::EVT_SIMULATOR_COMMAND_DONE) {
        wxSetCursor(*wxSTANDARD_CURSOR);
        GetStatusBar()->SetStatusText(_T(""));
        GetMenuBar()->Enable();

        std::string command = event.data();
        // Kludge to make stepi button to work better. See comment
        // in the previous if-clause.
        if (command != ProximConstants::SCL_STEP_INSTRUCTION) {
            GetToolBar()->Enable();
        }
    }

    if (eventType == SimulatorEvent::EVT_SIMULATOR_RUNTIME_ERROR) {
        wxString message = WxConversion::toWxString(event.data());
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
    }

    if (eventType == SimulatorEvent::EVT_SIMULATOR_RUNTIME_WARNING) {
        wxString message = WxConversion::toWxString(event.data());
        WarningDialog dialog(this, message);
        dialog.ShowModal();
    }

    // Loading program.
    if (eventType == SimulatorEvent::EVT_SIMULATOR_LOADING_PROGRAM) {
        wxString message = _T("Loading program... ");
        message.Append(WxConversion::toWxString(event.data()));
        GetStatusBar()->SetStatusText(message);
    }

    // Program loaded
    if (eventType == SimulatorEvent::EVT_SIMULATOR_PROGRAM_LOADED) {
        wxString message = _T("Program loaded.");
        GetStatusBar()->SetStatusText(message);
        updateMemoryWindowMenuItem();
    }

    // Loading machine.
    if (eventType == SimulatorEvent::EVT_SIMULATOR_LOADING_MACHINE) {
        wxString message = _T("Loading machine... ");
        message.Append(WxConversion::toWxString(event.data()));
        GetStatusBar()->SetStatusText(message);
    }

    // Machine loaded
    if (eventType == SimulatorEvent::EVT_SIMULATOR_MACHINE_LOADED) {
        wxString message = _T("Machine loaded.");
        GetStatusBar()->SetStatusText(message);
    }

    //  Program is being simulated.
    if (eventType == SimulatorEvent::EVT_SIMULATOR_RUN) {
        // If an instance of stop dialog is already visible, do not create
        // a new one.
        if (!stopDialogInstantiated_) {
            stopDialogInstantiated_ = true;
            wxString message = _T("Running simulation... ");
            message.Append(WxConversion::toWxString(event.data()));
            GetStatusBar()->SetStatusText(message);
            ProximStopDialog dialog(this, -1, *ProximToolbox::frontend());
            dialog.ShowModal();
            stopDialogInstantiated_ = false;
            
            // Another workaround for a gtk-bug where button goes out of focus.
            GetToolBar()->Show(false);
            GetToolBar()->Show(true);
        }
    }

    for (unsigned i = 0; i < subwindows.size(); i++) {
        wxWindow * window = subwindows[i];
        wxPostEvent(window, event);
    }

    // Update menus.
    wxMenuBar* menubar = GetMenuBar();
    for (unsigned i = 0; i < menubar->GetMenuCount(); i++) {
        menubar->GetMenu(i)->UpdateUI();
    }
}

/**
 * Adds a window to the list of subwindows to send the simulator events to.
 *
 * @param window Window to be added.
 */
void
ProximMainFrame::addSubWindow(ProximSimulatorWindow* window) {
    if (!ContainerTools::containsValue(subwindows, window)) {
        subwindows.push_back(window);
    }
}

/**
 * Removes window from the list of subwindows to send the simulator events to.
 *
 * @param window Window to be removed.
 */
void
ProximMainFrame::removeSubWindow(ProximSimulatorWindow* window) {
    if (!ContainerTools::removeValueIfExists(subwindows, window)) {
        std::string procName = "ProximMainFrame::removeSubWindow";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
}

/**
 * Event handler for View menu check items.
 *
 * @param event View menu event to handle.
 */
void
ProximMainFrame::onToggleWindow(wxCommandEvent& event) {

    int id = event.GetId();

    switch (id) {

    case ProximConstants::COMMAND_TOGGLE_MEMORY_WIN: {
    // Toggle memory window.
    wxWindow* memoryWindow = FindWindowById(ProximConstants::ID_MEMORY_WINDOW);
    if (memoryWindow == NULL) {
        memoryWindow = new ProximMemoryWindow(
            this, ProximConstants::ID_MEMORY_WINDOW);
        ProximToolbox::addFramedWindow(
            memoryWindow, _T("Memory"), false, wxSize(620, 400));
    } else {
        memoryWindow->GetParent()->Destroy();
        memoryWindow = NULL;
    }
    return;

    }

    case ProximConstants::COMMAND_TOGGLE_CONTROL_WIN: {
    // Toggle control window.
    wxWindow* controlWindow =
        FindWindowById(ProximConstants::ID_CONTROL_WINDOW);
    if (controlWindow == NULL) {
        controlWindow = new ProximControlWindow(
            this, ProximConstants::ID_CONTROL_WINDOW);
        ProximToolbox::addFramedWindow(
            controlWindow, _T("Simulator control"), false);
    } else {
        controlWindow->GetParent()->Destroy();
        controlWindow = NULL;
    }
    return;

    }

    case ProximConstants::COMMAND_TOGGLE_MACHINE_WIN: {
    // Toggle machine window.
    wxWindow* machineWindow = FindWindowById(
        ProximConstants::ID_MACHINE_STATE_WINDOW);
    if (machineWindow == NULL) {
        machineWindow = new ProximMachineStateWindow(
            this, ProximConstants::ID_MACHINE_STATE_WINDOW);
        ProximToolbox::addFramedWindow(
            machineWindow, _T("Simulated Machine"), false, wxSize(600,400));
    } else {
        machineWindow->GetParent()->Destroy();
        machineWindow = NULL;
    }
    return;

    }

    case ProximConstants::COMMAND_TOGGLE_BREAKPOINT_WIN: {
    // Toggle breakpoitn window.
    wxWindow* breakpointWindow = FindWindowById(
        ProximConstants::ID_BREAKPOINT_WINDOW);
    if (breakpointWindow == NULL) {
        breakpointWindow = new ProximBreakpointWindow(
            this, ProximConstants::ID_BREAKPOINT_WINDOW);
        ProximToolbox::addFramedWindow(
            breakpointWindow, _T("Breakpoints"), false);
    } else {
        breakpointWindow->GetParent()->Close();
        breakpointWindow = NULL;
    }
    return;

    }

    case ProximConstants::COMMAND_TOGGLE_CMD_HISTORY_WIN: {
    // Toggle command history window.
    wxWindow* historyWindow = FindWindowById(
        ProximConstants::ID_CMD_HISTORY_WINDOW);
    if (historyWindow == NULL) {
        historyWindow = new ProximCmdHistoryWindow(
            this, ProximConstants::ID_CMD_HISTORY_WINDOW,
            ProximToolbox::lineReader());
        ProximToolbox::addFramedWindow(
            historyWindow, _T("Command History"), false);
    } else {
        historyWindow->GetParent()->Close();
        historyWindow = NULL;
    }
    return;

    }

    case ProximConstants::COMMAND_TOGGLE_DEBUGGER_WIN: {
    // Toggle command history window.
    wxWindow* debuggerWindow = FindWindowById(
        ProximConstants::ID_DEBUGGER_WINDOW);
    if (debuggerWindow == NULL) {
        debuggerWindow = new ProximDebuggerWindow(
            this, ProximConstants::ID_DEBUGGER_WINDOW);
        ProximToolbox::addFramedWindow(
            debuggerWindow, _T("Source code debugger"), false);
    } else {
        debuggerWindow->GetParent()->Close();
        debuggerWindow = NULL;
    }
    return;

    }
    }
    event.Skip();
}


/**
 * Event handler for wxUpdateUIEvents.
 *
 * Proxim updates buttons and menu items for command registry commands using
 * wxUpdateUIEvents. When a wxUpdateUIEvent is received, command registry
 * is searched for corresponding command ID. If a command is found, the
 * UI element enabled or disabled according to the command's isEnabled()
 * return value.
 *
 * @param event Upadate event to handle.
 */
void
ProximMainFrame::updateCommand(wxUpdateUIEvent& event) {

    if (wxGetApp().simulation() == NULL) {
        return;
    }

    GUICommand* command =
        wxGetApp().commandRegistry().createCommand(event.GetId());

    if (command != NULL) {
        event.Enable(command->isEnabled());
        delete command;
    } else {
        event.Enable(false);
    }
}


/**
 * Event handler which updates view-menu check item check-marks.
 *
 * When a wxUpdateUIEvent is received for view-menu check item, the
 * main frame checks if corresponding subwindow exists and updates the
 * menu item.
 */
void
ProximMainFrame::updateToggleItem(wxUpdateUIEvent& event) {

    // memory window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_MEMORY_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_MEMORY_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }

    // simulator control window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_CONTROL_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_CONTROL_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }

    // machine window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_MACHINE_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_MACHINE_STATE_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }

    // breakpoint window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_BREAKPOINT_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_BREAKPOINT_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }

    // command history window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_CMD_HISTORY_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_CMD_HISTORY_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }

    // debugger window check-item
    if (event.GetId() == ProximConstants::COMMAND_TOGGLE_DEBUGGER_WIN) {
        bool exists = FindWindowById(ProximConstants::ID_DEBUGGER_WINDOW);
        if (exists) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    }
}

/**
 * Handles key events.
 * 
 * @param event Key event to handle.
 */
void
ProximMainFrame::onKeyEvent(wxKeyEvent& event) {
    event.Skip();
}   


/**
 * Handles system close events.
 *
 * This event handler is called for example when the window is being closed
 * using the frame close button X.
 *
 * @param event Close event to handle;
 */
void
ProximMainFrame::onClose(wxCloseEvent& event) {
    if (event.CanVeto()) {
        // Execute quit-command.
        GUICommand* quitCommand = wxGetApp().commandRegistry().createCommand(
            ProximConstants::COMMAND_NAME_QUIT);
        if (quitCommand->isEnabled()) {
            quitCommand->setParentWindow(this);
            if (!quitCommand->Do()) {
                event.Veto();
            }
        }
        event.Veto();
        delete quitCommand;
        return;
    }
}


/**
 * Updates simulation status and cycle count in the statusbar.
 */
void
ProximMainFrame::updateSimulationStatus() {
    const SimulatorFrontend* simulation = ProximToolbox::frontend();
    wxString status;
    ClockCycleCount cycles = 0;
    if (simulation == NULL) {
        status = _T("Not initialzed");
    } else if (simulation->isSimulationInitialized()) {
        cycles = simulation->cycleCount();
        status = _T("Initialized");
    } else if (simulation->isSimulationRunning()) {
        cycles = simulation->cycleCount();
        status = _T("Running");
    } else if (simulation->isSimulationStopped()) {
        cycles = simulation->cycleCount();
        status = _T("Stopped");
    } else if (simulation->hasSimulationEnded()) {
        cycles = simulation->cycleCount();
        status = _T("Finished");
    }

    wxStatusBar* statusBar = GetStatusBar();
    statusBar->SetStatusText(status, 1);

    wxString cyclesStr = WxConversion::toWxString((boost::format("%.0f") %
                                                   cycles).str());
    cyclesStr.Prepend(_T("Cycles: "));
    statusBar->SetStatusText(cyclesStr, 2);
}


/**
 * Updates "Memory Window" menu item depending on if memory is available
 */
void 
ProximMainFrame::updateMemoryWindowMenuItem() {
    if (!wxGetApp().simulation() ||
        !wxGetApp().simulation()->frontend() ||        
        !wxGetApp().simulation()->frontend()->isSimulationInitialized() ||
        wxGetApp().simulation()->frontend()->memorySystem().
        memoryCount() == 0) {
        GetMenuBar()->Enable(ProximConstants::COMMAND_TOGGLE_MEMORY_WIN, false);
        wxWindow* memoryWindow = FindWindowById(ProximConstants::ID_MEMORY_WINDOW);
        if (memoryWindow != NULL) {
            memoryWindow->Destroy();
            memoryWindow = NULL;
        }
    } else {
        GetMenuBar()->Enable(ProximConstants::COMMAND_TOGGLE_MEMORY_WIN, true);
    }
}


/**
 * Event handler for the simulator reset event.
 */
void
ProximMainFrame::onReset(const SimulatorEvent&) {
    reset();
}


/**
 * Resets all subwidnows, which acess simulator data directly.
 *
 * If this fucntion is from another thread than the GUI thread,
 * a reset event is passed to the GUI thread, which will reset
 * the windows in the GUI thread. The caller thread is blocked and
 * wont return from this function until the reset is complete.
 */
void
ProximMainFrame::reset() {
    if (wxThread::IsMain()) {
        for (int i = subwindows.size() - 1; i >= 0; i--) {
            subwindows[i]->reset();
        }
        isReset_ = true;
        resetCondition_->Signal();
    } else {
        SimulatorEvent event(SimulatorEvent::EVT_SIMULATOR_RESET);
        wxPostEvent(this, event);
        while (!isReset_) {
            resetCondition_->WaitTimeout(100);
        }
        isReset_ = false;
    }
}
