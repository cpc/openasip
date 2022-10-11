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
 * @file OSEdAboutDialog.cc
 *
 * Definition of OSEdAboutDialog class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <string>
#include <wx/statline.h>

#include "OSEdAboutDialog.hh"
#include "WidgetTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "GUITextGenerator.hh"
#include "OSEdConstants.hh"
#include "DialogPosition.hh"
#include "tce_config.h"

using boost::format;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent window.
 */
OSEdAboutDialog::OSEdAboutDialog(wxWindow* parent) : 
    wxDialog(parent, -1, _T(""),
             DialogPosition::getPosition(DialogPosition::DIALOG_ABOUT),
             wxDefaultSize) {

    createContents(this, true, true);
    setTexts();
}

/**
 * Destructor.
 */
OSEdAboutDialog::~OSEdAboutDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_ABOUT, point);
}

/**
 * Sets the texts for the dialog.
 */
void
OSEdAboutDialog::setTexts() {

    OSEdTextGenerator& osedTexts = OSEdTextGenerator::instance();
    GUITextGenerator& guiTexts = *GUITextGenerator::instance();

    // title
    format fmt = osedTexts.text(OSEdTextGenerator::TXT_ABOUT_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // static texts
    WidgetTools::setLabel(&osedTexts, FindWindow(ID_ABOUT_TEXT),
                          OSEdTextGenerator::TXT_ABOUT);

    // buttons
    WidgetTools::setLabel(&guiTexts, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);
}

/**
 * Creates the contents of the dialog.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contents inside the dialog.
 * @param set_sizer If true, sets the sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
OSEdAboutDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxBitmap logo;

    string dir = Environment::bitmapsDirPath(
        WxConversion::toString(OSEdConstants::APPLICATION_NAME));

    string fileName = dir + FileSystem::DIRECTORY_SEPARATOR +
        OSEdConstants::LOGO_NAME;

    logo.LoadFile(WxConversion::toWxString(fileName), wxBITMAP_TYPE_PNG);
    wxStaticBitmap* dialogLogo = new wxStaticBitmap(parent, -1, logo);

    item0->Add(dialogLogo, 0, wxALIGN_CENTER|wxALL, 5 );
    wxBoxSizer *item3 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *item4 =
	new wxStaticText(parent, ID_ABOUT_TEXT, wxT("Operation Set Editor"),
			 wxDefaultPosition, wxDefaultSize, 0);
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *item5 =
	new wxStaticText(parent, -1, _T("OpenASIP ") + WxConversion::toWxString(VERSION),
			 wxDefaultPosition, wxDefaultSize, 0);
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );
    
     wxStaticText *item6 =
    new wxStaticText(parent, -1, OSEdConstants::OSED_COPYRIGHT,
             wxDefaultPosition, wxDefaultSize, 0);
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticLine *item7 =
	new wxStaticLine(parent, -1, wxDefaultPosition,
			 wxSize(20,-1), wxLI_HORIZONTAL);
    item0->Add( item7, 0, wxGROW|wxALL, 5 );
    wxButton *item8 =
	new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item0->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );
    if (set_sizer) {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }

    return item0;
}
