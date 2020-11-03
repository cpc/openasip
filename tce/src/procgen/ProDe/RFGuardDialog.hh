/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file RFGuardDialog.hh
 *
 * Declaration of RFGuardDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
    const TTAMachine::RegisterFile* selectedRF() const;
    void setTexts();

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
    const TTAMachine::RegisterFile* rf_;
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
