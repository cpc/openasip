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
 * @file ADFPrintout.cc
 *
 * Implementation of ADFPrintout class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
