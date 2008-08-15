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
 * @file ChildFrame.cc
 *
 * Definition of ChildFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "ChildFrame.hh"
#include "MDFDocument.hh"
#include "MDFView.hh"

using std::string;

BEGIN_EVENT_TABLE(ChildFrame, wxDocMDIChildFrame)
    EVT_SET_FOCUS(ChildFrame::onFocus)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param doc Document which is displayed in the ChildFrame.
 * @param view View of the document to display.
 * @param parent MDI parent frame of the child frame.
 */
ChildFrame::ChildFrame(
    wxDocument* doc,
    wxView* view,
    wxDocMDIParentFrame* parent):
    wxDocMDIChildFrame(doc, view, parent, -1, _T("ChildFrame")),
    document_(doc), view_(view), parent_(parent) {

}


/**
 * The Destructor.
 */
ChildFrame::~ChildFrame() {
}


/**
 * Sets the statusbar text.
 *
 * @param text New text for the statusbar.
 */
void
ChildFrame::setStatus(const wxString text) {
    wxStatusBar* statusbar = parent_->GetStatusBar();
    if (statusbar != NULL) {
	statusbar->SetStatusText(text);
    }
}

/**
 * Activates the associated view when a ChildFrame instance gains focus.
 *
 * Correct views are not always activated by the wxWidgets document/view
 * framework when documents are closed. This function makes sure that the
 * view associated with the focused ChildFrame is activated.
 */
void
ChildFrame::onFocus(wxFocusEvent& event) {
    view_->Activate(true);
    event.Skip();
}
