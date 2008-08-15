/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file AddWatchDialog.hh
 *
 * Declaration of AddWatchDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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

    /// Watch to modify. Null if a new watch is being created.
    Watch* watch_;

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
