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
 * @file FrequencySweep.hh
 *
 * Frequency sweeper that sweeps through the given frequency field by given 
 * steps.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FREQUENCY_SWEEP_HH
#define TTA_FREQUENCY_SWEEP_HH

#include "Exception.hh"

/**
 * Sweeps through the frequencies from lowest frequency towards the
 * highest frequency and returns all step frequencies including the start and
 * end frequency.
 *
 * FrequencySweep takes start and and frequencies and a step frequency that
 * tells how big frequency steps are taken between the start and end point.
 */
class FrequencySweep {
public:
    FrequencySweep(int startMHz, int endMHz, int stepMHz)
        throw (IllegalParameters);
    virtual ~FrequencySweep();
    int nextFrequency();
private:
    /// Next frequency to be returned.
    int nextFrequency_;
    /// Flag indicating is there next frequency to return.
    bool hasNext_;
    /// Start frequency.
    int startMHz_;
    /// End frequency.
    int endMHz_;
    /// Step frequency.
    int stepMHz_;
};

#endif
