/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file MachineDialog.hh
 *
 * Declaration of MachineDialog class.
 *
 * Created on: 6.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MACHINEDIALOG_HH
#define MACHINEDIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>

namespace TTAMachine {
    class Machine;
}

/**
 * Dialog for editing processor wide properties.
 */
class MachineDialog : public wxDialog {
public:
    MachineDialog(wxWindow* parent, TTAMachine::Machine& machine);
    virtual ~MachineDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);

    /// The machine to modify
    TTAMachine::Machine& machine_;
    wxChoice* endianessChoise_;

    enum {
        ID_ENDIANESS_CHOICE = 10000
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};

#endif /* MACHINEDIALOG_HH */
