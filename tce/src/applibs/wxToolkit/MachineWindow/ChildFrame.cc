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
 * @file ChildFrame.cc
 *
 * Definition of ChildFrame class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
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
    view_(view), parent_(parent) {

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
 * @param field Field of status bar to set text for.
 */
void
ChildFrame::setStatus(const wxString text, int field) {
    wxStatusBar* statusbar = parent_->GetStatusBar();
    if (statusbar != NULL && statusbar->GetFieldsCount() > field) {
        statusbar->SetStatusText(text, field);
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
