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
 * @file MemoryValueDialog.hh
 *
 * Declaration of MemoryValueDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_VALUE_DIALOG_HH
#define TTA_MEMORY_VALUE_DIALOG_HH

#include <wx/wx.h>
#include <string>

class NumberControl;

/**
 * Class for giving new values to memory addresses.
 */
class MemoryValueDialog : public wxDialog {
public:
    MemoryValueDialog(wxWindow* parent, unsigned maxBits = 64);
    virtual ~MemoryValueDialog();

    void setValue(int value);
    long mode();
    int intValue();
    double doubleValue();

private:

    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /**
     * Widget ids.
     */
    enum {
        ID_VALUE = 100000
    };

    /// NumberControl for giving new values to memory.
    NumberControl* value_;
    /// Maximum number of bits used by the returned value.
    unsigned maxBits_;

    DECLARE_EVENT_TABLE()
};

#endif
