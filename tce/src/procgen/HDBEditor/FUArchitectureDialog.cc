/**
 * @file FUArchParamatersDialog.cc
 *
 * Implementation of FUArchitectureDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include "FUArchitectureDialog.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "WxConversion.hh"

BEGIN_EVENT_TABLE(FUArchitectureDialog, wxDialog)
    EVT_BUTTON(wxID_OK, FUArchitectureDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog.
 * @param fu Function unit to parametrize.
 */
FUArchitectureDialog::FUArchitectureDialog(
    wxWindow* parent, wxWindowID id, HDB::FUArchitecture& arch) :
    wxDialog(parent, id, _T("Function unit architecture.")),
    arch_(arch) {

        createContents(this, true, true);


        paramWidthList_ =
            dynamic_cast<wxCheckListBox*>(FindWindow(ID_PARAM_WIDTH_LIST));
        guardSupportList_ =
            dynamic_cast<wxCheckListBox*>(FindWindow(ID_GUARD_SUPPORT_LIST));

        update();
}

/**
 * The Destructor.
 */
FUArchitectureDialog::~FUArchitectureDialog() {
}


/**
 * Updates the dialog list widgets.
 */
void
FUArchitectureDialog::update() {
    paramWidthList_->Clear();
    guardSupportList_->Clear();
    const TTAMachine::FunctionUnit& fu = arch_.architecture();
    for (int i = 0; i < fu.portCount(); i++) {
        std::string port = fu.port(i)->name();
        paramWidthList_->Append(WxConversion::toWxString(port));
        guardSupportList_->Append(WxConversion::toWxString(port));
        if (arch_.hasParameterizedWidth(port)) {
            paramWidthList_->Check(i);
        }
        if (arch_.hasGuardSupport(port)) {
            guardSupportList_->Check(i);
        }
    }
}

/**
 * Event handler for the dialog OK-button.
 */
void
FUArchitectureDialog::onOK(wxCommandEvent&) {
    const TTAMachine::FunctionUnit& fu = arch_.architecture();
    for (int i = 0; i < fu.portCount(); i++) {
        std::string port = fu.port(i)->name();
        if (paramWidthList_->IsChecked(i)) {
            arch_.setParameterizedWidth(port);
        }
        if (guardSupportList_->IsChecked(i)) {
            arch_.setGuardSupport(port);
        }
    }

    EndModal(wxID_OK);
}

/**
 * Creates the dialog widgets.
 */
wxSizer*
FUArchitectureDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Parametrized width:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );

    wxWindow *item4 = new wxCheckListBox(parent, ID_PARAM_WIDTH_LIST, wxDefaultPosition, wxSize(200, 240));
    wxASSERT( item4 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item6 = new wxStaticBox( parent, -1, wxT("Guard support:") );
    wxStaticBoxSizer *item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    wxWindow *item7 = new wxCheckListBox(parent, ID_GUARD_SUPPORT_LIST, wxDefaultPosition, wxSize(200, 240));
    wxASSERT( item7 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item10 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
