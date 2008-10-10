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
 * @file ConsoleWindow.hh
 *
 * Declaration of ConsoleWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
    virtual void onSimulatorBusy(SimulatorEvent& event);
    virtual void onSimulatorDone(SimulatorEvent& event);

    void createContents();
    void onSimulatorOutput(const SimulatorEvent& event);
    void onError(const SimulatorEvent& event);
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
