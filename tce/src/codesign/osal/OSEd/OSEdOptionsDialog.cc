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
 * @file src/codesign/osal/OSEd/OSEdOptionsDialog.cc
 *
 * Definition of OSEdOptionsDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <wx/filedlg.h>
#include <boost/format.hpp>
#include <string>

#include "OSEdOptionsDialog.hh"
#include "WxConversion.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "WidgetTools.hh"
#include "OSEd.hh"
#include "OSEdOptionsSerializer.hh"
#include "ErrorDialog.hh"
#include "OSEdConstants.hh"
#include "DialogPosition.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using boost::format;
using std::string;

BEGIN_EVENT_TABLE(OSEdOptionsDialog, wxDialog)

    EVT_BUTTON(ID_BUTTON_SAVE, OSEdOptionsDialog::onSave)
    EVT_BUTTON(ID_BUTTON_BROWSE, OSEdOptionsDialog::onBrowse)

END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent The parent window.
 */
OSEdOptionsDialog::OSEdOptionsDialog(wxWindow* parent) :
    wxDialog(parent, -1, _T(""), 
             DialogPosition::getPosition(DialogPosition::DIALOG_OPTIONS), 
             wxSize(400, 200)) {

    createContents(this, true, true);

    FindWindow(ID_EDITOR)->
        SetValidator(wxTextValidator(wxFILTER_ASCII, &editor_));
	
    setTexts();
}

/**
 * Destructor.
 */
OSEdOptionsDialog::~OSEdOptionsDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_OPTIONS, point);
}

/**
 * Set texts to all widgets.
 */
void
OSEdOptionsDialog::setTexts() {
    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_OPTIONS_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT),
                          OSEdTextGenerator::TXT_LABEL_EDITOR);

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_SAVE),
                          OSEdTextGenerator::TXT_BUTTON_SAVE);

    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_BROWSE),
                          OSEdTextGenerator::TXT_BUTTON_BROWSE);
}

/**
 * Transfers data to window.
 *
 * @return True if all is successful.
 */
bool
OSEdOptionsDialog::TransferDataToWindow() {
    OSEdOptions* options = wxGetApp().options();
    editor_ = WxConversion::toWxString(options->editor());
    return wxWindow::TransferDataToWindow();
}

/**
 * Handles the event when Save button is pushed.
 */
void
OSEdOptionsDialog::onSave(wxCommandEvent&) {
    TransferDataFromWindow();
    wxGetApp().options()->setEditor(WxConversion::toString(editor_));
    OSEdOptionsSerializer serializer;
    string confFile = Environment::confDirPath(OSEdConstants::CONF_FILE_NAME);
    serializer.setDestinationFile(confFile);
    try {
        serializer.writeState(wxGetApp().options()->saveState());
    } catch (const Exception& e) {
        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_CAN_NOT_SAVE);
        ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
    }
    EndModal(wxID_OK);
}

/**
 * Handles the event when Browse button is pushed.
 */
void
OSEdOptionsDialog::onBrowse(wxCommandEvent&) {

    string path = FileSystem::directoryOfPath(WxConversion::toString(editor_));
    wxFileDialog dialog(this, _T("Choose a file"), 
                        WxConversion::toWxString(path));
    if (dialog.ShowModal() == wxID_OK) {
        editor_ = dialog.GetPath();
        wxWindow::TransferDataToWindow();
    }
}

/**
 * Creates the contents of the dialog.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contents inside the dialog.
 * @param set_sizer If true sets the main sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
OSEdOptionsDialog::createContents(
    wxWindow *parent, 
    bool call_fit, 
    bool set_sizer) {
	
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    
    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
    
    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Source code editor:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );
    
    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_EDITOR, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_BUTTON_BROWSE, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item6 = new wxButton( parent, ID_BUTTON_SAVE, wxT("&Save"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item7 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
