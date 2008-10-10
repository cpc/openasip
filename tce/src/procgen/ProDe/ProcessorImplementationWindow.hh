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

private:

    void onSelectRFImplementation(wxCommandEvent& event);
    void onRFSelection(wxListEvent& event);

    void onSelectIUImplementation(wxCommandEvent& event);
    void onIUSelection(wxListEvent& event);

    void onSelectFUImplementation(wxCommandEvent& event);
    void onFUSelection(wxListEvent& event);

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

    DECLARE_EVENT_TABLE()
};

#endif
