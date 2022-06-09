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
 * @file RFImplementationDialog.hh
 *
 * Declaration of RFImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_IMPLEMENTATION_DIALOG_HH
#define TTA_RF_IMPLEMENTATION_DIALOG_HH

#include <wx/listctrl.h>
#include "RFImplementation.hh"

class wxListCtrl;
class wxChoice;

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
    HDB::RFExternalPort* selectedExternalPort();
    HDB::RFImplementation::Parameter selectedParameter();

    void onSACchoise(wxCommandEvent& event);

    void onPortSelection(wxListEvent& event); 
    void onAddPort(wxCommandEvent& event); 
    void onModifyPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);

    void onExternalPortActivation(wxListEvent& event);
    void onExternalPortSelection(wxListEvent& event);
    void onAddExternalPort(wxCommandEvent& event);
    void onEditExternalPort(wxCommandEvent& event);
    void onDeleteExternalPort(wxCommandEvent& event);

    void onParameterActivation(wxListEvent& event);
    void onParameterSelection(wxListEvent& event);
    void onAddParameter(wxCommandEvent& event);
    void onEditParameter(wxCommandEvent& event);
    void onDeleteParameter(wxCommandEvent& event);

    void onSourceFileSelection(wxListEvent& event);
    void onAddSourceFile(wxCommandEvent& event);
    void onDeleteSourceFile(wxCommandEvent& event);
    void onMoveSourceFileUp(wxCommandEvent&);
    void onMoveSourceFileDown(wxCommandEvent&);

    void onSizeChoice(wxCommandEvent& event);
    void onWidthChoice(wxCommandEvent& event);

    wxString getWidthParameter();
    wxString getSizeParameter();

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
        ID_LABEL_SAC,
        ID_SAC,
        ID_PORT_LIST,
        ID_ADD_PORT,
        ID_MODIFY_PORT,
        ID_DELETE_PORT,
        ID_SOURCE_LIST,
        ID_ADD_SOURCE,
        ID_DELETE_SOURCE,
        ID_MOVE_SOURCE_UP,
        ID_MOVE_SOURCE_DOWN,
        ID_LINE,

        ID_EXTERNAL_PORT_LIST,
        ID_ADD_EXTERNAL_PORT,
        ID_EDIT_EXTERNAL_PORT,
        ID_DELETE_EXTERNAL_PORT,
        ID_PARAMETER_LIST,
        ID_EDIT_PARAMETER,
        ID_ADD_PARAMETER,
        ID_DELETE_PARAMETER,
        ID_SIZE_CHOICE,
        ID_WIDTH_CHOICE
    };

    enum {
        RBOX_FALSE = 0,
        RBOX_TRUE
    };

    /// RF Implementation to modify.
    HDB::RFImplementation& implementation_;

    /// Pointer to the port list widget.
    wxListCtrl* portList_;
    /// Pointer to the source file list widget.
    wxListCtrl* sourceList_;
    /// Pointer to the external port list widget.
    wxListCtrl* externalPortList_;
    /// Pointer to the parameter list widget.
    wxListCtrl* parameterList_;
    /// Pointer to size parameter choice widget
    wxChoice* sizeChoice_;
    /// Pointer to width parameter choice widget
    wxChoice* widthChoice_;

    wxString name_;
    wxString clkPort_;
    wxString rstPort_;
    wxString gLockPort_;
    wxString guardPort_;
    bool sacParam_;

    DECLARE_EVENT_TABLE()
};

#endif
