/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file DialogPosition.hh
 *
 * Declaration of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DIALOG_POSITION_HH
#define TTA_DIALOG_POSITION_HH

#include <wx/wx.h>
#include <map>

/**
 * Class that holds the positions of dialogs.
 */
class DialogPosition {
public:
    /**
     * Enumeration of all dialogs in application.
     */
    enum Dialogs {
        DIALOG_PROPERTIES,        ///< Operation properties dialog.
        DIALOG_SIMULATE,          ///< Simulation dialog.
        DIALOG_INPUT_OPERAND,     ///< Input operand dialog.
        DIALOG_OUTPUT_OPERAND,    ///< Output operand dialog.
        DIALOG_RESULT,            ///< Result dialog.
        DIALOG_OPTIONS,           ///< Options dialog.
        DIALOG_ADD_MODULE,        ///< Add module dialog.
        DIALOG_SIMULATION_INFO,   ///< Simulation info dialog.
        DIALOG_MEMORY,            ///< Memory dialog.
        DIALOG_ABOUT              ///< About dialog.
    };

    static wxPoint getPosition(Dialogs dialog);
    static void setPosition(Dialogs dialog, wxPoint point);

    DialogPosition(const DialogPosition&) = delete;
    DialogPosition& operator=(const DialogPosition&) = delete;

private:
    /// Container for dialog positions.
    typedef std::map<Dialogs, wxPoint> PositionMap;

    DialogPosition();
    ~DialogPosition();

    /// Contains all dialog positions.
    static PositionMap positions_;
};

#endif
