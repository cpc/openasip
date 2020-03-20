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
 * @file DialogPosition.cc
 *
 * Definition of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DialogPosition.hh"

using std::map;

// Initialize static data members.
map<DialogPosition::Dialogs, wxPoint> DialogPosition::positions_;

/**
 * Gets the position of the dialog.
 *
 * If position is not stored, returns wxDefaultPosition.
 *
 * @param dialog Dialog which position is asked.
 * @return The position of the dialog.
 */
wxPoint
DialogPosition::getPosition(Dialogs dialog) {
    PositionMap::iterator it = positions_.find(dialog);
    if (it == positions_.end()) {
        return wxDefaultPosition;
    }
    return (*it).second;
}

/**
 * Sets the position of the dialog.
 * 
 * @param dialog Dialog which position is set.
 */
void
DialogPosition::setPosition(Dialogs dialog, wxPoint point) {
    PositionMap::iterator it = positions_.find(dialog);
    if (it == positions_.end()) {
        positions_.insert(PositionMap::value_type(dialog, point));
    } else {
        (*it).second = point;
    }
}
