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
 * @file SRPortDialog.hh
 *
 * Declaration of SRPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SR_PORT_DIALOG_HH
#define TTA_SR_PORT_DIALOG_HH

namespace TTAMachine {
    class SpecialRegisterPort;
    class Socket;
}

/**
 * Dialog for creating and editing special register ports.
 */
class SRPortDialog : public wxDialog {
public:
    SRPortDialog(wxWindow* parent, TTAMachine::SpecialRegisterPort* port);
    ~SRPortDialog();

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
    TTAMachine::SpecialRegisterPort* port_;
    /// Name of the port.
    wxString name_;
    /// Width of the port.
    int width_;
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
