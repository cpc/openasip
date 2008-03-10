/**
 * @file ProximUnitWindow.cc
 *
 * Declaration of ProximUnitWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_UNIT_WINDOW_HH
#define TTA_PROXIM_UNIT_WINDOW_HH

#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"

class SimulatorFrontend;
class ProximMainFrame;
class wxListCtrl;

/**
 * Parent class for windows displaying list information of the Proxim machine
 * state window units.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximUnitWindow : public ProximSimulatorWindow {
public:
    ProximUnitWindow(ProximMainFrame* parent, int id);
    virtual ~ProximUnitWindow();

protected:
    /// Simulator instance .
    SimulatorFrontend* simulator_;
    /// Unit choicer widget.
    wxChoice* unitChoice_;
    /// Value display mode choicer widget.
    wxChoice* modeChoice_;
    /// List widget for the values.
    wxListCtrl* valueList_;

    /// String for the mode choicer integer mode.
    static const wxString MODE_INT;
    /// String for the mode choicer unsigned integer mode.
    static const wxString MODE_UNSIGNED;
    /// String for the mode choicer hexadecimal mode.
    static const wxString MODE_HEX;
    /// String for the mode choicer binary mode.
    static const wxString MODE_BIN;

private:
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void onChoice(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    wxSizer* createContents(
        wxWindow* parent, bool call_fit, bool set_sizer);

    virtual void reinitialize();
    virtual void update();

    // Toplevel sizer for the window widgets.
    wxBoxSizer* sizer_;

    // Widget IDs.
    enum {
	ID_UNIT_CHOICE = 10000,
        ID_MODE_CHOICE,
	ID_HELP,
	ID_CLOSE,
	ID_LINE,
	ID_VALUE_LIST
    };

    DECLARE_EVENT_TABLE()
};

#endif
