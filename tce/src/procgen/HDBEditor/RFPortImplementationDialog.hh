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
 * @file RFPortImplementationDialog.hh
 *
 * Declaration of RFPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH
#define TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class RFPortImplementation;
}

/**
 * Dialog for editing register file port implementations.
 */
class RFPortImplementationDialog : public wxDialog {
public:
    RFPortImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::RFPortImplementation& implementation);

    virtual ~RFPortImplementationDialog();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_LOAD_PORT,
        ID_LOAD_PORT,
        ID_LABEL_OPCODE_PORT,
        ID_OPCODE_PORT,
        ID_LABEL_OPCODE_PORT_WIDTH,
        ID_OPCODE_PORT_WIDTH,
        ID_DIRECTION,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::RFPortImplementation& port_;

    wxString name_;
    wxString loadPort_;
    wxString opcodePort_;
    wxString opcodePortWidth_;
    int direction_;

    DECLARE_EVENT_TABLE()
};

#endif
