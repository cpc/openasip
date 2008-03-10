/**
 * @file FrequencySweep.hh
 *
 * Frequency sweeper that sweeps through the given frequency field by given 
 * steps.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
