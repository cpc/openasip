/**
 * @file ADFPrintout.cc
 *
 * Implementation of ADFPrintout class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include "ADFPrintout.hh"
#include "MachineCanvas.hh"

/**
 * The Contructor.
 */
ADFPrintout::ADFPrintout(MachineCanvas& canvas, const wxString& name) :
    wxPrintout(name),
    canvas_(canvas) {

}

/**
 * The Destructor.
 */
ADFPrintout::~ADFPrintout() {
}


/**
 * Called by the framework when a page should be printed.
 */
bool
ADFPrintout::OnPrintPage(int /* page */) {


    wxDC* dc = GetDC();

    dc->Clear();

    int machWidth = 0;
    int machHeight = 0;

    int paperWidth = 0;
    int paperHeight = 0;

    canvas_.GetVirtualSize(&machWidth, &machHeight);
    dc->GetSize(&paperWidth, &paperHeight);

    double scaleX = ((double) paperHeight / (double) machHeight) * 0.9;
    double scaleY = ((double) paperWidth / (double) machWidth) * 0.9;

    double scale = 0;

    // Keep aspect ratio by scaling X and Y with the same variable.
    if (scaleX < scaleY) {
        scale = scaleX;
    } else {
        scale = scaleY;
    }

    if (scale > 1) {
        // Do not scale up.
        scale = 1;
    }

    // Add a 20-pixel marigin to keep the figure in the printable area of
    // the paper.
    dc->SetDeviceOrigin(20, 20);
    dc->SetClippingRegion(0, 0, machWidth + 40, machHeight + 40);
    // scale the DC to fit the paper.
    dc->SetUserScale(scale, scale);

    canvas_.OnDraw(*dc);

    return true;
}
