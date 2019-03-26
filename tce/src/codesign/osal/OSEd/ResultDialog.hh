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
 * @file ResultDialog.hh
 *
 * Declaration of ResultDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESULT_DIALOG_HH
#define TTA_RESULT_DIALOG_HH

#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/textctrl.h>

/**
 * Dialog that shows operation build results.
 */
class ResultDialog : public wxDialog {
public:
    ResultDialog(
        wxWindow* parent, 
        std::vector<std::string> output,
        const std::string& title,
        const std::string& module = "");
    virtual ~ResultDialog();

private:
    /// Copying not allowed.
    ResultDialog(const ResultDialog&);
    /// Assignment not allowed.
    ResultDialog operator=(const ResultDialog&);
    
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();
    virtual bool TransferDataToWindow();
	void onOpen(wxCommandEvent& event);

    /**
     * Widget ids.
     */
    enum {
        ID_RESULT,
        ID_BUTTON_OPEN
    };

    /// Result window pointer.
    wxTextCtrl* result_;
    /// Output of compilation.
    std::vector<std::string> output_;
    /// Module which was compiled or empty string, if all modules were
    /// compiled.
    std::string module_;

    DECLARE_EVENT_TABLE()
};

#endif
