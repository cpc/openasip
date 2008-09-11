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
 * @file RFPortDialog.hh
 *
 * Declaration of RFPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_DIALOG_HH
#define TTA_RF_PORT_DIALOG_HH

namespace TTAMachine {
    class Port;
    class Socket;
}

/**
 * Dialog for querying register file port parameters from the user.
 */
class RFPortDialog : public wxDialog {
public:
    RFPortDialog(wxWindow* parent, TTAMachine::Port* port);
    virtual ~RFPortDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onSocketChoice(wxCommandEvent& event);
    void updateSockets();
    void setTexts();

    /// Port to modify.
    TTAMachine::Port* port_;
    /// Name of the port.
    wxString name_;
    /// Input socket choice control.
    wxChoice* inputSocketChoice_;
    /// Output socket choice control.
    wxChoice* outputSocketChoice_;
    /// Original input socket of the port.
    TTAMachine::Socket* oldInput_;
    /// Original output socket of the port.
    TTAMachine::Socket* oldOutput_;

    /// enumerated IDs for the port controls
    enum {
	ID_NAME = 10000,
	ID_INPUT_SOCKET,
	ID_OUTPUT_SOCKET,
	ID_HELP
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
