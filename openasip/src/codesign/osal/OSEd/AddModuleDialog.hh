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
 * @file AddModuleDialog.hh
 *
 * Declaration of AddModuleDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_MODULE_DIALOG_HH
#define TTA_ADD_MODULE_DIALOG_HH

#include <wx/wx.h>
#include <string>

/**
 * Dialog for adding new modules.
 */
class AddModuleDialog : public wxDialog {
public:
    AddModuleDialog(wxWindow* parent, std::string path);
    virtual ~AddModuleDialog();

    std::string name() const;

    AddModuleDialog(const AddModuleDialog&) = delete;
    AddModuleDialog& operator=(const AddModuleDialog&) = delete;

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onOk(wxCommandEvent& event);
    void setTexts();

    /**
     * Widget ids.
     */
    enum {
        ID_TEXT,
        ID_MODULE_NAME
    };

    /// Path in which module is added.
    std::string path_;
    /// Name of the module.
    wxString name_;

    DECLARE_EVENT_TABLE()
};

#endif
