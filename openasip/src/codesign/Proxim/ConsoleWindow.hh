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
 * @file ConsoleWindow.hh
 *
 * Declaration of ConsoleWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef CONSOLE_WINDOW_HH
#define CONSOLE_WINDOW_HH

#include <wx/wx.h>
#include <string>
#include "SimulatorEvent.hh"
#include "ProximSimulatorWindow.hh"

class ProximLineReader;
class ProximMainFrame;

/**
 * Simulator console window class.
 *
 * ConsoleWindow is a console with a multiline textual output window,
 * and a singleline textual input widget. Commands are requested from
 * the console window using SimulatorEvents.
 */
class ConsoleWindow : public ProximSimulatorWindow {
public:
    ConsoleWindow(ProximMainFrame* parent, wxWindowID id);
    virtual ~ConsoleWindow();
    virtual void reset();
    void write(std::string text);
    void clear();

private:
    virtual void onSimulatorBusy(wxEvent& simEvent);
    virtual void onSimulatorDone(wxEvent& simEvent);

    void createContents();
    void onSimulatorOutput(wxEvent& simEvent);
    void onError(wxEvent& simEvent);
    void textEntered(wxCommandEvent& event);
    void onInputKey(wxKeyEvent& event);
    /// Output text control.
    wxTextCtrl* outTextCtrl_;
    /// Input text control.
    wxTextCtrl* inTextCtrl_;
    /// Linereader to send the user input to.
    ProximLineReader* lineReader_;
    /// Stores the command history iterator, used for command history browsing.
    int historyIterator_;

    /// IDs for the window widgets.
    enum {
	ID_INPUT = 20000,
	ID_OUTPUT
    };

    /**
     * Text input widget for the console.
     *
     * This derived wxTextCtrl passes all key events through the parent
     * console's key event handler.  Event passing makes it possible to hook
     * special keys such as browsing command history with up/down arrow keys.
     */
    class ConsoleWindowInputCtrl : public wxTextCtrl {
    public:
	ConsoleWindowInputCtrl(ConsoleWindow* console);
	virtual ~ConsoleWindowInputCtrl() {};
    private:
	void onKeyDown(wxKeyEvent& event);
	/// Parent console of the widget.
	ConsoleWindow* console_;

	DECLARE_EVENT_TABLE()
    };

    DECLARE_EVENT_TABLE()

};

#endif
