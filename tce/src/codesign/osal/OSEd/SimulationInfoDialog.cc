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
 * @file SimulationInfoDialog.cc
 *
 * Definition of SimulationInfoDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <string>

#include "SimulationInfoDialog.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "SimulationInfoDialog.hh"
#include "OperationContext.hh"
#include "WxConversion.hh"
#include "DialogPosition.hh"
#include "OSEdConstants.hh"

using boost::format;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent window.
 * @param context Operation context.
 */
SimulationInfoDialog::SimulationInfoDialog(
    wxWindow* parent, 
    OperationContext* context) :
    wxDialog(parent, -1, _T(""),
             DialogPosition::getPosition(DialogPosition::DIALOG_SIMULATION_INFO),
             wxSize(400, 150), wxRESIZE_BORDER), context_(context) {

    createContents(this, false, true);

    parent_ = dynamic_cast<SimulateDialog*>(parent);

    infoList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LISTCTRL));
    infoList_->InsertColumn(0, _T(""), wxLIST_FORMAT_LEFT, 
                           OSEdConstants::DEFAULT_COLUMN_WIDTH);
    infoList_->InsertColumn(1, _T(""), wxLIST_FORMAT_LEFT, 
                           OSEdConstants::DEFAULT_COLUMN_WIDTH);

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();

    format fmt = texts.text(OSEdTextGenerator::TXT_LABEL_PC);
    infoList_->InsertItem(0, WxConversion::toWxString(fmt.str()));

    fmt = texts.text(OSEdTextGenerator::TXT_LABEL_RA);
    infoList_->InsertItem(1, WxConversion::toWxString(fmt.str()));

    fmt = texts.text(OSEdTextGenerator::TXT_LABEL_SYS_CALL_HANDLER);
    infoList_->InsertItem(2, WxConversion::toWxString(fmt.str()));

    fmt = texts.text(OSEdTextGenerator::TXT_LABEL_SYS_CALL_NUMBER);
    infoList_->InsertItem(3, WxConversion::toWxString(fmt.str()));

    setTexts();
}

/**
 * Sets the texts for the widgets.
 */
void
SimulationInfoDialog::setTexts() {

    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();
   
    // title
    format fmt = 
        osedText.text(OSEdTextGenerator::TXT_SIMULATION_INFO_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

}


/**
 * Destructor.
 */
SimulationInfoDialog::~SimulationInfoDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_SIMULATION_INFO, point);
}

/**
 * Transfer data to dialog.
 *
 * @return True if transfer is successful.
 */
bool
SimulationInfoDialog::TransferDataToWindow() {    
    updateList();
    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the list of information.
 */
void
SimulationInfoDialog::updateList() {
#if 0
    if (&context_->programCounter() == &NullSimValue::instance()) {
        infoList_->SetItem(0, 1, WxConversion::toWxString(0));
    } else {
        SimValue& value = context_->programCounter();
        string valueString = parent_->formattedValue(&value);
        infoList_->SetItem(0, 1, WxConversion::toWxString(valueString));
    }
#endif
    infoList_->SetItem(
        0, 1, WxConversion::toWxString(context_->programCounter()));

#if 0    
    if (&context_->returnAddress() == &NullSimValue::instance()) {
        infoList_->SetItem(1, 1, WxConversion::toWxString(0));
    } else {
        SimValue& value = context_->returnAddress();
        string valueString = parent_->formattedValue(&value);
        infoList_->SetItem(1, 1, WxConversion::toWxString(valueString));
    }
#endif
    infoList_->SetItem(
        1, 1,
        WxConversion::toWxString(context_->returnAddress().uIntWordValue()));
}

/**
 * Updates the list which contains information about simulation.
 *
 * This method is called when trigger button is pushed in parent dialog
 * (SimulateDialog).
 *
 * @param id The id of the event. 
 */
void
SimulationInfoDialog::handleEvent(OSEdInformer::EventId id) {
    switch (id) {
    case OSEdInformer::EVENT_REGISTER:
        updateList();
        break;
    case OSEdInformer::EVENT_RESET:
        context_->programCounter() = 0;
        context_->returnAddress() = 0;
        updateList();
        break;
    default:
        break;
    }
}

/**
 * Creates the contents of the dialog.
 *
 * NOTE! This function was generated by wxDesigner.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contenst inside the dialog.
 * @param set_sizer If true sets the main sizer as dialog contents. 
 * @return The created sizer.
 */
wxSizer*
SimulationInfoDialog::createContents(
    wxWindow* parent, 
    bool call_fit, 
    bool set_sizer ) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_LISTCTRL, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxLC_NO_HEADER|wxSUNKEN_BORDER );
    item0->Add( item1, 1, wxGROW|wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
