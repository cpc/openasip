/**
 * @file ProximCmdHistoryWindow.hh
 *
 * Declaration of ProximCmdHistoryWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_CMD_HISTORY_WINDOW_HH
#define TTA_PROXIM_CMD_HISTORY_WINDOW_HH

#include <wx/wx.h>

#include "ProximSimulatorWindow.hh"

class SimulatorEvent;
class ProximLineReader;
class ProximMainFrame;

/**
 * Window for browsing Proxim command history.
 */
class ProximCmdHistoryWindow : public ProximSimulatorWindow {
public:
    ProximCmdHistoryWindow(
        ProximMainFrame* parent,
        wxWindowID id,
        ProximLineReader& lineReader);

    virtual ~ProximCmdHistoryWindow();
private:
    void onSimulatorCommand(SimulatorEvent& event);
    void updateCommandList();
    wxSizer* createContents(wxWindow* parent, bool set_sizer, bool call_fit);

    void onClose(wxCommandEvent& event);
    void onSave(wxCommandEvent& event);
    void onCommandDClick(wxCommandEvent& event);

    // Widget IDs.
    enum {
        ID_LIST = 10000,
        ID_SAVE,
        ID_CLOSE,
        ID_LINE
    };

    /// Line reader storing the command history.
    ProximLineReader& lineReader_;
    /// ListBox widget displaying the command history.
    wxListBox* cmdList_;

    DECLARE_EVENT_TABLE()
};
#endif
