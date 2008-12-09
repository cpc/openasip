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
 * @file FUPortDialog.hh
 *
 * Declaration of FUPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_DIALOG_HH
#define TTA_FU_PORT_DIALOG_HH

namespace TTAMachine {
    class FUPort;
    class Socket;
}

/**
 * Dialog for querying port parameters from the user.
 */
class FUPortDialog : public wxDialog {
public:
    FUPortDialog(wxWindow* parent, TTAMachine::FUPort* port);
    ~FUPortDialog();

protected:
    virtual bool TransferDataToWindow();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    void updateSockets();
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    void onSocketChoice(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void setTexts();

    /// Function unit port to modify.
    TTAMachine::FUPort* port_;
    /// Name of the port.
    wxString name_;
    /// Width of the port.
    int width_;
    /// Trigger attribute of the port.
    bool triggers_;
    /// Input socket choice control.
    wxChoice* inputSocketChoice_;
    /// Output socket choice control.
    wxChoice* outputSocketChoice_;

    /// Original input socket of the port.
    TTAMachine::Socket* oldInput_;
    /// Original output socket of the port.
    TTAMachine::Socket* oldOutput_;

    enum {
        ID_NAME=10000,
        ID_WIDTH,
        ID_INPUT_SOCKET,
        ID_OUTPUT_SOCKET,
        ID_TRIGGERS,
        ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_WIDTH,
        ID_LABEL_INPUT_SOCKET,
        ID_LABEL_OUTPUT_SOCKET,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
