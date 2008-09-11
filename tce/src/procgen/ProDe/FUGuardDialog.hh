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
 * @file FUGuardDialog.hh
 *
 * Declaration of FUGuardDualog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_FU_GUARD_DIALOG_HH
#define TTA_FU_GUARD_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class Bus;
    class PortGuard;
    class FunctionUnit;
    class FUPort;
}

/**
 * Dialog for editing function unit port guard properties.
 */
class FUGuardDialog : public wxDialog {
public:
    FUGuardDialog(
        wxWindow* parent,
        TTAMachine::Bus* bus,
        TTAMachine::PortGuard* guard = NULL);
    virtual ~FUGuardDialog();
    virtual bool TransferDataToWindow();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onFUChoice(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    TTAMachine::FunctionUnit* selectedFU() const;
    TTAMachine::FUPort* selectedPort() const;
    void setTexts();

    /// Port guard to edit.
    TTAMachine::PortGuard* guard_;
    /// Choice widget for the function unit selection.
    wxChoice* nameChoice_;
    /// Choice widget for the port selection.
    wxChoice* portChoice_;
    /// Checkbox widget for the inverted flag.
    wxCheckBox* invertedBox_;

    /// Inverted flag for the guard.
    bool inverted_;
    /// Modified inverted flag.
    bool newInverted_;
    /// Port of the guard.
    TTAMachine::FUPort* port_;
    /// Parent bus of the guard.
    TTAMachine::Bus* bus_;
    /// True if a new guard is being added, false otherwise.
    bool adding_;

    // IDs for the dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_LABEL_PORT,
        ID_FU_NAME,
        ID_FU_PORT,
        ID_INVERTED,
        ID_HELP,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
