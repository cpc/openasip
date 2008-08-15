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
 * @file FocusTrackingTextCtrl.cc
 *
 * Implementation of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
