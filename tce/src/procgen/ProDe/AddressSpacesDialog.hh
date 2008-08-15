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
 * @file AddressSpacesDialog.hh
 *
 * Declaration of AddressSpacesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACES_DIALOG_HH
#define TTA_ADDRESS_SPACES_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "Machine.hh"

class wxListCtrl;

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class AddressSpacesDialog : public wxDialog {
public:
    AddressSpacesDialog(wxWindow* parent, TTAMachine::Machine* machine);
    ~AddressSpacesDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    TTAMachine::AddressSpace* selectedAS();
    void updateASList();
    void onAdd(wxCommandEvent& event);
    void onActivateAS(wxListEvent& event);
    void onEdit(wxCommandEvent& event);
    void onDelete(wxCommandEvent& event);
    void onASSelection(wxListEvent& event);
    void onASRightClick(wxListEvent& event);
    void setTexts();

    /// Machine containing the address spaces to list.
    TTAMachine::Machine* machine_;
    /// List control for listing the address spaces.
    wxListCtrl* asList_;

    // enumerated ids for dialog controls
    enum {
        ID_ADD = 10000,
        ID_EDIT,
        ID_HELP,
        ID_DELETE,
        ID_LIST,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
