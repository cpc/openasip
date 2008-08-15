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
 * @file FocusTrackingTextCtrl.hh
 *
 * Declaration of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
