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
 * @file SimulatorSettingsDialog.hh
 *
 * Declaration of SimulatorSettingsDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
        ID_PROFILE_DATA_SAVING,
        ID_UTILIZATION_DATA_SAVING,
        ID_NEXT_INSTRUCTION_PRINTING,
        ID_SIMULATION_TIME_STATISTICS,
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

    /// Profile data saving enabled/disabled
    bool profileDataSaving_;
    /// Utilization data saving enabled/disabled
    bool utilizationDataSaving_;
    /// Next instruction printing enabled/disabled
    bool nextInstructionPrinting_;
    /// Profile data saving enabled/disabled
    bool simulationTimeStatistics_;

    /// Input history log file enabled/disabled.
    bool historySave_;
    /// History log max size.
    int historySize_;
    /// History log file name.
    wxString historyFile_;

    DECLARE_EVENT_TABLE()
};
#endif
