/**
 * @file ConsoleWindow.cc
 *
 * Implementation of ConsoleWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/textctrl.h>
#include <wx/valtext.h>
#include "ConsoleWindow.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"

BEGIN_EVENT_TABLE(ConsoleWindow, ProximSimulatorWindow)
    EVT_TEXT_ENTER(ID_INPUT, ConsoleWindow::textEntered)
    EVT_SIMULATOR_OUTPUT(-1, ConsoleWindow::onSimulatorOutput)
    EVT_SIMULATOR_ERROR(-1, ConsoleWindow::onError)
    EVT_SIMULATOR_RUNTIME_WARNING(-1, ConsoleWindow::onError)
    EVT_SIMULATOR_RUNTIME_ERROR(-1, ConsoleWindow::onError)
    EVT_SIMULATOR_COMMAND(-1, ConsoleWindow::onSimulatorBusy)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ConsoleWindow::ConsoleWindowInputCtrl, wxTextCtrl)
    EVT_KEY_DOWN(ConsoleWindowInputCtrl::onKeyDown)
END_EVENT_TABLE()


//-----------------------------------------------------------------------------
//
// ConsoleWindowInputCtrl
//
//-----------------------------------------------------------------------------

/**
 * The Constructor.
 *
 * @param console Pointer to the parent console of the input widget.
 */
ConsoleWindow::ConsoleWindowInputCtrl::ConsoleWindowInputCtrl(
    ConsoleWindow* console) :
    wxTextCtrl(console, ID_INPUT, _T(""), wxDefaultPosition,
	       wxSize(500,30), wxTE_PROCESS_ENTER),
    console_(console) {

}


/**
 * An event handler for the input control key events.
 *
 * Events are passed to the parent console's event handler.
 *
 * @param event Key press event to handle.
 */
void
ConsoleWindow::ConsoleWindowInputCtrl::onKeyDown(wxKeyEvent& event) {
    console_->onInputKey(event);
}


//-----------------------------------------------------------------------------
//
// ConsoleWindow
//
//-----------------------------------------------------------------------------


/**
 * The Constructor.
 *
 * @param parent Parent window of the console window.
 * @param id Window identifier.
 */
ConsoleWindow::ConsoleWindow(
    ProximMainFrame* parent,
    wxWindowID id) :
    ProximSimulatorWindow(parent, id),
    outTextCtrl_(NULL),
    inTextCtrl_(NULL),
    historyIterator_(-1) {

    createContents();
    lineReader_ = &wxGetApp().simulation()->lineReader();
}


/**
 * The Destructor.
 */
ConsoleWindow::~ConsoleWindow() {
}

/**
 * Called when the simulator program, machine and memory models are deleted.
 */
void
ConsoleWindow::reset() {
    // Do nothing.
}


/**
 * Event handler for the user input to the text input widget.
 *
 * The text is passed to the linereader as input.
 */
void
ConsoleWindow::textEntered(wxCommandEvent&) {
    std::string command = WxConversion::toString(inTextCtrl_->GetLineText(0));
    lineReader_->input(command);
    historyIterator_ = -1;
    inTextCtrl_->Clear();
}



/**
 * Appends text to the output window.
 *
 * @param text Text to append to the output window.
 */
void
ConsoleWindow::write(std::string text) {
    
    if (text == "") {
	return;
    }
    wxString output = WxConversion::toWxString(text);
    outTextCtrl_->AppendText(output);
}


/**
 * Creates the window widgets.
 */
void
ConsoleWindow::createContents() {

    // output widget
    outTextCtrl_ = new wxTextCtrl(
	this, ID_OUTPUT, _T(""), wxDefaultPosition, wxDefaultSize,
	wxTE_MULTILINE | wxTE_READONLY);

    // input widget
    inTextCtrl_ = new ConsoleWindowInputCtrl(this);

    // Sizer responsible for the window layout.
    wxFlexGridSizer* sizer = new wxFlexGridSizer(1, 0, 0);
    sizer->AddGrowableCol(0);
    sizer->AddGrowableRow(0);

    sizer->Add(outTextCtrl_, 0, wxGROW | wxALL, 5);
    sizer->Add(inTextCtrl_, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
    SetSizer(sizer);
    sizer->SetSizeHints(this);
}


/**
 * Handles simulator text output events.
 *
 * The event data contains the simulator interpreter output, which is appended
 * to the text output widget.
 *
 * @param event Simulator interpreter text output event.
 */
void
ConsoleWindow::onSimulatorOutput(const SimulatorEvent& event) {
    // Append text to the output widget.
    std::string text = event.data();
    outTextCtrl_->AppendText(WxConversion::toWxString(text));
    outTextCtrl_->ShowPosition(outTextCtrl_->GetLastPosition());
}


/**
 * Handles simulator error events.
 *
 * The error text is printed in the console.
 *
 * @param event Simulator interpreter command event.
 */
void
ConsoleWindow::onError(const SimulatorEvent& event) {
    std::string error = event.data();
    wxTextAttr oldStyle = outTextCtrl_->GetDefaultStyle();
    outTextCtrl_->SetDefaultStyle(wxTextAttr(*wxRED));
    outTextCtrl_->AppendText(WxConversion::toWxString("\n" + error + "\n"));
    outTextCtrl_->SetDefaultStyle(oldStyle);
}


/**
 * Handles special keypresses in the input widget.
 *
 * @param event Keypress event to handle.
 */
void
ConsoleWindow::onInputKey(wxKeyEvent& event) {

    if (event.GetKeyCode() == WXK_UP) {
	// Up key. Browses the command history backward.
	historyIterator_++;
	if (historyIterator_ >= int(lineReader_->inputsInHistory())) {
	    historyIterator_ = lineReader_->inputsInHistory() - 1;
	    return;
	}
    } else if (event.GetKeyCode() == WXK_DOWN) {
	// Down key. Browses the command history forward.
	if (historyIterator_ < 0) {
	    return;
	}
	historyIterator_--;
    } else {
	// Key with no special fucntion. The key event is skipped.
        event.Skip();
	return;
    }

    // If the command history is browsed back to top, the
    // input widget is cleared.
    if (historyIterator_ < 0) {
	inTextCtrl_->Clear();
	return;
    }

    // Command browsed from the command history is set as the input
    // widget value.
    wxString command = WxConversion::toWxString(
	lineReader_->inputHistoryEntry(historyIterator_));

    inTextCtrl_->SetValue(command);
    inTextCtrl_->SetInsertionPointEnd();
}



/**
 * Clears text from the output text widget.
 */
void
ConsoleWindow::clear() {
    outTextCtrl_->Clear();
}

/**
 * An event handler for the event of the simulator to start processing
 * a command.
 *
 * The Console window must stay enabled when the simulator is busy, so this
 * method is overloaded to not to lock the window.
 */
void
ConsoleWindow::onSimulatorBusy(SimulatorEvent&) {
    // Do nothing.
}

/**
 * An event handler for the event of the simulator to complete command
 * processing.
 *
 * The Console window is not locked during simulator processing, so there's
 * no need to do anything.
 */
void
ConsoleWindow::onSimulatorDone(SimulatorEvent&) {
    // Do nothing.
}
