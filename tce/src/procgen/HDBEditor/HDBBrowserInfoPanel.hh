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
 * @file HDBBrowserInfoPanel.hh
 *
 * Declaration of HDBBrowserInfoPanel class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_BROWSER_INFO_PANEL_HH
#define TTA_HDB_BROWSER_INFO_PANEL_HH

#include <wx/wx.h>
#include <wx/html/htmlwin.h>

#include "DBTypes.hh"

class HDBToHtml;
namespace HDB {
    class HDBManager;
}

/**
 * Window for displaying HDB element details.
 */
class HDBBrowserInfoPanel : public wxHtmlWindow {
public:
    HDBBrowserInfoPanel(wxWindow* parent, wxWindowID id);
    virtual ~HDBBrowserInfoPanel();

    void setHDB(const HDB::HDBManager& hdb);

    void clear();
    void displayFUEntry(RowID id);
    void displayRFEntry(RowID id);
    void displayBusEntry(RowID id);
    void displaySocketEntry(RowID id);
    void displayFUArchitecture(RowID id);
    void displayRFArchitecture(RowID id);
    void displayFUImplementation(RowID id);
    void displayRFImplementation(RowID id);
    void displayCostFunctionPlugin(RowID id);

private:
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link);

    /// HDB to HTML generator.
    HDBToHtml* htmlGen_;
};

#endif
