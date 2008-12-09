/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MemoryTools.cc
 *
 * Definition of MemoryTools class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <string>

#include "MemoryDialog.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "DialogPosition.hh"
#include "OperationContainer.hh"
#include "Conversion.hh"
#include "MemoryControl.hh"
#include "OSEd.hh"

using boost::format;
using std::string;

BEGIN_EVENT_TABLE(MemoryDialog, wxDialog)
    EVT_BUTTON(ID_BUTTON_CLOSE, MemoryDialog::onClose)
END_EVENT_TABLE() 

/**
 * Constructor.
 *
 * @param window Parent window.
 */
MemoryDialog::MemoryDialog(wxWindow* window) : 
    wxDialog(window, -1, _T(""),
             DialogPosition::getPosition(DialogPosition::DIALOG_MEMORY),
             wxDefaultSize, wxRESIZE_BORDER) {
    
    createContents(this, true, true);
    setTexts();

    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->registerListener(OSEdInformer::EVENT_MEMORY, this);
}

/**
 * Destructor.
 */
MemoryDialog::~MemoryDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_MEMORY, point);
}

/**
 * Set texts to widgets.
 */
void
MemoryDialog::setTexts() {
    
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_MEMORY_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_CLOSE),
                          OSEdTextGenerator::TXT_BUTTON_CLOSE);
}

/**
 * Handles the event when dialog is closed.
 */
void
MemoryDialog::onClose(wxCommandEvent&) {
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->unregisterListener(OSEdInformer::EVENT_MEMORY, this);
    Close();
}

/**
 * Handles event when the contents of the memory might have changed.
 */
void
MemoryDialog::handleEvent(OSEdInformer::EventId event) {
    switch(event) {
    case OSEdInformer::EVENT_MEMORY:
        memoryWindow_->updateView();
        break;
    default:
        break;
    }
}

/**
 * Creates the contents of the MemoryDialog.
 *
 * @param parent Parent window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the the main sizer as dialog contents. 
 * @return The created sizer.
 */
wxSizer*
MemoryDialog::createContents(
    wxWindow* parent,
    bool call_fit,
    bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
 
    memoryWindow_ = new MemoryControl(
        parent, &OperationContainer::memory(), MemoryDialog::ID_CONTROL_MEMORY);

    item0->Add(memoryWindow_, 1, wxGROW|wxALIGN_CENTER|wxALL, 5);

    wxButton *item1 = new wxButton( parent, ID_BUTTON_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;

}
