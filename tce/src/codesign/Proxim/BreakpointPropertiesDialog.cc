/**
 * @file BreakpointPropertiesDialog.cc
 *
 * Implementation of BreakpointPropertiesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red.
 */

#include <string>
#include <vector>
#include "BreakpointPropertiesDialog.hh"
#include "Breakpoint.hh"
#include "WxConversion.hh"
#include "StopPointManager.hh"
#include "ProximToolbox.hh"
#include "ProximConstants.hh"
#include "ProximLineReader.hh"
#include "Conversion.hh"

BEGIN_EVENT_TABLE(BreakpointPropertiesDialog, wxDialog)
    EVT_BUTTON(wxID_OK, BreakpointPropertiesDialog::onOK)
END_EVENT_TABLE()

using std::vector;
using std::string;

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param breakpoint Breakpoint to edit.
 */
BreakpointPropertiesDialog::BreakpointPropertiesDialog(
    wxWindow* parent, StopPointManager& manager, unsigned int handle) :
  wxDialog(parent, -1, _T("Breakpoint properties"), wxDefaultPosition),
    manager_(manager), handle_(handle) {

    createContents(this, true, true);

}


/**
 * The Destructor.
 */
BreakpointPropertiesDialog::~BreakpointPropertiesDialog() {
}


/**
 * Sets the widget values when the dialog is opened.
 */
bool
BreakpointPropertiesDialog::TransferDataToWindow() {

    const Breakpoint& breakpoint = dynamic_cast<const Breakpoint&>(
        manager_.stopPointWithHandleConst(handle_));

    if (breakpoint.isConditional()) {
        wxTextCtrl* conditionCtrl =
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_CONDITION));
        for (unsigned i = 0; i < breakpoint.condition().script().size(); i++) {
            string line = breakpoint.condition().script()[i];
            conditionCtrl->AppendText(WxConversion::toWxString(line));
        }
    }

    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_IGNORE_CTRL))->SetValue(
        breakpoint.ignoreCount());

    dynamic_cast<wxStaticText*>(FindWindow(ID_TEXT_BP_ID))->SetLabel(
        WxConversion::toWxString(handle_));

    return wxDialog::TransferDataToWindow();
}


/**
 * Handles the dialog OK-button event.
 *
 * Breakpoint properties are checked for validity, and the breakpoint is
 * modified if the properties are valid. An error dialog is displayed if
 * the breakpoint properties are invalid.
 */
void
BreakpointPropertiesDialog::onOK(wxCommandEvent&) {

    wxTextCtrl* conditionCtrl =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_CONDITION));

    unsigned ignoreCount =
        dynamic_cast<wxSpinCtrl*>(FindWindow(ID_IGNORE_CTRL))->GetValue();

    string condition = WxConversion::toString(
        conditionCtrl->GetValue().Trim(true).Trim(false));

    // Check condition script.
    if (condition != "" && !ProximToolbox::testCondition(this, condition)) {
        // Invalid condition.
        return;
    }

    ProximLineReader& lineReader = ProximToolbox::lineReader();

    // Set ignore count.
    std::string ignoreCommand = ProximConstants::SCL_SET_IGNORE_COUNT +
        " " + Conversion::toString(handle_) +
        " " + Conversion::toString(ignoreCount);

    lineReader.input(ignoreCommand);

    // Set Condition script.
    std::string conditionCommand = ProximConstants::SCL_SET_CONDITION + " " +
        Conversion::toString(handle_);

    lineReader.input(conditionCommand);
    lineReader.input(condition);

    EndModal(wxID_OK);
}


/**
 * Creates the dialog contents.
 */
wxSizer*
BreakpointPropertiesDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT_BREAKPOINT, wxT("Breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item3 = new wxStaticText( parent, ID_TEXT_BP_ID, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_IGNORE_COUNT, wxT("Ignore count:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item5 = new wxSpinCtrl( parent, ID_IGNORE_CTRL, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item1->Add( item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_CONDITION, wxT("Condition:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_CONDITION, wxT(""), wxDefaultPosition, wxSize(250,-1));
    item1->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item9 = new wxGridSizer( 2, 0, 0 );

    wxButton *item10 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
