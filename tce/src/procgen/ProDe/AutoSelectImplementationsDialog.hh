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
 * @file AutoSelectImplementationsDialog.hh
 *
 * Declaration of AutoSelectImplementationsDialog class.
 *
 * @author Mikko Järvelä 2013 (jarvela7-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_AUTO_SELECT_IMPLEMENTATIONS_DIALOG_HH
#define TTA_AUTO_SELECT_IMPLEMENTATIONS_DIALOG_HH

#include <wx/wx.h>
#include "TCEString.hh"
#include "Machine.hh"
#include "HDBManager.hh"
#include "UnitImplementationLocation.hh"

namespace IDF {
    class MachineImplementation;
    class UnitImplementationLocation;
}

namespace HDB {
    class HDBManager;
}

namespace TTAMachine {
    class Machine;
}

/**
 * Dialog for searching and selecting implementations for empty RF/IU/FU
 * units automatically from user chosen HDB file.
 */
class AutoSelectImplementationsDialog : public wxDialog {
public:
    AutoSelectImplementationsDialog(
        wxWindow* parent, 
        TTAMachine::Machine& machine,
        IDF::MachineImplementation& impl);
    virtual ~AutoSelectImplementationsDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    
    void onBrowse(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    void onFind(wxCommandEvent& event);

    void findRFImplementations(HDB::HDBManager& hdb);
    void findIUImplementations(HDB::HDBManager& hdb);
    void findFUImplementations(HDB::HDBManager& hdb);

    /// Machine containing navigators fo RFs, IUs and FUs.
    TTAMachine::Machine& machine_;
    // Contains machine's implementation.
    IDF::MachineImplementation& impl_;

    /// Choice selection for list of HDB files.
    wxChoice* hdbChoice_;

    /// Checkbox widget for register file flag.
    wxCheckBox* cboxRF_;
    /// Checkbox widget for immediate unit flag.
    wxCheckBox* cboxIU_;
    /// Checkbox widget for function unit flag.
    wxCheckBox* cboxFU_;
 
    /// Set for hdb paths.
    std::set<TCEString> hdbs_;
    
    // HDB & ID pair for units without implementation
    struct HdbIdPair {
        TCEString hdbFile;
        int id;
    };

    // units that are without implementation, but for which an 
    // implementation file was found (HDB & ID pair)
    std::map<const IDF::UnitImplementationLocation*, HdbIdPair> foundRF_;
    std::map<const IDF::UnitImplementationLocation*, HdbIdPair> foundIU_;
    std::map<const IDF::UnitImplementationLocation*, HdbIdPair> foundFU_;
    
    enum {
        ID_HDB_CHOICE,
        ID_BROWSE,
        ID_LINE,
        ID_TEXT,
        ID_FIND,
        ID_RF,
        ID_IU,
        ID_FU,
        ID_CLOSE
    };

    DECLARE_EVENT_TABLE()
};
#endif
