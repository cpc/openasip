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
 * @file ModelConstants.cc
 *
 * Definition of ModelConstants class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include <string>
#include "ModelConstants.hh"
#include "RegisterFile.hh"

using std::string;

const int ModelConstants::DEFAULT_WIDTH = 32;
const int ModelConstants::DEFAULT_RF_SIZE = 8;
const int ModelConstants::DEFAULT_RF_MAX_READS = 0;
const int ModelConstants::DEFAULT_RF_MAX_WRITES = 0;
const int ModelConstants::DEFAULT_RF_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_IU_SIZE = 4;
const int ModelConstants::DEFAULT_IU_CYCLES = 1;
const int ModelConstants::DEFAULT_IU_MAX_READS = 1;
const int ModelConstants::DEFAULT_IU_MAX_WRITES = 1;
const int ModelConstants::DEFAULT_IU_GUARD_LATENCY = 0;
const int ModelConstants::DEFAULT_AS_MAU_WIDTH = 8;
const int ModelConstants::DEFAULT_AS_MIN_ADDRESS = 0;
const int ModelConstants::DEFAULT_AS_MAX_ADDRESS = 1048575;
const int ModelConstants::DEFAULT_AS_INTERLEAVING_FACTOR = 1;

