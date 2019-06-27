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
 * @file FrequencySweep.cc
 *
 * Implementation of the FrequencySweep class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "FrequencySweep.hh"

/**
 * Constructor of the FrequencySweep.
 *
 * @param startMHz Start frequency in MHz.
 * @param endMHz End frequency in MHz.
 * @param stepMHz Stepping frequency in MHz.
 * @exception IllegalParameters Throws if the frequencies are smaller than one.
 */
FrequencySweep::FrequencySweep(int startMHz, int endMHz, int stepMHz)
    : hasNext_(true), endMHz_(endMHz), stepMHz_(stepMHz) {
    // Sweep is always done from the lowest frequency towards the highest
    // frequency.
    if (startMHz > endMHz) {
        int tempMHz = startMHz;
        startMHz = endMHz;
        endMHz = tempMHz;
    }
    
    if (startMHz < 1 || stepMHz < 1) {
        std::string message = "Parameter value lower than one.";
        throw IllegalParameters(__FILE__, __LINE__, __func__, message);
    }
    nextFrequency_ = startMHz;
}

/**
 * The destructor.
 */
FrequencySweep::~FrequencySweep() {
}

/**
 * Returns the next frequency or zero if all steps have been returned and steps
 * forward.
 *
 * @return Next frequency of zero if all steps have been returned.
 */
int
FrequencySweep::nextFrequency() {

    if (hasNext_ == false) {
        return 0;
    }
    int current = nextFrequency_;
    nextFrequency_ = current + stepMHz_;
    if (nextFrequency_ > endMHz_) {
        nextFrequency_ = endMHz_;
        if (current == nextFrequency_) {
            hasNext_ = false;
        }
    }
    return current;
}
