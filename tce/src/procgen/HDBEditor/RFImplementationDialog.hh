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
