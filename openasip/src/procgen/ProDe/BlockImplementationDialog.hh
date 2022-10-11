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
 * @file BlockImplementationDialog.hh
 *
 * Declaration of BlockImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_DIALOG_HH
#define TTA_BLOCK_IMPLEMENTATION_DIALOG_HH

#include <set>
#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Component;
}

namespace IDF {
    class UnitImplementationLocation;
}

class BlockImplementationDialog : public wxDialog {
public:
    BlockImplementationDialog(
        wxWindow* parent, const TTAMachine::Component& block,
        IDF::UnitImplementationLocation& impl);

    virtual ~BlockImplementationDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onBrowse(wxCommandEvent& event);
    void onImplSelection(wxListEvent& event);
    void onImplActivation(wxListEvent& e);

    void doOK();
    void onOK(wxCommandEvent& event);
    void onHDBSelection(wxCommandEvent& event);

    const TTAMachine::Component& block_;
    IDF::UnitImplementationLocation& impl_;
    wxListCtrl* list_;
    wxChoice* hdbChoice_;

    /// Static set for hdb paths.
    static std::set<std::string> hdbs_;
    /// Static variable for the hdb choicer selection.
    static int selection_;

    // Default HDB file to set in the beginning
    static const std::string defaultHDB_;

    enum {
        ID_HDB_CHOICE,
        ID_BROWSE,
        ID_LIST,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};

#endif
