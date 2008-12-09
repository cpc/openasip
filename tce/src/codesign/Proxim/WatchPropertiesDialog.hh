/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file WatchPropertiesDialog.hh
 *
 * Declaration of WatchPropertiesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_PROPERTIES_DIALOG_HH
#define TTA_WATCH_PROPERTIES_DIALOG_HH

#include <wx/wx.h>

class StopPointManager;

/**
 * Dialog for modifying watchpoint properties.
 */
class WatchPropertiesDialog : public wxDialog {
public:
    WatchPropertiesDialog(
        wxWindow* parent, wxWindowID id,
        StopPointManager& manager, int handle);
    ~WatchPropertiesDialog();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool call_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);

    /// Widget IDs.
    enum {
        ID_LABEL_WATCH = 10000,
        ID_LABEL_EXPRESSION,
        ID_LABEL_CONDITION,
        ID_LABEL_IGNORE_COUNT,

        ID_WATCH_HANDLE,
        ID_EXPRESSION,
        ID_CONDITION,
        ID_IGNORE_COUNT,
        ID_LINE
    };

    /// Stop point manager of the simulator.
    StopPointManager& manager_;
    /// Handle of the watch to modify.
    int handle_;

    /// Text widget for the watch expression script.
    wxTextCtrl* expressionCtrl_;
    /// Text widget fot the watch condition script.
    wxTextCtrl* conditionCtrl_;
    /// Spin button widget for the watch ignore count.
    wxSpinCtrl* ignoreCtrl_;

    DECLARE_EVENT_TABLE()
};
#endif
