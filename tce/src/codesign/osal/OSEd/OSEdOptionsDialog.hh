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
 * @file src/codesign/osal/OSEd/OSEdOptionsDialog.hh
 *
 * Declaration of OSEdOptionsDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_DIALOG_HH
#define TTA_OSED_OPTIONS_DIALOG_HH

#include <wx/wx.h>

/**
 * Dialog for modifying OSEd options.
 */
class OSEdOptionsDialog : public wxDialog {
public:
    OSEdOptionsDialog(wxWindow* parent);
    virtual ~OSEdOptionsDialog();

private:
    /// Copying not allowed.
    OSEdOptionsDialog(const OSEdOptionsDialog&);
    /// Assignment not allowed.
    OSEdOptionsDialog& operator=(const OSEdOptionsDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();
    bool TransferDataToWindow();
    void onSave(wxCommandEvent& event);
    void onBrowse(wxCommandEvent& event);

    /**
     * Widget ids.
     */
    enum {
        ID_TEXT = 1000000,
        ID_EDITOR,
        ID_BUTTON_SAVE,
        ID_BUTTON_BROWSE
    };

    /// The name of the editor.
    wxString editor_;

    DECLARE_EVENT_TABLE()
};

#endif
