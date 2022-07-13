/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file SignalGroupDefinitions.cc
 *
 * Created on: 3.6.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "SignalGroupDefinitions.hh"
#include "SignalTypes.hh"
#include "SignalGroupTypes.hh"

#include <map>

namespace ProGe {

// Signal Group Definitions //
namespace {
const std::map<SignalGroupType, std::set<SignalType> > definitions {
    {
        SignalGroupType::INSTRUCTION_LINE, {
            SignalType::READ_REQUEST,
            SignalType::ADDRESS,
            SignalType::FETCHBLOCK,
            SignalType::READ_REQUEST_READY
        }
    }, {
        SignalGroupType::BURST_INSTRUCTION, {
            SignalType::BURST_READ_REQUEST,
            SignalType::ADDRESS,
            SignalType::FETCHBLOCK,
            SignalType::BURST_READ_REQUEST_READY,
        }
    }, {
        SignalGroupType::BITMASKED_SRAM_PORT, {
            SignalType::ADDRESS,
            SignalType::WRITE_DATA,
            SignalType::READ_DATA,
            SignalType::WRITEMODE,
            SignalType::WRITE_BITMASK,
            SignalType::READ_WRITE_REQUEST
        }
    }, {
        SignalGroupType::BYTEMASKED_SRAM_PORT, {
            SignalType::AVALID,
            SignalType::AREADY,
            SignalType::AADDR,
            SignalType::AWREN,
            SignalType::ASTRB,
            SignalType::RVALID,
            SignalType::RREADY,
            SignalType::RDATA,
            SignalType::ADATA
        }
    }
}; /* end of definitions */
} /* end of anonymous namespace*/

/**
 * Returns set of SignalTypes defined by SignalGroupType.
 */
std::set<SignalType>
signalGroupDefinition(SignalGroupType type) {
    if (definitions.count(type)) {
        return definitions.at(type);
    } else {
        return std::set<SignalType>();
    }
}

} /* namespace ProGe */
