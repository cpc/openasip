/*
 Copyright (C) 2024 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

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

    for (const auto option : options_) {
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
