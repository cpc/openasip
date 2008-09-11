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
 * @file GenerateProcessorDialog.hh
 *
 * Declaration of GenerateProcessorDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GENERATE_PROCESSOR_DIALOG_HH
#define TTA_GENERATE_PROCESSOR_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

namespace ProGe {
    class ICDecoderGeneratorPlugin;
}

class BinaryEncoding;

/**
 * Dialog for generating processor implementation using ProGe.
 */
class GenerateProcessorDialog : public wxDialog {
public:
    GenerateProcessorDialog(
        wxWindow* parent, TTAMachine::Machine& machine,
        const IDF::MachineImplementation& impl);
    virtual ~GenerateProcessorDialog();

private:

    void onOK(wxCommandEvent& event);
    void onBrowseBEMLoad(wxCommandEvent& event);
    void onBrowseBEMSave(wxCommandEvent& event);
    void onBrowseTargetDir(wxCommandEvent& event);

    ProGe::ICDecoderGeneratorPlugin* loadICDecoderGeneratorPlugin(
        const std::string& pluginFile,
        const std::string& pluginName);

    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);

    /// Processor architecture.
    TTAMachine::Machine& machine_;
    /// Processor implementation definition.
    const IDF::MachineImplementation& impl_;
    /// Binary Encosing Map.
    BinaryEncoding* bem_;

    /// Widget IDs.
    enum {
        ID_GENERATE_BEM,
        ID_LOAD_BEM,
        ID_SAVE_BEM,
        ID_BROWSE_BEM_LOAD,
        ID_BEM_LOAD_PATH,
        ID_BROWSE_BEM_SAVE,
        ID_BEM_SAVE_PATH,
        ID_TARGET,
        ID_BROWSE_TARGET,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};

#endif
