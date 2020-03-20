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
 * @file RFArchitectureDialog.hh
 *
 * Declaration of RFArchParamtersDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
