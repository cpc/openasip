/**
 * @file SimulationInfoDialog.hh
 *
 * Declaration of SimulationInfoDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_INFO_DIALOG_HH
#define TTA_SIMULATION_INFO_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "SimulateDialog.hh"
#include "OSEdInformer.hh"
#include "OSEdListener.hh"

class OperationContext;

/**
 * Dialog that holds additional information about simulation.
 */
class SimulationInfoDialog : public wxDialog, public OSEdListener {
public:
    SimulationInfoDialog(wxWindow* parent, OperationContext* context);
    virtual ~SimulationInfoDialog();

    virtual void handleEvent(OSEdInformer::EventId id);

private:
    /// Copying not allowed.
    SimulationInfoDialog(const SimulationInfoDialog&);
    /// Assignment not allowed.
    SimulationInfoDialog& operator=(const SimulationInfoDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void setTexts();
    void updateList();

    /**
     * Component ids.
     */
    enum {
        ID_LISTCTRL
    };

    /// List for showing simulation statistics.
    wxListCtrl* infoList_;
    /// Parent window.
    SimulateDialog* parent_;
    /// Operation context for retrieving simulation data.
    OperationContext* context_;
};

#endif
