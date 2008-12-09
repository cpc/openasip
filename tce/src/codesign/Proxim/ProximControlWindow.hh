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
 * @file ProximControlWindow.cc
 *
 * Declaration of ProximControlWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_CONTROL_WINDOW_HH
#define TTA_PROXIM_CONTROL_WINDOW_HH


#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"

class ProximLineReader;
class SimulatorFrontend;
class wxSizer;

/**
 * Proxim subwindow with buttons to control the simulation process.
 *
 * ProximControlWindow doesn't handle button events. The events are
 * passed to the parent window. Button enabled status is updated
 * using wxUpdateUI events.
 */
class ProximControlWindow : public ProximSimulatorWindow {
public:
    ProximControlWindow(ProximMainFrame* parent, int id);
    virtual ~ProximControlWindow();
    virtual void reset();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
};

#endif
