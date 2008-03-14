/**
 * @file ModelConstants.cc
 *
 * Definition of ModelConstants class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#include <string>
#include "ModelConstants.hh"
#include "RegisterFile.hh"

using std::string;

const int ModelConstants::DEFAULT_WIDTH = 32;
const int ModelConstants::DEFAULT_RF_SIZE = 8;
const int ModelConstants::DEFAULT_RF_MAX_READS = 1;
const int ModelConstants::DEFAULT_RF_MAX_WRITES = 0;
const int ModelConstants::DEFAULT_RF_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_IU_SIZE = 4;
const int ModelConstants::DEFAULT_IU_CYCLES = 1;
const int ModelConstants::DEFAULT_IU_MAX_READS = 1;
const int ModelConstants::DEFAULT_IU_MAX_WRITES = 1;
const int ModelConstants::DEFAULT_IU_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_AS_MAU_WIDTH = 8;
const int ModelConstants::DEFAULT_AS_MIN_ADDRESS = 0;
const int ModelConstants::DEFAULT_AS_MAX_ADDRESS = 1048576;
const int ModelConstants::DEFAULT_AS_INTERLEAVING_FACTOR = 1;

