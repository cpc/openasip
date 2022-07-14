/*
    Copyright (c) 2002-2017 Tampere University.

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
/**
 * @file FUGenerated.cc
 *
 * Implementation of FUGenerated.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */

#include "FUGenerated.hh"
#include "MachineImplementation.hh"

namespace IDF {

const std::string ATTRIB_NAME = "name";
const std::string TAG_OPERATION = "operation";
const std::string TAG_OPERATION_ID = "operation-id";
const std::string TAG_OPERATION_LATENCY = "operation-latency";
const std::string TAG_OPTION = "option";
const std::string TAG_FUGENERATE = "fu-generate";
const std::string TAG_HDBFILE = "hdb-file";

FUGenerated::FUGenerated(const std::string& name) : name_(name) {}

void
FUGenerated::loadState(const ObjectState* state) {
    name_ = state->stringAttribute(ATTRIB_NAME);

    for (int i = state->childCount() - 1; i >= 0; --i) {
        ObjectState* child = state->child(i);

        if (child->name() == TAG_OPERATION) {
            std::string opName = child->stringAttribute(ATTRIB_NAME);
            std::string hdbFile =
                child->childByName(TAG_HDBFILE)->stringValue();
            int id = child->childByName(TAG_OPERATION_ID)->intValue();
            int latency = 0;
            if (child->hasChild(TAG_OPERATION_LATENCY)) {
                latency =
                    child->childByName(TAG_OPERATION_LATENCY)->intValue();
            }
            Info info = {opName, hdbFile, id, latency};
            operations_.emplace_back(info);
        } else if (child->name() == TAG_OPTION) {
            options_.emplace_back(child->stringValue());
        }
    }
}

ObjectState*
FUGenerated::saveState() const {
    ObjectState* state = new ObjectState(TAG_FUGENERATE);

    state->setAttribute(ATTRIB_NAME, name_);

    for (const auto operation : operations_) {
        ObjectState* opState = new ObjectState(TAG_OPERATION);
        opState->setAttribute(ATTRIB_NAME, operation.operationName);
        ObjectState* hdbState = new ObjectState(TAG_HDBFILE);
        hdbState->setValue(operation.hdb);
        ObjectState* idState = new ObjectState(TAG_OPERATION_ID);
        idState->setValue(operation.id);
        ObjectState* latencyState = new ObjectState(TAG_OPERATION_LATENCY);
        latencyState->setValue(operation.latency);
        opState->addChild(hdbState);
        opState->addChild(idState);
        opState->addChild(latencyState);
        state->addChild(opState);
    }

    for (const auto option : options_) {
        ObjectState* opState = new ObjectState(TAG_OPTION);
        opState->setValue(option);
        state->addChild(opState);
    }

    return state;
}

std::string
FUGenerated::name() const {
    return name_;
}

void
FUGenerated::name(const std::string& newName) {
    name_ = newName;
}

std::vector<FUGenerated::Info>&
FUGenerated::operations() {
    return operations_;
}

const std::vector<FUGenerated::Info>&
FUGenerated::operations() const {
    return operations_;
}

const std::vector<std::string>&
FUGenerated::dagOperations() const {
    return dagOperations_;
}

void
FUGenerated::addOperation(const FUGenerated::Info& op) {
    operations_.emplace_back(op);
}

void
FUGenerated::addOperation(const FUGenerated::DAGOperation& op) {
    dagOperations_.emplace_back(op.operationName);
    for (auto&& subop : op.suboperations) {
        operations_.emplace_back(subop);
    }
}

const std::vector<std::string>&
FUGenerated::options() const {
    return options_;
}
}  // namespace IDF
