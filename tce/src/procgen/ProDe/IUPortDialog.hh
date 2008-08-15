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
 * @file IUPortDialog.hh
 *
 * Declaration of IUPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_IU_PORT_DIALOG_HH
#define TTA_IU_PORT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

namespace TTAMachine {
    class Port;
}

/**
 * Dialog for editing immediate unit ports.
 */
class IUPortDialog : public wxDialog {
public:
    IUPortDialog(wxWindow* parent, TTAMachine::Port* port);
    virtual ~IUPortDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void updateSocket();
    void setTexts();

    /// Immediate unit port to edit with the dialog.
    TTAMachine::Port* port_;
    /// Name of the port.
    wxString name_;
    /// Output socket choice control.
    wxChoice* outputSocketChoice_;

    /// Enumerated IDs for dialog controls.
    enum {
	ID_NAME= 10000,
	ID_OUTPUT_SOCKET,
	ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_OUTPUT_SOCKET,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
   DECLARE_EVENT_TABLE()
};
#endif
