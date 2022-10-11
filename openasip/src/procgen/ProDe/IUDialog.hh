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
 * @file IUDialog.hh
 *
 * Declaration of IUDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
