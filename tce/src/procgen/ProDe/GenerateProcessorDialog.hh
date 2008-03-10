/**
 * @file GenerateProcessorDialog.hh
 *
 * Declaration of GenerateProcessorDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
