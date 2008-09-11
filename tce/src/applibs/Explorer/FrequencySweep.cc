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
    throw (IllegalParameters):
    hasNext_(true), startMHz_(startMHz), endMHz_(endMHz), stepMHz_(stepMHz) {

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
