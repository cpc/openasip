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
 * @file ImmInfo.hh
 *
 * Implementation/Declaration of ImmInfo class.
 *
 * Created on: 8.3.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef IMMINFO_HH
#define IMMINFO_HH

#include <map>
#include <utility>
#include <string>


class Operation;
class Operand;
namespace TTAMachine {
    class RegisterFile;
}

/**
 * The key definition for ImmInfo.
 */
using ImmInfoKey = std::pair<
    std::string, /* = The name of the operation. */
    int /* = The ID of the operand. */ >;

/**
 * The stored immediate result in ImmInfo.
 */
class ImmInfoValue : public std::pair<
    int, /* = The bit width of the immediate */
    bool /* = Sign extends */ > {
public:

    ImmInfoValue();
    ImmInfoValue(int immediateWidth, bool signExtending);

    int64_t lowerBound() const;
    int64_t upperBound() const;
    /**
     * Returns bit width of the immediate.
     */
    int width() const { return first; }
    /**
     * Return true if the immediate is sign extending.
     */
    bool signExtending() const { return second; }
};

/*
 * Container class for short immediate analysis results.
 *
 */
class ImmInfo : public std::multimap<ImmInfoKey, ImmInfoValue> {
public:
    ImmInfo() = default;
    virtual ~ImmInfo() = default;

    size_t count(const ImmInfoKey& key) const {
        return std::multimap<ImmInfoKey, ImmInfoValue>::count(key);
    }
    size_t count(const Operation& operation, const Operand& operand) const;
    size_t count(const Operation& operation, int inputOperandId) const;

    const ImmInfoValue& widestImmediate(const ImmInfoKey& key) const;
    const ImmInfoValue& widestImmediate(
        const Operation& operation, const Operand& operand) const;
    const ImmInfoValue& widestImmediate(
        const Operation& operation, int inputOperandId) const;
    const ImmInfoValue& narrowestImmediate(const ImmInfoKey& key) const;

    std::pair<int64_t, int64_t> immediateValueBounds(
        const ImmInfoKey& key, int destWidth) const;
    std::pair<int64_t, int64_t> immediateValueBounds(
        const Operation& operation, const Operand& operand,
        int destWidth) const;
    std::pair<int64_t, int64_t> immediateValueBounds(
        const Operation& operation, int inputOperandId,
        int destWidth) const;

    bool canTakeImmediate(
        const Operation& operation,
        int inputOperandId,
        int64_t value,
        int destWidth);

    bool canTakeImmediateByWidth(
        const Operation& operation,
        int inputOperandId,
        int bitWidth);

    static ImmInfoKey key(
        const Operation& operation,
        int inputOperandId);
    static ImmInfoKey key(
        const Operation& operation,
        const Operand& operand);

    static int registerImmediateLoadWidth(
        const TTAMachine::RegisterFile& targetRF,
        bool allowSignExtension = false);

};

#endif /* IMMINFO_HH */
