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
 * @file SimulatorSettingsDialog.cc
 *
 * Implementation of SimulatorSettingsDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
