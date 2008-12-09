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
 * @file FocusTrackingTextCtrl.cc
 *
 * Implementation of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include "FocusTrackingTextCtrl.hh"

BEGIN_EVENT_TABLE(FocusTrackingTextCtrl, wxTextCtrl)
    EVT_KILL_FOCUS(FocusTrackingTextCtrl::onKillFocus)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the control.
 * @param id Numeric id for the control.
 * @param value Initial value of the text field.
 * @param pos Position of the control.
 * @param size Size of the control.
 * @param style Style flags for the text control.
 * @param validator Validator for the control.
 * @param name Name of the control.
 */
FocusTrackingTextCtrl::FocusTrackingTextCtrl(
    wxWindow* parent,
    wxWindowID id,
    const wxString& value,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) :
    wxTextCtrl(parent, id, value, pos, size,
               (style | wxTE_PROCESS_ENTER) , validator, name) {

}

/**
 * The Destructor.
 */
FocusTrackingTextCtrl::~FocusTrackingTextCtrl() {
}

/**
 * Emits a wxEVT_COMMAND_TEXT_ENTER event when the control loses focus.
 *
 * @param event Focus event of the control losing focus.
 */
void
FocusTrackingTextCtrl::onKillFocus(wxFocusEvent& event) {
    wxCommandEvent textEntered =
        wxCommandEvent(wxEVT_COMMAND_TEXT_ENTER, GetId());
    AddPendingEvent(textEntered);
    event.Skip();
}
