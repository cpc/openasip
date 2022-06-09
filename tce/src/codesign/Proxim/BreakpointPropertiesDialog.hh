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
 * @file BreakpointPropertiesDialog.hh
 *
 * Declaration of BreakpointPropertiesDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BREAKPOINT_PROPERTIES_DIALOG_HH
#define TTA_BREAKPOINT_PROPERTIES_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

class StopPointManager;

/**
 * Dialog for editing breakpoint properties in Proxim.
 */
class BreakpointPropertiesDialog : public wxDialog {
public:
    BreakpointPropertiesDialog(
        wxWindow* parent, StopPointManager& manager, unsigned int handle);

    virtual ~BreakpointPropertiesDialog();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    /// Widget IDs.
    enum {
        ID_TEXT_BREAKPOINT = 10000,
        ID_TEXT_BP_ID,
        ID_LABEL_CONDITION,
        ID_CONDITION,
        ID_LABEL_IGNORE_COUNT,
        ID_IGNORE_CTRL,
        ID_LINE,
        ID_HELP,
        ID_CLOSE
    };
    /// Stoppoint manager containing the breakpoint,
    StopPointManager& manager_;
    /// Handle of the breakpoint to modify.
    unsigned int handle_;

    DECLARE_EVENT_TABLE()
};
#endif
