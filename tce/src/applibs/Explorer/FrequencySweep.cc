/**
 * @file FrequencySweep.cc
 *
 * Implementation of the FrequencySweep class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
