/**
 * @file TemplateSlotDialog.cc
 *
 * Implementation of TemplateSlotDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "TemplateSlotDialog.hh"
#include "Machine.hh"
#include "InstructionTemplate.hh"
#include "WxConversion.hh"
#include "AssocTools.hh"
#include "WidgetTools.hh"
#include "InformationDialog.hh"
#include "ModelConstants.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "Application.hh"
#include "ImmediateSlot.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(TemplateSlotDialog, wxDialog)
    EVT_BUTTON(wxID_OK, TemplateSlotDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param temp InstructionTemplate to edit.
 * @param slot Slot to edit, NULL if a new slot is being added.
 */
TemplateSlotDialog::TemplateSlotDialog(
    wxWindow* parent,
    InstructionTemplate* it,
    Bus* slot) :
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    slot_(slot),
    it_(it),
    width_(ModelConstants::DEFAULT_WIDTH),
    destination_(NULL) {

    createContents(this, true, true);

    slotChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_SLOT));
    destinationChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_DESTINATION));
    widthCtrl_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_WIDTH));

    widthCtrl_->SetValidator(wxGenericValidator(&width_));

    // Read slot attributes.
    if (slot_ != NULL) {
        width_ = it_->supportedWidth(slot_->name());
        Machine::ImmediateUnitNavigator navigator =
            it_->machine()->immediateUnitNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            if (it_->destinationUsesSlot(
                    slot_->name(), *navigator.item(i))) {
                destination_ = navigator.item(i);
            }
        }
    }

    // set widget texts
    setTexts();
}


/**
 * The Destructor.
 */
TemplateSlotDialog::~TemplateSlotDialog() {
}


/**
 * Sets texts for widgets.
 */
void
TemplateSlotDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_TEMPLATE_SLOT_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);


    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_SLOT),
                          ProDeTextGenerator::TXT_LABEL_SLOT);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_DESTINATION),
                          ProDeTextGenerator::TXT_LABEL_DESTINATION);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);
}


/**
 * Transfers data from the InstructionTemplate object to the dialog widgets.
 *
 * @return true, if the transfer was succesful, false otherwise.
 */
bool
TemplateSlotDialog::TransferDataToWindow() {

    // Set slot choicer.
    slotChoice_->Clear();

    if (slot_ == NULL) {
        // A new slot is being added, set all available busses to the
        // slot choice.
        Machine::BusNavigator navigator = it_->machine()->busNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            if (!(it_->usesSlot(navigator.item(i)->name()))) {
                wxString name =
                    WxConversion::toWxString(navigator.item(i)->name());
                slotChoice_->Append(name);
            }
        }
	// Add immediate slots.
        Machine::ImmediateSlotNavigator immSlotNavigator =
	    it_->machine()->immediateSlotNavigator();
        for (int i = 0; i < immSlotNavigator.count(); i++) {
            if (!(it_->usesSlot(immSlotNavigator.item(i)->name()))) {
                wxString name =
                    WxConversion::toWxString(immSlotNavigator.item(i)->name());
                slotChoice_->Append(name);
            }
        }
    } else {
        // An old slot is being modified, set the slot and disable the control.
        slotChoice_->Append(WxConversion::toWxString(slot_->name()));
        slotChoice_->Disable();
    }

    // Set destination choicer.
    destinationChoice_->Clear();
    Machine::ImmediateUnitNavigator navigator =
        it_->machine()->immediateUnitNavigator();
    for (int i = 0; i < navigator.count(); i++) {
        wxString name =
            WxConversion::toWxString(navigator.item(i)->name());
        destinationChoice_->Append(name);
    }

    // Set choicer selections.
    slotChoice_->SetSelection(0);
    if (destination_ == NULL) {
        destinationChoice_->SetSelection(0);
    } else {
        wxString name = WxConversion::toWxString(destination_->name());
        destinationChoice_->SetStringSelection(name);
    }
    return wxDialog::TransferDataToWindow();
}


/**
 * Creates a new tempalte slot based on the dialog widget values.
 */
void
TemplateSlotDialog::onOK(wxCommandEvent&) {

    // delete old slot
    if (slot_ != NULL) {
        // Currently editing slots is not possible due to the restrictions
        // in the MOM interface.
        assert(false);
    }
    TransferDataFromWindow();
    string slotName =
        WxConversion::toString(slotChoice_->GetStringSelection());
    
    const Machine::BusNavigator busNavigator = it_->machine()->busNavigator();
    const Machine::ImmediateSlotNavigator immsNavigator =
        it_->machine()->immediateSlotNavigator();
    
    destination_ = it_->machine()->immediateUnitNavigator().item(
        WxConversion::toString(destinationChoice_->GetStringSelection()));

    if (busNavigator.hasItem(slotName)) {
        Bus* slot = it_->machine()->busNavigator().item(slotName);
        // create the new slot
        it_->addSlot(slot->name(), width_, *destination_);
    } else if (immsNavigator.hasItem(slotName)) {
        ImmediateSlot* slot = immsNavigator.item(slotName);
        // create the new slot
        it_->addSlot(slot->name(), width_, *destination_);
    } else {
        assert(false);
    }

    EndModal(wxID_OK);
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
TemplateSlotDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_SLOT, wxT("Slot:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs3 = (wxString*) NULL;
    wxChoice *item3 = new wxChoice( parent, ID_SLOT, wxDefaultPosition, wxSize(150,-1), 0, strs3, 0 );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_DESTINATION, wxT("Destination:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_DESTINATION, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item7 = new wxSpinCtrl( parent, ID_WIDTH, wxT("1"), wxDefaultPosition, wxSize(100,-1), 0, 1, 10000, 1 );
    item1->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item9 = new wxFlexGridSizer( 2, 0, 0 );

    wxButton *item10 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item12 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
