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
 * @file AddWatchDialog.hh
 *
 * Declaration of AddWatchDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_DIALOG_HH
#define TTA_WATCH_DIALOG_HH

#include <wx/wx.h>

class Watch;

/**
 * Dialog for creating watch points.
 */
class AddWatchDialog : public wxDialog {
public:
    AddWatchDialog(wxWindow* parent, wxWindowID id, Watch* watch = NULL);
    virtual ~AddWatchDialog();
private:
    void onWatchTypeChange(wxCommandEvent& event);
    void onFUChoice(wxCommandEvent& event);
    void onRFChoice(wxCommandEvent& event);
    void onASChoice(wxCommandEvent& event);
    void initialize();
    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Widget ids.
    enum {
        ID_RB_MEMORY,
        ID_RB_PORT,
        ID_RB_REGISTER,
        ID_RB_EXPRESSION,
        ID_RB_BUS,

        ID_LABEL_AS,
        ID_LABEL_UNIT,
        ID_LABEL_PORT,
        ID_LABEL_RF,
        ID_LABEL_BUS,
        ID_LABEL_EXPRESSION,
        ID_LABEL_CONDITION,
        ID_LABEL_START_ADDRESS,
        ID_LABEL_END_ADDRESS,
        ID_LABEL_INDEX,

        ID_START_ADDRESS,
        ID_END_ADDRESS,
        ID_BUS_CHOICE,
        ID_RF_CHOICE,
        ID_FU_CHOICE,
        ID_PORT_CHOICE,
        ID_INDEX_CHOICE,
        ID_EXPRESSION,
        ID_ADDRESS_SPACE,
        ID_LINE
    };

    /// Address space choicer for memory watch.
    wxChoice* asChoice_;
    /// Function unit choicer for port watch.
    wxChoice* fuChoice_;
    /// Port choicer for port watch.
    wxChoice* portChoice_;
    /// Register file choicer for register watch.
    wxChoice* rfChoice_;
    /// Register index choicer for register watch.
    wxChoice* indexChoice_;
    /// Bus choicer for bus watch.
    wxChoice* busChoice_;

    /// End adderss widget for memory watch.
    wxSpinCtrl* endAddressCtrl_;
    /// Start address widget for memory watch.
    wxSpinCtrl* startAddressCtrl_;

    DECLARE_EVENT_TABLE()
};
#endif
