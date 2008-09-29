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
