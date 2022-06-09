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
 * @file FocusTrackingTextCtrl.hh
 *
 * Declaration of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_FOCUS_TRACKING_TEXT_CTRL_HH
#define TTA_FOCUS_TRACKING_TEXT_CTRL_HH

#include <wx/wx.h>

/**
 * Textfield widget which emits a wxEVT_COMMAND_TEXT_ENTER when losing
 * keyboard focus.
 *
 * This widget behaves exactly like wxTextCtrl except for the event emited
 * when focus is lost. This widget is useful for textfields which are
 * validated when the user presses enter on the widget or changes the
 * keyboard focus to another window. The created text widget has always
 * the wxTE_PROCESS_ENTER style set.
 */
class FocusTrackingTextCtrl : public wxTextCtrl {
public:
    FocusTrackingTextCtrl(
        wxWindow* parent,
        wxWindowID id,
        const wxString& value = _T(""),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = _T("FocusTrackingTextCtrl"));
    ~FocusTrackingTextCtrl();
private:
    void onKillFocus(wxFocusEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
