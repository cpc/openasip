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
 * @file HelpBrowser.cc
 *
 * Implementation of the HelpBrowser class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/string.h>
#include <wx/menu.h>
#include <wx/html/htmlwin.h>
#include <wx/toolbar.h>
#include <wx/image.h>

#include "HelpBrowser.hh"
#include "WxConversion.hh"
#include "Environment.hh"
#include "FileSystem.hh"

BEGIN_EVENT_TABLE(HelpBrowser, wxFrame)
    EVT_CLOSE(HelpBrowser::onClose)
    EVT_MENU(ID_BACK, HelpBrowser::onBack)
    EVT_MENU(ID_FORWARD, HelpBrowser::onForward)
    EVT_MENU(wxID_CLOSE, HelpBrowser::onMenuClose)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * Creates a browser window and opens the given file in the
 * html-widget.
 *
 * @param parent Parent window of the window.
 * @param title Title of the help browser frame.
 * @param file Html file  to open in the frame.
 * @param position Help frame position on the screen.
 * @param size Help frame size.
 */
HelpBrowser::HelpBrowser(wxString title, wxString file,
                        wxPoint position, wxSize size):
    wxFrame(NULL, -1, title, position, size), html_(NULL) {

    wxToolBar* toolbar =
        new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize,
                      wxTB_HORIZONTAL | wxNO_BORDER | wxTB_TEXT);

    wxBitmap icon;

	if (wxImage::FindHandler(wxBITMAP_TYPE_PNG) == NULL) {
		// load image handler for pngs
		wxImage::AddHandler(new wxPNGHandler);
	}

    // toolbar icon locations
    wxString backIconFile = WxConversion::toWxString(
        Environment::bitmapsDirPath("ProDe") +
        FileSystem::DIRECTORY_SEPARATOR +
        "back.png");
    wxString forwardIconFile = WxConversion::toWxString(
        Environment::bitmapsDirPath("ProDe") +
        FileSystem::DIRECTORY_SEPARATOR +
        "forward.png");
    wxString closeIconFile = WxConversion::toWxString(
        Environment::bitmapsDirPath("ProDe") +
        FileSystem::DIRECTORY_SEPARATOR +
        "close_document.png");

    // add toolbar buttons
    icon.LoadFile(closeIconFile, wxBITMAP_TYPE_PNG);

    toolbar->AddTool(wxID_CLOSE, _T("Close"), icon,
                     wxNullBitmap, wxITEM_NORMAL, _T("Close window"),
                     _T("CloseWindow"));

    toolbar->AddSeparator();

    icon.LoadFile(backIconFile, wxBITMAP_TYPE_PNG);
    toolbar->AddTool(ID_BACK, _T("Back"), icon,
                     wxNullBitmap, wxITEM_NORMAL, _T("Back"));

    icon.LoadFile(forwardIconFile, wxBITMAP_TYPE_PNG);
    toolbar->AddTool(ID_FORWARD, _T("Forward"), icon,
                     wxNullBitmap, wxITEM_NORMAL, _T("Forward"));

    toolbar->Realize();
    SetToolBar(toolbar);

	// create html-widget and open the given file in it
    html_ = new wxHtmlWindow(this);
    html_->LoadPage(file);
}


/**
 * The Destructor.
 */
HelpBrowser::~HelpBrowser() {
}


/**
 * Destroys the window when a close event is received.
 */
void
HelpBrowser::onClose(wxCloseEvent&) {
    Destroy();
}

/**
 * Destroys the window when a close event is received.
 */
void
HelpBrowser::onMenuClose(wxCommandEvent&) {
    Destroy();
}


/**
 * Loads the previous page in the html-widget.
 */
void
HelpBrowser::onBack(wxCommandEvent&) {
    html_->HistoryBack();
}


/**
 * Loads the next page in html-widget history.
 */
void
HelpBrowser::onForward(wxCommandEvent&) {
    html_->HistoryForward();
}
