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
 * @file LHSValue.hh
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#include <LHSValue.hh>
#include <BinaryOps.hh>
#include <StringTools.hh>
#include <cassert>

namespace HDLGenerator {

void
LHSValue::hdl(std::ostream& stream, Language lang, int level) {
    stream << StringTools::indent(level);
    hdl(stream, lang);
}

void
LHSValue::hdl(std::ostream& stream, Language lang) {
    if (lang == Language::VHDL) {
        stream << vhdl_;
    } else {
        stream << verilog_;
    }
}

void
LHSValue::writeSignals(std::unordered_set<std::string>& readList) const {
    for (auto a : readList_) {
        readList.insert(a);
    }
}

LHSValue
LHSValue::operator||(LHSValue rhs) {
    return LogicalOr(*this, rhs);
}

LHSValue
LHSValue::operator|(LHSValue rhs) {
    return BitwiseOr(*this, rhs);
}

LHSValue
LHSValue::operator&&(LHSValue rhs) {
    return LogicalAnd(*this, rhs);
}

LHSValue
LHSValue::operator&(LHSValue rhs) {
    return BitwiseAnd(*this, rhs);
}

LHSValue
LHSValue::operator^(LHSValue rhs) {
    return BitwiseXor(*this, rhs);
}

LHSValue
LHSValue::operator!() {
    return LogicalNot(*this);
}

LHSValue
LHSValue::operator~() {
    return BitwiseNot(*this);
}

LHSSignal::LHSSignal(std::string name) {
    vhdl_ = name;
    verilog_ = name;
    readList_.insert(name);
}

BinaryLiteral::BinaryLiteral(std::string value) : value_(value) {
    std::string delim;
    if (value.size() > 1) {
        delim = "\"";
    } else {
        delim = "'";
    }
    vhdl_ = delim + value + delim;
    verilog_ = value;
}

BinaryLiteral::BinaryLiteral(int value, int width, bool signExtend)
    : value_(std::to_string(value)) {
    // e.g. std_logic_vector(to_signed(value, width))
    std::string type = signExtend ? "signed" : "unsigned";
    vhdl_ = "std_logic_vector(to_" + type + "(" + std::to_string(value) +
            ", " + std::to_string(width) + "))";

    verilog_ = value;
}
}  // namespace HDLGenerator
