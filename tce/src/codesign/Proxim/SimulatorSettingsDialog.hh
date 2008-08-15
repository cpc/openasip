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
