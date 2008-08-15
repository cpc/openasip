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
 * @file FUImplementationParameterDialog.hh
 *
 * Declaration of FUImplementationParameterDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_IMPLEMENTATION_PARAMETER_DIALOG_HH
#define TTA_FU_IMPLEMENTATION_PARAMETER_DIALOG_HH

#include <wx/wx.h>

#include "FUImplementation.hh"


/**
 * Dialog for editing FU port implementations.
 */
class FUImplementationParameterDialog : public wxDialog {
public:
    FUImplementationParameterDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUImplementation::Parameter& implementation);

    virtual ~FUImplementationParameterDialog();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_TYPE,
        ID_TYPE,
        ID_LABEL_VALUE,
        ID_VALUE,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUImplementation::Parameter& parameter_;
 
    wxString name_;
    wxString type_;
    wxString value_;

    DECLARE_EVENT_TABLE()
};

#endif
