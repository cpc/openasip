/**
 * @file ProDeBusOrderDialog.hh
 *
 * Declaration of ProDeBusOrderDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_BUS_ORDER_DIALOG_HH
#define TTA_PRODE_BUS_ORDER_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Machine;
}

class wxListCtrl;

/**
 * Dialog for modifying order of transport buses in a machine.
 */
class ProDeBusOrderDialog : public wxDialog {
public:
    ProDeBusOrderDialog(wxWindow* parent, TTAMachine::Machine& machine);
    virtual ~ProDeBusOrderDialog();
private:
    void onUp(wxCommandEvent& event);
    void onDown(wxCommandEvent& event);
    void onBusSelectionChanged(wxListEvent& event);
    void updateBusList();
    int selectedBus() const;
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    // Widget IDs.
    enum {
        ID_UP = 10000,
        ID_DOWN,
        ID_LINE,
        ID_LIST
    };

    /// Machine containing the buses to sort.
    TTAMachine::Machine& machine_;
    /// List widget for the buses.
    wxListCtrl* list_;

    DECLARE_EVENT_TABLE()
};

#endif
