/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ValidateMachineDialog.hh
 *
 * Declaration of ValidateMachineDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_VALIDATE_MACHINE_DIALOG_HH
#define TTA_VALIDATE_MACHINE_DIALOG_HH

#include <wx/wx.h>

class wxHtmlWindow;

namespace TTAMachine {
    class Machine;
}

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class ValidateMachineDialog : public wxDialog {
public:
    ValidateMachineDialog(
        wxWindow* parent, const TTAMachine::Machine& machine);
    ~ValidateMachineDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onValidate(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    void onCheck(wxCommandEvent& event);

    /// Machine to be validated.
    const TTAMachine::Machine& machine_;
    /// HTML-widget for validation result texts.
    wxHtmlWindow* resultsWindow_;

    wxCheckBox* checkAnsiC_;
    wxCheckBox* checkGlobalConnReg_;

    // enumerated ids for dialog controls
    enum {
        ID_VALIDATE = 10000,
        ID_CLOSE,
        ID_RESULTS,
        ID_LINE,
        ID_CHECK_ANSI_C,
        ID_CHECK_GLOBAL_CONN_REGISTER
    };

    DECLARE_EVENT_TABLE()
};
#endif
