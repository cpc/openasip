/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file CanvasConstants.hh
 *
 * Declaration of CanvasConstants class.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#ifndef TTA_CANVAS_CONSTANTS_HH
#define TTA_CANVAS_CONSTANTS_HH


/**
 * Constants for machine canvas
 */
class CanvasConstants {
public:
    // zooming constants

    /// Minimum zoom factor.
    static const double MIN_ZOOM_FACTOR;
    /// Maximum zoom factor.
    static const double MAX_ZOOM_FACTOR;
    /// Step between zoom levels.
    static const double ZOOM_STEP;
    /// More precise step for automatic canvas resizing
    static const double AUTOZOOM_STEP;
    };

#endif
