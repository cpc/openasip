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
 * @file SocketDialog.hh
 *
 * Declaration of SocketDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
