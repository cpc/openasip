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
 * @file ArbiterImplementation.cc
 *
 * Implementation of ArbiterImplementation class.
 *
 * Created on: 7.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ArbiterImplementation.hh"

#include "ObjectState.hh"

namespace IDF {

const std::string ArbiterImplementation::OSNAME_ARBITER_IMPLEMENTATION =
    "instruction-arbiter";
const std::string ArbiterImplementation::OSKEY_ARBITRATION_SCHEME =
    "arbitration-scheme";
const std::string ArbiterImplementation::OSKEY_MAX_TIME_SLICE =
    "maximum-time-slice";

ArbiterImplementation::ArbiterImplementation() {
}

ArbiterImplementation::ArbiterImplementation(const ObjectState* state) {
    loadState(state);
}

ArbiterImplementation::~ArbiterImplementation() {
}


ArbiterImplementation::ArbitrationScheme
ArbiterImplementation::arbitrationScheme() const {
    return arbitrationScheme_;
}

void
ArbiterImplementation::setArbitrationScheme(ArbitrationScheme scheme) {
    arbitrationScheme_ = scheme;
}

unsigned
ArbiterImplementation::maximumTimeSlice() const {
    return maximumTimeSlice_;
}

void
ArbiterImplementation::setMaximumTimeSlice(unsigned value) {
    maximumTimeSlice_ = value;
}

void
ArbiterImplementation::loadState(const ObjectState* state) {
    if (state->name() != OSNAME_ARBITER_IMPLEMENTATION) {
        THROW_EXCEPTION(ObjectStateLoadingException, "Invalid object state.");
    }
    try {
        arbitrationScheme_ = unserializeArbitrationScheme(
            state->childByName(OSKEY_ARBITRATION_SCHEME)->stringValue());
        if (state->hasChild(OSKEY_MAX_TIME_SLICE)) {
            maximumTimeSlice_ =
                state->childByName(OSKEY_MAX_TIME_SLICE)->unsignedIntValue();
        } else {
            maximumTimeSlice_ = 0;
        }
    } catch (const Exception& exception) {
        THROW_EXCEPTION(ObjectStateLoadingException, exception.errorMessage());
    }
}

ObjectState*
ArbiterImplementation::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_ARBITER_IMPLEMENTATION);

    ObjectState* arbScheme = new ObjectState(OSKEY_ARBITRATION_SCHEME);
    arbScheme->setValue(serialize(arbitrationScheme_));
    state->addChild(arbScheme);

    if (maximumTimeSlice_ > 0) {
        ObjectState* maxTimeSlice = new ObjectState(OSKEY_MAX_TIME_SLICE);
        maxTimeSlice->setValue(maximumTimeSlice_);
        state->addChild(maxTimeSlice);
    }

    return state;
}

ArbiterImplementation::ArbitrationScheme
ArbiterImplementation::unserializeArbitrationScheme(const std::string& str) {
    if (str == "round-robin") {
        return ArbiterImplementation::ROUND_ROBIN;
    } else {
        THROW_EXCEPTION(NoKnownConversion, "Unknown arbitration scheme: "
            + str + ".");
    }
}

std::string
ArbiterImplementation:: serialize(
    ArbiterImplementation::ArbitrationScheme scheme) {

    if (scheme == ROUND_ROBIN) {
        return "round-robin";
    } else {
        assert(false && "unknown arbitration scheme.");
        return "";
    }
}

} /* namespace ProGe */
