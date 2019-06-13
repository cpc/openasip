/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file RFExternalPortDialog.hh
 *
 * Declaration of RFExternalPortDialog class.
 *
 * @author Henry Linjamäki (henry-linjamaki-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RFEXTERNAL_PORT_DIALOG_HH_
#define TTA_RFEXTERNAL_PORT_DIALOG_HH_

#include <wx/wx.h>

class wxCheckListBox;

namespace HDB {
    class RFExternalPort;
    class RFImplementation;
}

/**
 * Dialog for editing RF external ports.
 */
class RFExternalPortDialog : public wxDialog {
public:
    RFExternalPortDialog(
        wxWindow* parent, wxWindowID id,
        HDB::RFExternalPort& implementation,
        const HDB::RFImplementation& rf);

    virtual ~RFExternalPortDialog();

private:
    void initialize();
    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESCRIPTION,
        ID_DESCRIPTION,
        ID_DIRECTION,
        ID_PARAMETER_DEPS,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// RF port implementation to modify.
    HDB::RFExternalPort& port_;
    /// Parent RF implementation of the port.
    const HDB::RFImplementation& rf_;

    wxString name_;
    wxString widthFormula_;
    wxString description_;
    int direction_;

    wxCheckListBox* depList_;

    DECLARE_EVENT_TABLE()
};


#endif /* TTA_RFEXTERNAL_PORT_DIALOG_HH_ */
