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
 * @file ChildFrame.hh
 *
 * Declaration of ChildFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
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
