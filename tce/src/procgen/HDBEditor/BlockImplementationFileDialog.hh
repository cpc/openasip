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
 * @file BlockImplementationFileDialog.hh
 *
 * Declaration of BlockImplementationFileDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_FILE_DIALOG_HH
#define TTA_BLOCK_IMPLEMENTATION_FILE_DIALOG_HH

#include <wx/wx.h>

#include "FUImplementation.hh"


/**
 * Dialog for editing BlockImplementationFile objects.
 */
class BlockImplementationFileDialog : public wxDialog {
public:
    BlockImplementationFileDialog(
        wxWindow* parent, wxWindowID id,
        HDB::BlockImplementationFile& file);

    virtual ~BlockImplementationFileDialog();
private:
    void onBrowse(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_PATH = 10000,
        ID_PATH,
        ID_LABEL_FORMAT,
        ID_FORMAT,
        ID_BROWSE,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// BlockImplementationFile object to modify.
    HDB::BlockImplementationFile& file_;

    /// Block implementation file path.
    wxString path_;
    /// Selected implementation file type.
    int format_;

    DECLARE_EVENT_TABLE()
};

#endif
