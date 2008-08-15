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
 * @file RFGuardDialog.hh
 *
 * Declaration of RFGuardDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_RF_GUARD_DIALOG_HH
#define TTA_RF_GUARD_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class Bus;
    class RegisterGuard;
    class RegisterFile;
}

/**
 * Dialog for editing register file guard properties.
 */
class RFGuardDialog : public wxDialog {
public:
    RFGuardDialog(
        wxWindow* parent,
        TTAMachine::Bus* bus,
        TTAMachine::RegisterGuard* guard = NULL);
    virtual ~RFGuardDialog();
    virtual bool TransferDataToWindow();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onRFChoice(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    TTAMachine::RegisterFile* selectedRF() const;
    void setTexts();

    /// RegisterGuard to edit.
    TTAMachine::RegisterGuard* guard_;
    /// Choice widget for register file name selection.
    wxChoice* nameChoice_;
    /// Choice widget for register index selection.
    wxChoice* indexChoice_;
    /// Checkbox widget for the inverted flag.
    wxCheckBox* invertedBox_;

    /// Inverted flag of the guard.
    bool inverted_;
    /// Modified inverted flag of the guard.
    bool newInverted_;
    /// Register index of the guard.
    int index_;
    /// Modified register index of the guard.
    int newIndex_;
    /// Guard register.
    TTAMachine::RegisterFile* rf_;
    /// Parent bus of the register guard.
    TTAMachine::Bus* bus_;
    /// True if a new guard is being added, false otherwise.
    bool adding_;

    // IDs for dialog widgets
    enum {
        ID_LABEL_NAME = 10000,
        ID_RF_NAME,
        ID_LABEL_INDEX,
        ID_RF_INDEX,
        ID_INVERTED,
        ID_HELP,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
