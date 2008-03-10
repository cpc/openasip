/**
 * @file SimulatorSettingsDialog.hh
 *
 * Declaration of SimulatorSettingsDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_SETTINGS_DIALOG_HH
#define TTA_SIMULATOR_SETTINGS_DIALOG_HH

#include <wx/wx.h>

class SimulatorFrontend;
class ProximLineReader;

/**
 * Dialog for modifying simulator settings.
 */
class SimulatorSettingsDialog : public wxDialog {
public:
    SimulatorSettingsDialog(
        wxWindow* parent,
        wxWindowID id,
        SimulatorFrontend& simulator,
        ProximLineReader& lineReader);
    virtual ~SimulatorSettingsDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool set_sizer, bool call_fit);
    void onOK(wxCommandEvent& event);

    /// Widget IDs.
    enum {
        ID_EXEC_TRACE = 10000,
        ID_BUS_TRACE,
        ID_PT_TRACKING,
        ID_RF_TRACKING,
        ID_FU_CONFLICT_DETECTION,
        ID_LABEL_HISTORY_SIZE,
        ID_HISTORY_SIZE,
        ID_SAVE_HISTORY,
        ID_HISTORY_FILE,
        ID_LINE
    };

    /// Frontend of the simulator to configure.
    SimulatorFrontend& simulator_;
    /// Line reader to configure
    ProximLineReader& lineReader_;

    /// Execution trace enabled/disabled.
    bool execTrace_;
    /// Bus trace enabled/disabled.
    bool busTrace_;
    /// Procedure transfer tracing enabled/disabled
    bool ptTrace_;
    /// Register file access tracing enabled/disabled.
    bool rfTrace_;
    /// FU conflict detection enabled/disabled.
    bool fuConflictDetection_;    

    /// Input history log file enabled/disabled.
    bool historySave_;
    /// History log max size.
    int historySize_;
    /// History log file name.
    wxString historyFile_;

    DECLARE_EVENT_TABLE()
};
#endif
