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
 * @file IUDialog.hh
 *
 * Declaration of IUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_IU_DIALOG_HH
#define TTA_IU_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

namespace TTAMachine {
    class ImmediateUnit;
    class Port;
}

/**
 * Dialog for editing immediate unit parameters.
 */
class IUDialog : public wxDialog {
public:
    IUDialog(wxWindow* parent, TTAMachine::ImmediateUnit* immediateUnit);
    virtual ~IUDialog();

private:
    wxSizer* createContents(
        wxWindow* parent,
        bool call_fit,
        bool set_sizer);

    virtual bool TransferDataToWindow();
    TTAMachine::Port* selectedPort() const;
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);
    void onAddPort(wxCommandEvent& event);
    void onActivatePort(wxListEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void updatePortList();
    void updateTemplateList();
    void setTexts();

    /// Sizer containing the port list and associated buttons.
    wxStaticBoxSizer* portListSizer_;
    /// Sizer containing the template list.
    wxStaticBoxSizer* templateListSizer_;

    /// Immediate unit to modify.
    TTAMachine::ImmediateUnit* immediateUnit_;
    /// Name of the immediate unit.
    wxString name_;
    /// Number of registers.
    int size_;
    /// Width.
    int width_;
    /// Cycles.
    int cycles_;

    /// Port list control.
    wxListCtrl* portList_;
    /// Template list control.
    wxListCtrl* templateList_;
    /// Radio box for extension.
    wxRadioBox* extensionBox_;

    /// enumerated IDs for the dialog controls
    enum {
        ID_NAME = 10000,
        ID_WIDTH,
        ID_SIZE,
        ID_EXTENSION,
        ID_PORT_LIST,
        ID_ADD_PORT,
        ID_EDIT_PORT,
        ID_DELETE_PORT,
        ID_TEMPLATE_LIST,
        ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_WIDTH,
        ID_LABEL_SIZE,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
