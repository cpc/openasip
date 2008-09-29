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
 * @file RFImplementationDialog.hh
 *
 * Declaration of RFImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_IMPLEMENTATION_DIALOG_HH
#define TTA_RF_IMPLEMENTATION_DIALOG_HH

#include "RFImplementation.hh"

class wxListCtrl;

/**
 * Dialog for editing RF implementations.
 */
class RFImplementationDialog : public wxDialog {
public:
    RFImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::RFImplementation& implementation);

    virtual ~RFImplementationDialog();
private:

    void onOK(wxCommandEvent& event);
    void update();

    HDB::RFPortImplementation* selectedPort();

    void onPortSelection(wxListEvent& event); 
    void onAddPort(wxCommandEvent& event); 
    void onModifyPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);

    void onSourceFileSelection(wxListEvent& event);
    void onAddSourceFile(wxCommandEvent& event);
    void onDeleteSourceFile(wxCommandEvent& event);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Window indentifiers for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_CLK_PORT,
        ID_CLK_PORT,
        ID_LABEL_RESET_PORT,
        ID_RESET_PORT,
        ID_LABEL_GLOCK_PORT,
        ID_GLOCK_PORT,
        ID_LABEL_GUARD_PORT,
        ID_GUARD_PORT,
        ID_LABEL_SIZE_PARAMETER,
        ID_SIZE_PARAMETER,
        ID_LABEL_WIDTH_PARAMETER,
        ID_WIDTH_PARAMETER,
        ID_PORT_LIST,
        ID_ADD_PORT,
        ID_MODIFY_PORT,
        ID_DELETE_PORT,
        ID_SOURCE_LIST,
        ID_ADD_SOURCE,
        ID_DELETE_SOURCE,
        ID_LINE
    };

    /// RF Implementation to modify.
    HDB::RFImplementation& implementation_;

    /// Pointer to the port list widget.
    wxListCtrl* portList_;
    /// Pointer to the source file list widget.
    wxListCtrl* sourceList_;

    wxString name_;
    wxString clkPort_;
    wxString rstPort_;
    wxString gLockPort_;
    wxString guardPort_;
    wxString sizeParam_;
    wxString widthParam_;

    DECLARE_EVENT_TABLE()
};

#endif
