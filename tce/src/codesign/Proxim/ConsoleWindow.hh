/**
 * @file ConsoleWindow.hh
 *
 * Declaration of ConsoleWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
