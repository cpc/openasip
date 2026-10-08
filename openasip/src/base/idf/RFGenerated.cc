/*
 Copyright (C) 2024-2026 Tampere University.

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
 * @file RFGenerated.cc
 *
 * Implementation of RFGenerated.
 *
 * @author Joonas Multanen 2024 (joonas.multanen-no-spam-tuni.fi)
 * @note rating: red
*/

#include "RFGenerated.hh"
#include "MachineImplementation.hh"

namespace IDF {

const std::string ATTRIB_NAME = "name";
const std::string TAG_OPTION = "option";
const std::string TAG_RFGENERATE = "rf-generate";

RFGenerated::RFGenerated(const std::string& name) : name_(name) {}

void
RFGenerated::loadState(const ObjectState* state) {
    name_ = state->stringAttribute(ATTRIB_NAME);

    for (int i = state->childCount() - 1; i >= 0; --i) {
        ObjectState* child = state->child(i);
        options_.emplace_back(child->stringValue());
    }
}

ObjectState*
RFGenerated::saveState() const {
    ObjectState* state = new ObjectState(TAG_RFGENERATE);

    state->setAttribute(ATTRIB_NAME, name_);

    for (const auto& option : options_) {
        ObjectState* opState = new ObjectState(TAG_OPTION);
        opState->setValue(option);
        state->addChild(opState);
    }

    return state;
}

std::string
RFGenerated::name() const {
    return name_;
}

void
RFGenerated::name(const std::string& newName) {
    name_ = newName;
}

const std::vector<std::string>&
RFGenerated::options() const {
    return options_;
}
}  // namespace IDF
