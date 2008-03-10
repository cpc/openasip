/**
 * @file AddressSpaceDialog.cc
 *
 * Definition of AddressSpaceDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include <boost/format.hpp>

#include "AddressSpaceDialog.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "WarningDialog.hh"
#include "AddressSpace.hh"
#include "ModelConstants.hh"
#include "NumberControl.hh"
#include "InformationDialog.hh"
#include "MachineTester.hh"
#include "GUITextGenerator.hh"
#include "WidgetTools.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(AddressSpaceDialog, wxDialog)
    EVT_TEXT(ID_NAME, AddressSpaceDialog::onName)
    EVT_BUTTON(wxID_OK, AddressSpaceDialog::onOK)
    EVT_TEXT(ID_MIN_ADDRESS, AddressSpaceDialog::onMinAddress)
    EVT_TEXT(ID_MAX_ADDRESS, AddressSpaceDialog::onMaxAddress)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param addressSpace The address space to be modified with the dialog.
 */
AddressSpaceDialog::AddressSpaceDialog(
    wxWindow* parent,
    AddressSpace* addressSpace):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    as_(addressSpace),
    name_(_T("")),
    width_(ModelConstants::DEFAULT_WIDTH),
    nameSizer_(NULL),
    minAddressSizer_(NULL),
    maxAddressSizer_(NULL),
    widthSizer_(NULL) {

    createContents(this, true, true);

    // disable conditional buttons initially
    FindWindow(wxID_OK)->Disable();

    // set validators for dialog controls
    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_WIDTH)->SetValidator(
        wxGenericValidator(&width_));

    // set min and max adress spin button ranges
    minControl_ = dynamic_cast<NumberControl*>(FindWindow(ID_MIN_ADDRESS));
    maxControl_ = dynamic_cast<NumberControl*>(FindWindow(ID_MAX_ADDRESS));

    // set widget texts
    setTexts();

    TransferDataToWindow();

    FindWindow(ID_NAME)->SetFocus();
}


/**
 * The Destructor.
 */
AddressSpaceDialog::~AddressSpaceDialog() {
}


/**
 * Sets texts for widgets.
 */
void
AddressSpaceDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_ADDRESS_SPACE_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_LABEL_NAME);
    WidgetTools::setWidgetLabel(nameSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_LABEL_MAU);
    WidgetTools::setWidgetLabel(widthSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_ADDRESS_SPACE_MIN_ADDRESS);
    WidgetTools::setWidgetLabel(minAddressSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_ADDRESS_SPACE_MAX_ADDRESS);
    WidgetTools::setWidgetLabel(maxAddressSizer_, fmt.str());
}


/**
 * Transfers data from the AddressSpace object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer, true otherwise.
 */
bool
AddressSpaceDialog::TransferDataToWindow() {

    name_ = WxConversion::toWxString(as_->name());
    width_ = as_->width();

    minControl_->setValue(as_->start());
    maxControl_->setValue(as_->end());
    //minControl_->setRange(0, as_->end() - 1);
    //maxControl_->setRange(as_->start() + 1, 0xFFFFFFFF);

    // wxWidgets GTK1 version seems to bug with spincontrol validators.
    // The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_WIDTH))->SetValue(width_);

    wxCommandEvent dummy;
    onMaxAddress(dummy);
    onMinAddress(dummy);


    return wxWindow::TransferDataToWindow();
}


/**
 * Validates input in the controls, and updates the AddressSpace.
 */
void
AddressSpaceDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    string trimmedName =
	WxConversion::toString(name_.Trim(false).Trim(true));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    if (trimmedName != as_->name()) {

        // Check that the new address space name is unique among
        // all address spaces in the machine.
	Machine::AddressSpaceNavigator navigator =
	    as_->machine()->addressSpaceNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            string asName = navigator.item(i)->name();
            if (trimmedName == asName) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format an_as = prodeTexts->text(
                    ProDeTextGenerator::COMP_AN_ADDRESS_SPACE);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
                format as =
                    prodeTexts->text(ProDeTextGenerator::COMP_ADDRESS_SPACE);
                message % trimmedName % an_as.str() % machine.str() % as.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    as_->setName(trimmedName);
    as_->setWidth(width_);
    unsigned int minAddr = minControl_->unsignedValue();
    unsigned int maxAddr = maxControl_->unsignedValue();
    as_->setAddressBounds(minAddr, maxAddr);

    EndModal(wxID_OK);
}


/**
 * Checks whether the name field is empty, and disables OK button of the
 * dialog if it is.
 */
void
AddressSpaceDialog::onName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = name_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(wxID_OK)->Disable();
    } else {
        FindWindow(wxID_OK)->Enable();
    }
}


/**
 * Sets the range of the MaxAddress spin-button.
 */
void
AddressSpaceDialog::onMinAddress(wxCommandEvent&) {
    if (minControl_->unsignedValue() >= maxControl_->unsignedValue()) {
        minControl_->setValue(maxControl_->unsignedValue() - 1);
    }
}


/**
 * Sets the range of the MinAddress spin-button.
 */
void
AddressSpaceDialog::onMaxAddress(wxCommandEvent&) {
    if (maxControl_->unsignedValue() <= minControl_->unsignedValue()) {
        maxControl_->setValue(minControl_->unsignedValue() + 1);
    }
}

/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner, thus the ugly code and
 * too long lines.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
AddressSpaceDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Name:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
    nameSizer_ = item2;

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item6 = new wxStaticBox( parent, -1, wxT("Min-Address") );
    wxStaticBoxSizer *item5 = new wxStaticBoxSizer( item6, wxVERTICAL );
    minAddressSizer_ = item5;

    NumberControl* item7 = new NumberControl(parent, ID_MIN_ADDRESS, wxDefaultPosition, wxSize(140, 20), (NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL));
    wxASSERT( item7 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item9 = new wxStaticBox( parent, -1, wxT("Width:") );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item9, wxVERTICAL );
    widthSizer_ = item8;

    wxSpinCtrl *item10 = new wxSpinCtrl( parent, ID_WIDTH, wxT("1"), wxDefaultPosition, wxSize(100,-1), 0, 1, 10000, 1 );
    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item12 = new wxStaticBox( parent, -1, wxT("Max-Address") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
    maxAddressSizer_ = item11;

    NumberControl* item13 = new NumberControl(parent, ID_MAX_ADDRESS, wxDefaultPosition, wxSize(140, 20), (NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL));
    wxASSERT( item13 );
    item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item14 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item15 = new wxGridSizer( 2, 0, 0 );

    wxButton *item16 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item17 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item18 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item18, 0, wxALIGN_CENTER, 5 );

    wxButton *item19 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    item15->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
