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
 * @file HelpBrowser.hh
 *
 * Declaration of the HelpBrowser class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_HELP_BROWSER
#define TTA_HELP_BROWSER

#include <wx/frame.h>

class wxHtmlWindow;

/**
 * Simple window for displaying html-pages.
 */
class HelpBrowser : public wxFrame {
public:
    HelpBrowser(wxString title, wxString file,
                wxPoint position, wxSize size);

    ~HelpBrowser();

private:
    void onClose(wxCloseEvent& event);
    void onMenuClose(wxCommandEvent& event);
    void onBack(wxCommandEvent& event);
    void onForward(wxCommandEvent& event);

    wxHtmlWindow* html_;

    /// IDs form menu items.
    enum {
        ID_BACK = 10000,
        ID_FORWARD
    };

    DECLARE_EVENT_TABLE()
};

#endif
