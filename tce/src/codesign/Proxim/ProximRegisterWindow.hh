/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file ProximRegisterWindow.hh
 *
 * Declaration of ProximRegisterWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */


#ifndef TTA_PROXIM_REGISTER_WINDOW_HH
#define TTA_PROXIM_REGISTER_WINDOW_HH

#include "SimulatorEvent.hh"
#include "ProximUnitWindow.hh"

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
}

class ProximMainFrame;

/**
 * Proxim subwindow which displays register values.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximRegisterWindow : public ProximUnitWindow {
public:
    ProximRegisterWindow(ProximMainFrame* parent, int id);
    virtual ~ProximRegisterWindow();
    void showRegisterFile(const std::string& name);
    void showImmediateUnit(const std::string& name);

private:
    void loadRegisterFile(const TTAMachine::RegisterFile& rf);
    void loadImmediateUnit(const TTAMachine::ImmediateUnit& iu);
    virtual void reinitialize();
    virtual void update();

    /// Prefix for register files in the unit choicer.
    static const std::string RF_PREFIX;
    /// Prefix for immediate units in the unit choicer.
    static const std::string IMM_PREFIX;
};

#endif
