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
 * @file FUExternalPortDialog.hh
 *
 * Declaration of FUExternalPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_EXTERNAL_PORT_DIALOG_HH
#define TTA_FU_EXTERNAL_PORT_DIALOG_HH

#include <wx/wx.h>

class wxCheckListBox;

namespace HDB {
    class FUExternalPort;
    class FUImplementation;
}

/**
 * Dialog for editing FU external ports.
 */
class FUExternalPortDialog : public wxDialog {
public:
    FUExternalPortDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUExternalPort& implementation,
        const HDB::FUImplementation& fu);

    virtual ~FUExternalPortDialog();

private:
    void initialize();
    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESCRIPTION,
        ID_DESCRIPTION,
        ID_DIRECTION,
        ID_PARAMETER_DEPS,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUExternalPort& port_;
    /// Parent FU implementation of the port.
    const HDB::FUImplementation& fu_;

    wxString name_;
    wxString widthFormula_;
    wxString description_;
    int direction_;

    wxCheckListBox* depList_;

    DECLARE_EVENT_TABLE()
};

#endif
