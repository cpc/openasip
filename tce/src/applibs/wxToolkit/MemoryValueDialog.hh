/**
 * @file MemoryValueDialog.hh
 *
 * Declaration of MemoryValueDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_VALUE_DIALOG_HH
#define TTA_MEMORY_VALUE_DIALOG_HH

#include <wx/wx.h>
#include <string>

class NumberControl;

/**
 * Class for giving new values to memory addresses.
 */
class MemoryValueDialog : public wxDialog {
public:
    MemoryValueDialog(wxWindow* parent, unsigned maxBits = 64);
    virtual ~MemoryValueDialog();

    void setValue(int value);
    long mode();
    int intValue();
    double doubleValue();

private:

    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /**
     * Widget ids.
     */
    enum {
        ID_VALUE = 100000
    };

    /// NumberControl for giving new values to memory.
    NumberControl* value_;
    /// Maximum number of bits used by the returned value.
    unsigned maxBits_;

    DECLARE_EVENT_TABLE()
};

#endif
