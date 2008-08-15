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
 * @file RFPortImplementationDialog.hh
 *
 * Declaration of RFPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH
#define TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class RFPortImplementation;
}

/**
 * Dialog for editing register file port implementations.
 */
class RFPortImplementationDialog : public wxDialog {
public:
    RFPortImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::RFPortImplementation& implementation);

    virtual ~RFPortImplementationDialog();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_LOAD_PORT,
        ID_LOAD_PORT,
        ID_LABEL_OPCODE_PORT,
        ID_OPCODE_PORT,
        ID_LABEL_OPCODE_PORT_WIDTH,
        ID_OPCODE_PORT_WIDTH,
        ID_DIRECTION,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::RFPortImplementation& port_;

    wxString name_;
    wxString loadPort_;
    wxString opcodePort_;
    wxString opcodePortWidth_;
    int direction_;

    DECLARE_EVENT_TABLE()
};

#endif
