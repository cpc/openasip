/**
 * @file ADFPrintout.hh
 *
 * Declaration of ADFPrintout class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADF_PRINTOUT_HH
#define TTA_ADF_PRINTOUT_HH

#include <wx/print.h>

class MachineCanvas;

/**
 * Printout of an ADF-document.
 *
 * Handles printing of the machine canvas to printing DCs.
 */
class ADFPrintout : public wxPrintout {
public:
    ADFPrintout(MachineCanvas& canvas, const wxString& name);
    virtual ~ADFPrintout();

    virtual bool OnPrintPage(int page);

private:
    /// Machine canvas to print.
    MachineCanvas& canvas_;
};
#endif
