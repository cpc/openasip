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
 * @file SocketDialog.hh
 *
 * Declaration of SocketDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_SOCKET_DIALOG_HH
#define TTA_SOCKET_DIALOG_HH

#include <wx/wx.h>
#include <wx/valgen.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

class MachineTester;

namespace TTAMachine {
    class Socket;
    class Segment;
}

/**
 * Dialog for modifying socket attributes.
 */
class SocketDialog : public wxDialog {
public:
    SocketDialog(wxWindow* parent, TTAMachine::Socket* socket);
    virtual ~SocketDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    TTAMachine::Segment* segment(wxListCtrl* listCtrl, int index);
    void onOK(wxCommandEvent&);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent&);
    void onAttach(wxCommandEvent&);
    void onDetach(wxCommandEvent&);
    void onAttachedSelection(wxListEvent&);
    void onDetachedSelection(wxListEvent&);
    void onDirection(wxCommandEvent&);
    void updateConnected();
    void updateDirection();
    virtual bool TransferDataToWindow();
    void setTexts();

    /// Socket to modify.
    TTAMachine::Socket* socket_;
    /// Name of the socket.
    wxString name_;
    /// MachineTester which does sanity checks for the target machine.
    MachineTester* tester_;
    /// Radio box for direction.
    wxRadioBox* directionBox_;
    /// List control for connected (bus, segment) pairs.
    wxListCtrl* connectedListCtrl_;
    /// List control for all (bus, segment) pairs.
    wxListCtrl* segmentListCtrl_;
    /// Static boxsizer containing the list of attached buses.
    wxStaticBoxSizer* attachedSizer_;
    /// Static boxsizer containing the list of detached buses.
    wxStaticBoxSizer* detachedSizer_;

    enum {
        ID_NAME = 10000,
        ID_ATTACHED_LIST,
        ID_DETACHED_LIST,
        ID_ATTACH,
        ID_DETACH,
        ID_HELP,
        ID_DIRECTION,
        ID_LABEL_NAME,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
