/*
    Copyright (c) 2018 Tampere University.

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
* @file HDLGenerator.hh
*
* @author Aleksi Tervo 2018 (aleksi.tervo-no.spam-tut.fi)
*/
#pragma once
#include "HWGenTools.hh"
#include "LHSValue.hh"
#include "StringTools.hh"
#include <boost/format.hpp>
#include <chrono>
#include <cmath>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace HDLGenerator {
    /**
     * Sync signal/sync reg.
     */
    class Register {
    public:
        Register(std::string name, int width, WireType wt,
            ResetOption rst = ResetOption::Mandatory)
            : name_(name), strWidth_(), width_(width), wt_(wt),
              rst_(rst), literal_("") {}
        Register(std::string name, int width = 1,
            ResetOption rst = ResetOption::Mandatory)
            : name_(name), strWidth_(), width_(width),
              wt_(WireType::Auto), rst_(rst), literal_("") {}
        Register(std::string name, std::string width,
            ResetOption rst = ResetOption::Mandatory)
            : name_(name), strWidth_(width), width_(-1),
              wt_(WireType::Auto), rst_(rst), literal_("") {}

        Width width() noexcept { return {strWidth_, width_}; }

        std::string name() { return name_; }

        Register& setResetValue(BinaryLiteral&& rhs) {
            literal_ = rhs;
            return *this;
        }

        void reset(std::ostream& stream, Language lang, int ident) {
            stream << StringTools::indent(ident) << name() << " <= ";
            if (lang == Language::VHDL) {
                if (literal_.name().empty()) {
                    if (width_ == 1 && wt_ != WireType::Vector) {
                        stream << "'0';\n";
                    } else {
                        stream << "(others => '0');\n";
                    }
                } else {
                    literal_.hdl(stream, lang);
                    stream << ";\n";
                }
            } else if (lang == Language::Verilog) {
                if (literal_.name().empty()) {
                    stream << "0;\n";
                } else {
                    literal_.hdl(stream, lang);
                    stream << ";\n";
                }
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        void declare(std::ostream& stream, Language lang, int ident) {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(ident) << "signal "
                       << name() << " : ";
                if (width_ < 0 || width_ > 1 || wt_ == WireType::Vector) {
                    if (strWidth_.empty()) {
                        stream << "std_logic_vector("
                               << std::to_string(width_ - 1)
                               << " downto 0);\n";
                    } else {
                        stream << "std_logic_vector(" << strWidth_
                               << "-1 downto 0);\n";
                    }
                } else {
                    stream << "std_logic;\n";
                }
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(ident) << "reg ";
                if (width_ < 0 || width_ > 1) {
                    if (strWidth_.empty()) {
                        stream << "[" << std::to_string(width_ - 1) << ":0] ";
                    } else {
                        stream << "[" << strWidth_ << "-1:0] ";
                    }
                }
                stream << name() << ";\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        ResetOption resetOption() const noexcept { return rst_; }

    private:
        std::string name_;
        std::string strWidth_;
        int width_;
        WireType wt_;
        ResetOption rst_;
        BinaryLiteral literal_;
    };
}
