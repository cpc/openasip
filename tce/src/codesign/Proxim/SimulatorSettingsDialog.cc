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
 * @file SimulatorSettingsDialog.cc
 *
 * Implementation of SimulatorSettingsDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include "SimulatorSettingsDialog.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "SimulatorFrontend.hh"
#include "ProximConstants.hh"
#include "Conversion.hh"

BEGIN_EVENT_TABLE(SimulatorSettingsDialog, wxDialog)
    EVT_BUTTON(wxID_OK, SimulatorSettingsDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Dialog window identifier.
 * @param simulator Simulator frontend to configure.
 */
SimulatorSettingsDialog::SimulatorSettingsDialog(
    wxWindow* parent, wxWindowID id, SimulatorFrontend& simulator,
    ProximLineReader& lineReader) :
  wxDialog(parent, id, _T("Simulator Settings"), wxDefaultPosition),
    simulator_(simulator), lineReader_(lineReader) {

    createContents(this, true, true);

    FindWindow(ID_EXEC_TRACE)->SetValidator(wxGenericValidator(&execTrace_));
    FindWindow(ID_BUS_TRACE)->SetValidator(wxGenericValidator(&busTrace_));
    FindWindow(ID_PT_TRACKING)->SetValidator(wxGenericValidator(&ptTrace_));
    FindWindow(ID_RF_TRACKING)->SetValidator(wxGenericValidator(&rfTrace_));
    FindWindow(ID_FU_CONFLICT_DETECTION)->SetValidator(
        wxGenericValidator(&fuConflictDetection_));

    FindWindow(ID_PROFILE_DATA_SAVING)->SetValidator(
        wxGenericValidator(&profileDataSaving_));
    FindWindow(ID_UTILIZATION_DATA_SAVING)->SetValidator(
        wxGenericValidator(&utilizationDataSaving_));
    FindWindow(ID_NEXT_INSTRUCTION_PRINTING)->SetValidator(
        wxGenericValidator(&nextInstructionPrinting_));
    FindWindow(ID_SIMULATION_TIME_STATISTICS)->SetValidator(
        wxGenericValidator(&simulationTimeStatistics_));

    FindWindow(ID_SAVE_HISTORY)->SetValidator(
        wxGenericValidator(&historySave_));

    FindWindow(ID_HISTORY_FILE)->SetValidator(
        wxGenericValidator(&historyFile_));

    FindWindow(ID_HISTORY_SIZE)->SetValidator(
        wxGenericValidator(&historySize_));
}

/**
 * The Destructor.
 */
SimulatorSettingsDialog::~SimulatorSettingsDialog() {
}


/**
 * Transfers settings data from the simulator frontend to the dialog widgets.
 */
bool
SimulatorSettingsDialog::TransferDataToWindow() {

    execTrace_ = simulator_.executionTracing();
    busTrace_ = simulator_.busTracing();
    ptTrace_ = simulator_.procedureTransferTracing();
    rfTrace_ = simulator_.rfAccessTracing();

    profileDataSaving_ = simulator_.profileDataSaving();
    utilizationDataSaving_ = simulator_.utilizationDataSaving();
    nextInstructionPrinting_ = simulator_.nextInstructionPrinting();
    simulationTimeStatistics_ = simulator_.simulationTimeStatistics();
    
    fuConflictDetection_ = simulator_.fuResourceConflictDetection();

    historySave_ = lineReader_.saveInputHistoryToFile();
    historySize_ = lineReader_.inputHistoryMaxLength();
    historyFile_ = WxConversion::toWxString(
        lineReader_.inputHistoryFilename());

    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the simulator settings and closes the dialog when the OK-button
 * is pressed.
 */
void
SimulatorSettingsDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    std::string command;
    std::string historyFile =
        WxConversion::toString(historyFile_.Trim(false).Trim(true));

    if (simulator_.executionTracing() != execTrace_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_EXEC_TRACE + " ";
        command += Conversion::toString(execTrace_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.busTracing() != busTrace_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_BUS_TRACE + " ";
        command += Conversion::toString(busTrace_);
        command += ProximConstants::SCL_DELIM;
    }
    
    if (simulator_.procedureTransferTracing() != ptTrace_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_PROCEDURE_TRANSFER_TRACE + " ";
        command += Conversion::toString(ptTrace_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.rfAccessTracing() != rfTrace_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_RF_TRACE + " ";
        command += Conversion::toString(rfTrace_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.fuResourceConflictDetection() != fuConflictDetection_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_FU_CONFLICT_DETECTION + " ";
        command += Conversion::toString(fuConflictDetection_);
        command += ProximConstants::SCL_DELIM;
    }

    if (lineReader_.saveInputHistoryToFile() != historySave_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_HISTORY_SAVE + " ";
        command += Conversion::toString(historySave_);
        command += ProximConstants::SCL_DELIM;
    }

    if (lineReader_.inputHistoryMaxLength() != (size_t)historySize_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_HISTORY_SIZE + " ";
        command += Conversion::toString(historySize_);
        command += ProximConstants::SCL_DELIM;

    }

    if (lineReader_.inputHistoryFilename() != historyFile) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_HISTORY_FILE + " ";
        command += historyFile;
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.profileDataSaving() != profileDataSaving_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_PROFILE_DATA_SAVING + " ";
        command += Conversion::toString(profileDataSaving_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.utilizationDataSaving() != utilizationDataSaving_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_UTILIZATION_DATA_SAVING + " ";
        command += Conversion::toString(utilizationDataSaving_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.nextInstructionPrinting() != nextInstructionPrinting_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_NEXT_INSTRUCTION_PRINTING + " ";
        command += Conversion::toString(nextInstructionPrinting_);
        command += ProximConstants::SCL_DELIM;
    }

    if (simulator_.simulationTimeStatistics() != simulationTimeStatistics_) {
        command += ProximConstants::SCL_SET + " ";
        command += ProximConstants::SCL_SETTING_SIMULATION_TIME_STATISTICS + " ";
        command += Conversion::toString(simulationTimeStatistics_);
        command += ProximConstants::SCL_DELIM;
    }

    lineReader_.input(command);

    EndModal(wxID_OK);
}


/**
 * Creates the dialog widgets.
 *
 * Source code generated by wxDesigner. Do not modify by hand!
 *
 * @param parent Parent dialog of the widgets.
 * @param call_fit If true, resize the dialog to fit the widgets.
 * @param set_sizer If true, set the created widgets as the dialog contents.
 * @return Top level sizer of the dialog contents.
 */
wxSizer*
SimulatorSettingsDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Tracing:") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    wxCheckBox *item3 = new wxCheckBox( parent, ID_EXEC_TRACE, wxT("Write basic execution trace."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item4 = new wxCheckBox( parent, ID_BUS_TRACE, wxT("Write bus trace."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    wxCheckBox *item19 = new wxCheckBox( parent, ID_PT_TRACKING, wxT("Procedure transfer tracking."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );    

    wxCheckBox *item5 = new wxCheckBox( parent, ID_RF_TRACKING, wxT("Concurrent register file access tracking."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item6 = new wxCheckBox( parent, ID_FU_CONFLICT_DETECTION, wxT("Function unit resouce conflict detection."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *profiledata = new wxCheckBox( parent, ID_PROFILE_DATA_SAVING, wxT("Profile data saving."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( profiledata, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *utildata = new wxCheckBox( parent, ID_UTILIZATION_DATA_SAVING, wxT("Utilization data saving."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( utildata, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *nextinstr = new wxCheckBox( parent, ID_NEXT_INSTRUCTION_PRINTING, wxT("Next instruction printing."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( nextinstr, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *timestat = new wxCheckBox( parent, ID_SIMULATION_TIME_STATISTICS, wxT("Simulation time statistics."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( timestat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item8 = new wxStaticBox( parent, -1, wxT("Command history:") );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item8, wxVERTICAL );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item10 = new wxStaticText( parent, ID_LABEL_HISTORY_SIZE, wxT("Maximum size of command history:"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item11 = new wxSpinCtrl( parent, ID_HISTORY_SIZE, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 10000, 0 );
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item7->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item12 = new wxBoxSizer( wxVERTICAL );

    wxCheckBox *item13 = new wxCheckBox( parent, ID_SAVE_HISTORY, wxT("Save command history to file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add( item13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item14 = new wxTextCtrl( parent, ID_HISTORY_FILE, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item12->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item7->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item15 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item17 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item18 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item16, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
