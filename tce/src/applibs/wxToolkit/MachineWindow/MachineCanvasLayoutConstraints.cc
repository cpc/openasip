/*
    Copyright (c) 2014 Tampere University of Technology.

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
 * @file MachineCanvasLayoutConstraints.cc
 *
 * Clang++ 3.4 did not compile-time resolve all the static member variable
 * constants, leading to linkage errors. Therefore moved the intializations
 * here.
 *
 * @author Pekka Jääskeläinen 2014
 */
#include "MachineCanvasLayoutConstraints.hh"

const int MachineCanvasLayoutConstraints::UNIT_WIDTH = 85;
const int MachineCanvasLayoutConstraints::UNIT_HEIGHT = 45;
const int MachineCanvasLayoutConstraints::UNIT_SPACE = 24;
const int MachineCanvasLayoutConstraints::MAX_UNIT_NAME_WIDTH = 250;
const int MachineCanvasLayoutConstraints::PORT_WIDTH = 16;
const int MachineCanvasLayoutConstraints::PORT_MIN_WIDTH = 9;
const int MachineCanvasLayoutConstraints::PORT_BASE_WIDTH = 5;
const int MachineCanvasLayoutConstraints::PORT_SPACE = 14;
const int MachineCanvasLayoutConstraints::SOCKET_WIDTH = 16;
const int MachineCanvasLayoutConstraints::SOCKET_HEIGHT = 100;
const int MachineCanvasLayoutConstraints::SOCKET_SPACE = SOCKET_WIDTH;
const int MachineCanvasLayoutConstraints::SOCKET_MIN_SPACE = 2;
const int MachineCanvasLayoutConstraints::SEGMENT_SPACE = 16;
const int MachineCanvasLayoutConstraints::SEGMENT_HEIGHT_BASE = 3;
const int MachineCanvasLayoutConstraints::BUS_MIN_WIDTH = 206;
const int MachineCanvasLayoutConstraints::BUS_MIN_HEIGHT = SEGMENT_HEIGHT_BASE;
const int MachineCanvasLayoutConstraints::BUS_SPACE = 24;
const int MachineCanvasLayoutConstraints::BRIDGE_WIDTH = BUS_MIN_WIDTH;
const int MachineCanvasLayoutConstraints::BRIDGE_HEIGHT = 24;
const int MachineCanvasLayoutConstraints::BRIDGE_SPACE = 12;
const int MachineCanvasLayoutConstraints::VIEW_MARGIN = 30;
const int MachineCanvasLayoutConstraints::VIEW_LEFT_MARGIN = 40;
const int MachineCanvasLayoutConstraints::CONNECTIONS_SPACE = 80;
const int MachineCanvasLayoutConstraints::TRIANGLE_HEIGHT = 15;
