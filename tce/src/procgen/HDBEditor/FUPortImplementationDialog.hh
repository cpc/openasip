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
 * @file FUPortImplementationDialog.hh
 *
 * Declaration of FUPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_IMPLEMENTATION_DIALOG_HH
#define TTA_FU_PORT_IMPLEMENTATION_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class FUPortImplementation;
}

namespace TTAMachine {
    class BaseFUPort;
}

/**
 * Dialog for editing FU port implementations.
 */
class FUPortImplementationDialog : public wxDialog {
public:
    FUPortImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUPortImplementation& implementation,
        const TTAMachine::BaseFUPort& architecture);

    virtual ~FUPortImplementationDialog();
    virtual bool TransferDataToWindow();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_ARCHITECTURE_PORT,
        ID_ARCHITECTURE_PORT_NAME,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LOAD_PORT_LABEL,
        ID_LOAD_PORT,
        ID_GUARD_PORT_LABEL,
        ID_GUARD_PORT,
        ID_OPERAND_LIST,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUPortImplementation& implementation_;
    /// Architecture of the modified port.
    const TTAMachine::BaseFUPort& architecture_;

    /// Name of the port.
    wxString name_;
    /// Name of the load port.
    wxString loadPortName_;
    /// Name of the guard port.
    wxString guardPortName_;
    /// Width formula string.
    wxString widthFormula_;

    /// List widget for the operand bindings.
    wxListCtrl* operandList_;

    DECLARE_EVENT_TABLE()
};

#endif
