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
