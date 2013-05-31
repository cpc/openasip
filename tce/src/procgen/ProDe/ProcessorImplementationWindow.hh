/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProcessorImplementationWindow.hh
 *
 * Declaration of ProcessorImplementationWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCESSOR_IMPLEMENTATION_WINDOW_HH
#define TTA_PROCESSOR_IMPLEMENTATION_WINDOW_HH

#include <wx/wx.h>
#include "PluginTools.hh"

namespace IDF {
    class MachineImplementation;
}

namespace TTAMachine {
    class Machine;
}

namespace ProGe {
    class ICDecoderGeneratorPlugin;
}

/**
 * Processor implementation window.
 */
class ProcessorImplementationWindow : public wxDialog {
public:
    ProcessorImplementationWindow(
        wxWindow* parent, TTAMachine::Machine& machine,
        IDF::MachineImplementation& impl);

    virtual ~ProcessorImplementationWindow();

    void setDirty(bool dirty = true) { dirtyData_ = dirty; }
private:

    void handleSelectRFImplementation(long item);
    void onSelectRFImplementation(wxCommandEvent& event);
    void onRFSelection(wxListEvent& event);
    void onRFActivation(wxListEvent& event);
    void updateRFList(const std::string& rfName, int index);

    void handleSelectIUImplementation(long item);
    void onSelectIUImplementation(wxCommandEvent& event);
    void onIUSelection(wxListEvent& event);
    void onIUActivation(wxListEvent& event);

    void handleSelectFUImplementation(long item);
    void onSelectFUImplementation(wxCommandEvent& event);
    void onFUSelection(wxListEvent& event);
    void onFUActivation(wxListEvent& event);
    void updateFUList(const std::string& fuName, int index);

    void onSetParameterValue(wxCommandEvent& event);
    void onBrowseDecompressor(wxCommandEvent& event);
    void onBrowseICDecPlugin(wxCommandEvent& event);
    void onBrowseICHDB(wxCommandEvent& event);
    
    void loadICDecoderPlugin(
        const std::string& pluginName,
        const std::string& pluginFile);
    void onParameterSelection(wxListEvent& event);

    virtual bool TransferDataToWindow();
    void onLoadIDF(wxCommandEvent& event);
    void onSaveIDF(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    void onGenerateProcessor(wxCommandEvent& event);
    void doSaveIDF();

    void checkImplementationFiles();

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    wxSizer* functionUnitPage(
        wxWindow* parent, bool call_fit, bool set_sizer = true);
    wxSizer* registerFilePage(
        wxWindow* parent, bool call_fit, bool set_sizer = true);
    wxSizer* immediateUnitPage(
        wxWindow* parent, bool call_fit, bool set_sizer = true);
    wxSizer* decompressionPage(
        wxWindow* parent, bool call_fit, bool set_sizer = true);
    wxSizer* icDecoderPluginPage(
        wxWindow* parent, bool call_fit, bool set_sizer = true);

    /// Machine that is being generated.
    TTAMachine::Machine& machine_;
    /// MachineImplementation object containing the implementation information.
    IDF::MachineImplementation& impl_;

    /// Pointer to the FU implementation list widget.
    wxListCtrl* fuList_;
    /// Pointer to the IU implementation list widget.
    wxListCtrl* iuList_;
    /// Pointer to the RF implementation list widget.
    wxListCtrl* rfList_;
    /// Pointer to the ic decoder parameter list widget.
    wxListCtrl* parameterList_;

    /// Plugintool for loading IC/Decoder plugins.
    PluginTools pluginTool_;
    /// IC/Decoder plugin of the current machine implementation.
    ProGe::ICDecoderGeneratorPlugin* plugin_;

    // Widget IDs.
    enum {
        ID_LOAD_IDF,
        ID_SAVE_IDF,
        ID_GENERATE_PROCESSOR,
        ID_FU_LIST,
        ID_IU_LIST,
        ID_RF_LIST,
        ID_SELECT_FU_IMPL,
        ID_SELECT_IU_IMPL,
        ID_SELECT_RF_IMPL,
        ID_BROWSE_DECOMPRESSOR,
        ID_BROWSE_IC_DEC_PLUGIN,
        ID_BROWSE_IC_DEC_HDB,
        ID_DECOMPRESSOR_PATH,
        ID_IC_DEC_PLUGIN_FILE,
        ID_IC_DEC_PLUGIN_NAME,
        ID_IC_DEC_PLUGIN_DESC,
        ID_IC_DEC_HDB_FILE,
        ID_PARAMETER_LIST,
        ID_PARAMETER_VALUE,
        ID_SET_VALUE,
        ID_NOTEBOOK,
        ID_GENERATE,
        ID_CLOSE,
        ID_TEXT,
        ID_LINE
    };
    
    bool dirtyData_;

    DECLARE_EVENT_TABLE()        
};

#endif
