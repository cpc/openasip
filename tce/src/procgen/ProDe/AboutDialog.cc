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
 * @file AboutDialog.cc
 *
 * Definition of AboutDialog class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
 */

#include <string>
#include <boost/format.hpp>

#include "AboutDialog.hh"
#include "ProDeConstants.hh"
#include "WxConversion.hh"
#include "ProDe.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "WidgetTools.hh"
#include "tce_config.h"

using boost::format;
using std::string;

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 */
AboutDialog::AboutDialog(wxWindow* parent):
    wxDialog(parent, -1, _T(""), wxDefaultPosition) {

    createContents(this, true, true);

    // set widget texts
    setTexts();
}

/**
 * The Destructor.
 */
AboutDialog::~AboutDialog() {
}


/**
 * Sets texts for widgets.
 */
void
AboutDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_ABOUT_DIALOG_TITLE);
    string title = fmt.str();
    title.append(WxConversion::toString(ProDeConstants::EDITOR_NAME));
    SetTitle(WxConversion::toWxString(title));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);
}


/**
 * Creates contents of the dialog window. Initially generated with
 * wxDesigner, the code will be cleaned up later.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits sizer in dialog window.
 * @param set_sizer If true, sets sizer as dialog's sizer.
 * @return Top level sizer of the contents.
 */
wxSizer*
AboutDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxBitmap logo;

    wxString filename = WxConversion::toWxString(
         ProDe::bitmapsDirPath() + ProDeConstants::EDITOR_LOGO_PATH);

    logo.LoadFile(filename, wxBITMAP_TYPE_PNG);
    wxStaticBitmap* dialogLogo = new wxStaticBitmap(parent, -1, logo);

    item0->Add(dialogLogo, 0, wxALIGN_CENTER|wxALL, 5 );
    wxBoxSizer *item3 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *item4 =
	new wxStaticText(parent, -1, ProDeConstants::EDITOR_NAME,
			 wxDefaultPosition, wxDefaultSize, 0);
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *item5 =
	new wxStaticText(parent, -1, _T("TCE ") + WxConversion::toWxString(VERSION),
			 wxDefaultPosition, wxDefaultSize, 0);
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );
    wxStaticText *item6 =
	new wxStaticText(parent, -1,
			 ProDeConstants::EDITOR_COPYRIGHT_TEXT,
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
