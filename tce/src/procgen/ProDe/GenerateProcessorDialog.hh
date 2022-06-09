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
 * @file GenerateProcessorDialog.hh
 *
 * Declaration of GenerateProcessorDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012  
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
        ID_LINE,
        ID_VHDL,
        ID_VERILOG
    };

    DECLARE_EVENT_TABLE()
};

#endif
