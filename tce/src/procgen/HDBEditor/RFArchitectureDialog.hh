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
 * @file RFArchitectureDialog.hh
 *
 * Declaration of RFArchParamtersDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_ARCHITECTURE_DIALOG_HH
#define TTA_RF_ARCHITECTURE_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class RFArchitecture;
}


/**
 * Dialog for defining RF architecture paramters to HDB.
 */
class RFArchitectureDialog : public wxDialog {
public:
    RFArchitectureDialog(
        wxWindow* parent, wxWindowID id, HDB::RFArchitecture& arch);
    virtual ~RFArchitectureDialog();

private:
    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void update();

    /// Widget IDs.
    enum {        
        ID_LABEL_SIZE = 10000,
        ID_SIZE,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_READ_PORTS,
        ID_READ_PORTS,
        ID_LABEL_WRITE_PORTS,
        ID_WRITE_PORTS,
        ID_LABEL_BIDIR_PORTS,
        ID_BIDIR_PORTS,
        ID_LABEL_MAX_READS,
        ID_MAX_READS,
        ID_LABEL_MAX_WRITES,
        ID_MAX_WRITES,
        ID_PARAM_WIDTH,
        ID_PARAM_SIZE,
        ID_GUARD_SUPPORT,
        ID_LINE
    };

    HDB::RFArchitecture& arch_;

    int size_;    
    int width_;
    int readPorts_;
    int writePorts_;
    int bidirPorts_;
    int maxReads_;
    int maxWrites_;
    int latency_;
    bool guardSupport_;
    bool paramWidth_;
    bool paramSize_;

    DECLARE_EVENT_TABLE()
};

#endif
