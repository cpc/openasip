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
 * @file EditParameterDialog.hh
 *
 * Declaration of EditParameterDialog class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-.tut.fi)
 * @note rating: red
 */

#ifndef EDIT_PARAMETER_DIALOG_HH
#define EDIT_PARAMETER_DIALOG_HH

#include <wx/wx.h>
#include "PluginTools.hh"
#include "DesignSpaceExplorer.hh"
#include "ExplorerPluginParameter.hh"

namespace TTAMachine {
    class Machine;
}

class DesignSpaceExplorerPlugin;

/**
 * Explorer plugin call window.
 */
class EditParameterDialog : public wxDialog {
public:
    EditParameterDialog(
        wxWindow* parent, ExplorerPluginParameter* parameter);

    virtual ~EditParameterDialog();

private:

    virtual bool TransferDataToWindow();
    void onClose(wxCommandEvent& event);
    void onOk(wxCommandEvent& event);
    void setTexts();  

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// name
    wxStaticText* paramName_;
    /// name
    wxStaticText* paramType_;
    /// name
    wxTextCtrl* paramValue_;
    
    ExplorerPluginParameter* parameter_;
    
    enum {
        ID_TEXT,
        ID_NAME,
        ID_TYPE,
        ID_VALUE,
        ID_OK,
        ID_CANCEL
    };

    DECLARE_EVENT_TABLE()
};

#endif
