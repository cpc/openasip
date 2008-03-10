/**
 * @file FUPortImplementationDialog.cc
 *
 * Implementation of FUPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/listctrl.h>
#include "FUPortImplementationDialog.hh"
#include "FUPortImplementation.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Application.hh"
#include "ExecutionPipeline.hh"

using namespace HDB;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(FUPortImplementationDialog, wxDialog)
    EVT_BUTTON(wxID_OK, FUPortImplementationDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param implementation FU port implementation to modify.
 * @param architecthre Architecture of the modified port.
 */
FUPortImplementationDialog::FUPortImplementationDialog(
    wxWindow* parent, wxWindowID id, FUPortImplementation& implementation,
    const BaseFUPort& architecture) :
    wxDialog(parent, id, _T("Function Unit Port Implementation")),
    implementation_(implementation), architecture_(architecture) {

    createContents(this, true, true);

    operandList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERAND_LIST));
    assert(operandList_ != NULL);
    operandList_->InsertColumn(
        0, _T("Operation"), wxLIST_FORMAT_LEFT, 140);

    operandList_->InsertColumn(
        1, _T("R/W"), wxLIST_FORMAT_LEFT, 60);

    operandList_->InsertColumn(
        2, _T("io"), wxLIST_FORMAT_LEFT, 60);

    name_ = WxConversion::toWxString(implementation_.name());
    loadPortName_ = WxConversion::toWxString(
        implementation_.loadPort());

    guardPortName_ = WxConversion::toWxString(
        implementation_.guardPort());

    widthFormula_ = WxConversion::toWxString(
        implementation_.widthFormula());

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_LOAD_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &loadPortName_));
    FindWindow(ID_GUARD_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &guardPortName_));
    FindWindow(ID_WIDTH)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &widthFormula_));

    dynamic_cast<wxStaticText*>(FindWindow(ID_ARCHITECTURE_PORT_NAME))->
        SetLabel(WxConversion::toWxString(implementation_.architecturePort()));

    TransferDataToWindow();
}

/**
 * The Destructor.
 */
FUPortImplementationDialog::~FUPortImplementationDialog() {
}


/**
 * Transfers data from the dialog attributes to the dialog widgets.
 *
 * @return True, if the data was succesfully transferred.
 */
bool
FUPortImplementationDialog::TransferDataToWindow() {

    operandList_->DeleteAllItems();
    const FunctionUnit* fu = architecture_.parentUnit();
    const FUPort* fuPort = dynamic_cast<const FUPort*>(&architecture_);
    assert(fu != NULL);
    int item = 0;
    for (int i = 0; i < fu->operationCount(); i++) {
        const HWOperation* operation = fu->operation(i);
        if (fuPort != NULL && operation->isBound(*fuPort)) {
            wxString rw;
            bool read = false;
            bool written = false;
            for (int cycle = 0; cycle < operation->latency(); cycle++) {
                if (operation->pipeline()->isPortRead(*fuPort, cycle)) {
                    read = true;
                }                
                if (operation->pipeline()->isPortWritten(*fuPort, cycle)) {
                    written = true;
                }
            }
            if (read) {
                rw.Append(_T("R"));
            }
            if (written) {
                rw.Append(_T("W"));
            }
            operandList_->InsertItem(
                item, WxConversion::toWxString(operation->name()));

            operandList_->SetItem(item, 1, rw);

            operandList_->SetItem(
                item, 2, WxConversion::toWxString(operation->io(*fuPort)));

            item++;
        }
    }

    return wxDialog::TransferDataToWindow();
}

/**
 * Event handler for the dialog OK-button.
 */
void
FUPortImplementationDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    loadPortName_ = loadPortName_.Trim(true).Trim(false);
    guardPortName_ = guardPortName_.Trim(true).Trim(false);
    widthFormula_ = widthFormula_.Trim(true).Trim(false);

    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    if (widthFormula_.IsEmpty()) {
        wxString message = _T("Width formula field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    implementation_.setName(WxConversion::toString(name_));
    implementation_.setLoadPort(WxConversion::toString(loadPortName_));
    implementation_.setGuardPort(WxConversion::toString(guardPortName_));
    implementation_.setWidthFormula(WxConversion::toString(widthFormula_));

    EndModal(wxID_OK);
}

/**
 * Creates the dialog contents.
 */
wxSizer*
FUPortImplementationDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_ARCHITECTURE_PORT, wxT("Architecture port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_ARCHITECTURE_PORT_NAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LOAD_PORT_LABEL, wxT("Load port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_LOAD_PORT, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_GUARD_PORT_LABEL, wxT("Guard port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_GUARD_PORT, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width formula:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_WIDTH, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item14 = new wxStaticBox( parent, -1, wxT("Operand bindings:") );
    wxStaticBoxSizer *item13 = new wxStaticBoxSizer( item14, wxVERTICAL );

    wxListCtrl *item15 = new wxListCtrl( parent, ID_OPERAND_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item13->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item16 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item17 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item18 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item18, 0, wxALIGN_CENTER, 5 );

    wxButton *item19 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
