/**
 * @file ChildFrame.hh
 *
 * Declaration of ChildFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CHILD_FRAME
#define TTA_CHILD_FRAME

#include <wx/docmdi.h>


/**
 * MDI child frame for displaying a document.
 */
class ChildFrame : public wxDocMDIChildFrame {
public:
    ChildFrame(wxDocument* doc, wxView* view, wxDocMDIParentFrame* parent);
    ~ChildFrame();

    void setStatus(const wxString text);

private:
    void onFocus(wxFocusEvent& event);

    /// Document which is displayed in the frame.
    wxDocument* document_;
    /// View of the document which is displayed in the frame.
    wxView* view_;
    /// MDI parent frame of the ChildFrame.
    wxDocMDIParentFrame* parent_;

    DECLARE_EVENT_TABLE()
};

#endif
