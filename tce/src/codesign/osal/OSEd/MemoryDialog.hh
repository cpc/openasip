/**
 * @file MemoryDialog.hh
 *
 * Declaration of MemoryDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_DIALOG_HH
#define TTA_MEMORY_DIALOG_HH


#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/textctrl.h>

#include "BaseType.hh"
#include "OSEdListener.hh"

class OperationContext;
class MemoryControl;

/**
 * Dialog for monitoring the contents of the memory.
 */
class MemoryDialog : public wxDialog, public OSEdListener {
public:
    MemoryDialog(wxWindow* window);
    virtual ~MemoryDialog();

    virtual void handleEvent(OSEdInformer::EventId event);

private:
    /// Copying not allowed.
    MemoryDialog(const MemoryDialog&);
    /// Assignment not allowed.
    MemoryDialog& operator=(const MemoryDialog&);

    wxSizer* createContents(wxWindow* window, bool call_fit, bool set_sizer);
    void setTexts();
    void onClose(wxCommandEvent&);

    /**
     * Component ids.
     */
    enum {
        ID_BUTTON_CLOSE = 1000,
        ID_CONTROL_MEMORY
    };

    /// Memory control of the dialog.
    MemoryControl* memoryWindow_;

    DECLARE_EVENT_TABLE()
};

#endif
