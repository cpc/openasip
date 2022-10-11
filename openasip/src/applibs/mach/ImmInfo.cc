/*
 Copyright (c) 2002-2016 Tampere University.

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
 * @file ImmInfo.cc
 *
 * Implementation/Declaration of ImmInfo class.
 *
 * Created on: 8.3.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ImmInfo.hh"

#include <algorithm>

#include "Machine.hh"
#include "RegisterFile.hh"
#include "Bus.hh"
#include "ImmediateUnit.hh"
#include "InstructionTemplate.hh"

#include "Operation.hh"
#include "Operand.hh"

#include "MachineConnectivityCheck.hh"

#include "Exception.hh"
#include "TCEString.hh"
#include "MathTools.hh"

/**
 * Returns key for ImmInfo data structure.
 */
ImmInfoKey
ImmInfo::key(const Operation& operation, int inputOperandId) {
    return std::make_pair(operation.name().upper(), inputOperandId);
}
ImmInfoKey
ImmInfo::key(const Operation& operation, const Operand& operand) {
    return std::make_pair(operation.name().upper(), operand.index());
}


/**
 * Returns the entry count for operation, operand pair (key).
 */
size_t
ImmInfo::count(const Operation& operation, const Operand& operand) const {
    return std::multimap<ImmInfoKey, ImmInfoValue>::count(
        key(operation, operand));
}


/**
 * Returns the entry count for operation, operand pair (key).
 */
size_t
ImmInfo::count(const Operation& operation, int inputOperandId) const {
    return count(operation, operation.operand(inputOperandId));
}


/**
 * Returns the entry count by the key.
 */
const ImmInfoValue&
ImmInfo::widestImmediate(const ImmInfoKey& key) const {
    if (std::multimap<ImmInfoKey, ImmInfoValue>::count(key)) {
        const_iterator it;
        const_iterator it_largest = this->lower_bound(key);
        const_iterator it_begin = it_largest;
        it_begin++;
        const_iterator it_end = this->upper_bound(key);
        for (it = it_begin; it != it_end; it++) {
            if (*it > *it_largest) {
                it_largest = it;
            }
        }
        assert(it_largest != it_end);
        return it_largest->second;
    } else {
        THROW_EXCEPTION(InstanceNotFound, "No immediate info for key found.");
    }
}

/**
 * returns widest immediate bit width by the key.
 *
 * @param operation The operation as the part of the key.
 * @param inputOperandId The input operand of the operation as the
 *                       of the key.
 */
const ImmInfoValue&
ImmInfo::widestImmediate(
    const Operation& operation, const Operand& operand) const {

    return widestImmediate(key(operation, operand));
}

/**
 * returns widest immediate bit width by the key.
 *
 * @param operation The operation as the part of the key.
 * @param inputOperandId The input operand id of the operation as the
 *                       of the key.
 */
const ImmInfoValue&
ImmInfo::widestImmediate(
    const Operation& operation, int inputOperandId) const {

    return widestImmediate(operation, operation.operand(inputOperandId));
}

/**
 * returns narrowest (non-zero width) immediate bit width by the key.
 */
const ImmInfoValue&
ImmInfo::narrowestImmediate(const ImmInfoKey& key) const {
    if (std::multimap<ImmInfoKey, ImmInfoValue>::count(key)) {
        const_iterator it;
        const_iterator it_largest = lower_bound(key);
        const_iterator it_begin = it_largest;
        it_begin++;
        const_iterator it_end = upper_bound(key);
        for (it = it_begin; it != it_end; it++) {
            if (*it < *it_largest) {
                it_largest = it;
            }
        }
        assert(it_largest != it_end);
        return it_largest->second;
    } else {
        THROW_EXCEPTION(InstanceNotFound,
            "No immediate info found for the key.");
    }
}


/**
 * Default constructor. Set as zero width and as zero extending.
 */
ImmInfoValue::ImmInfoValue()
    : pair(0, false) {
}


/**
 * Constructor with the specified immediate width and sign-extension.
 */
ImmInfoValue::ImmInfoValue(int immediateWidth, bool signExtending)
    : pair(immediateWidth, signExtending) {
}


/**
 * Return largest inclusive value that can be expressed as immediate.
 */
int64_t
ImmInfoValue::lowerBound() const {
    if (second) { // is sign extending?
        return -(1ll << (first-1));
    } else { // is then zero extending.
        return 0;
    }
}

/**
 * Return smallest inclusive value that can be expressed as immediate.
 */
int64_t
ImmInfoValue::upperBound() const {
    if (second) { // is sign extending?
        return (1ll << (first-1))-1;
    } else { // is then zero extending.
        return (1ll << (first));
    }
}


/**
 * Returns smallest and largest number that can be transported to operation
 * operand.
 *
 * The bounds are merged from multiple immediate sources. For example, if
 * immediates can be transported from two different sources, where the one is
 * 4 bits zero extending and the another is 3 bits sign extending, the
 * resulting bound is [-4, 15].
 *
 * if destination bit width is specified, then the bounds are ...TODO
 *
 * @param destWidth The destination width, where the immediates are
 *                  transported to. By default it is unspecified/unlimited.
 * @return std::pair, where the first is smallest value and second is largest.
 *         If no immediate can be transported, (0, 0) is returned.
 */
std::pair<int64_t, int64_t>
ImmInfo::immediateValueBounds(
    const ImmInfoKey& key, int destWidth) const {

    std::pair<int64_t, int64_t> result{ 0, 0 };

    const_iterator it_begin = lower_bound(key);
    const_iterator it_end = upper_bound(key);
    const_iterator it;
    for (it = it_begin; it != it_end; it++) {
        if (destWidth > 0) {
            if (it->second.width() >= destWidth) {
                return { (-(1ll << (destWidth-1))), ((1ll << destWidth)-1) };
            }
        }

        int64_t currLowerBound = 0;
        int64_t currUpperBound = 0;
        if (it->second.signExtending()) {
            currLowerBound = -(1ll << (it->second.width()-1));
            currUpperBound = (1ll << (it->second.width()-1))-1;
        } else {
            currUpperBound = (1ll << (it->second.width()))-1;
        }

        result.first = std::min(result.first, currLowerBound);
        result.second = std::max(result.second, currUpperBound);
    }

    return result;
}


bool
ImmInfo::canTakeImmediate(
    const Operation& operation,
    int inputOperandId,
    int64_t value,
    int /*destWidth*/) {

    auto unsignedReqBits = MathTools::requiredBits(value);
    auto signedReqBits = MathTools::requiredBitsSigned(value);
    auto theKey = key(operation, inputOperandId);
    const_iterator it_begin = lower_bound(theKey);
    const_iterator it_end = upper_bound(theKey);
    const_iterator it;
    for (it = it_begin; it != it_end; it++) {
        const ImmInfoValue& imm = it->second;
        if ((imm.signExtending() && signedReqBits <= imm.width()) ||
            (unsignedReqBits <= imm.width())) {
            return true;
        }
    }
    return false;
}

bool
ImmInfo::canTakeImmediateByWidth(
    const Operation& operation,
    int inputOperandId,
    int bitWidth) {

    auto theKey = key(operation, inputOperandId);
    const_iterator it_begin = lower_bound(theKey);
    const_iterator it_end = upper_bound(theKey);
    const_iterator it;
    for (it = it_begin; it != it_end; it++) {
        const ImmInfoValue& info = it->second;
        if (bitWidth <= info.width()) return true;
    }
    return false;
}

/**
 * Same as immediateValueBounds(const ImmInfoKey& key).
 *
 */
std::pair<int64_t, int64_t>
ImmInfo::immediateValueBounds(
    const Operation& operation,
    const Operand& operand,
    int destWidth) const {
    return immediateValueBounds(key(operation, operand), destWidth);
}


/**
 * Same as immediateValueBounds(const ImmInfoKey& key).
 *
 */
std::pair<int64_t, int64_t>
ImmInfo::immediateValueBounds(
    const Operation& operation,
    int inputOperandId,
    int destWidth) const {

    return immediateValueBounds(
        operation, operation.operand(inputOperandId), destWidth);
}

/**
 * Returns maximum immediate in bit width which can be transported to the RF.
 *
 * @param targetRF The target register file.
 * @param allowSignExtension If representation of the transported immediate
 *                           should not change set it to false (default).
 */
int
ImmInfo::registerImmediateLoadWidth(
    const TTAMachine::RegisterFile& targetRF,
    bool allowSignExtension) {

    using namespace TTAMachine;
    using MCC = MachineConnectivityCheck;

    int result = 0;
    assert(targetRF.machine());
    const Machine& mach = *targetRF.machine();

    for (const Bus* bus : mach.busNavigator()) {
        if (MCC::busConnectedToRF(*bus, targetRF)) {
            int immWidth = bus->immediateWidth();
            if (!allowSignExtension && bus->signExtends()) {
                immWidth -= 1;
            }
            result = std::max(result, immWidth);
        }
    }

    for (auto* iu : mach.immediateUnitNavigator()) {
        for (auto* it : mach.instructionTemplateNavigator()) {
            int supportedWidth = it->supportedWidth(*iu);
            if (!allowSignExtension && iu->signExtends()) {
                supportedWidth -= 1;
            }
            result = std::max(result, supportedWidth);
        }
    }

    return result;
}


