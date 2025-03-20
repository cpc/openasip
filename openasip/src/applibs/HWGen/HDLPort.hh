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
 * @file Port.hh
 *
 * Port classes for HDLGenerator
 *
 * @author Aleksi Tervo 2018 (aleksi.tervo-no.spam-tut.fi)
 */

#include "HWGenTools.hh"

namespace HDLGenerator {
    /**
     * Entity/module port base class.
     */
    class Port {
    public:
        Port(std::string name, Direction dir, int width = 1,
             WireType wireType = WireType::Auto)
            : name_(name), dir_(dir), width_(width), wireType_(wireType) {}
        Port(std::string name, Direction dir, std::string parametricWidth,
             WireType wireType = WireType::Auto)
            : name_(name), dir_(dir), parametricWidth_(parametricWidth),
              width_(-1), wireType_(wireType) {}

        virtual ~Port() = default;

        void declare(std::ostream& stream, Language lang, int level = 0) {
            stream << StringTools::indent(level);
            if (lang == Language::VHDL) {
                stream << name_;
                if (dir_ == Direction::In) {
                    stream << " : in ";
                } else {
                    stream << " : out ";
                }
                if (!parametricWidth_.empty()) {
                    stream << "std_logic_vector(" << parametricWidth_
                           << "-1 downto 0)";
                } else if (wireType_ == WireType::Vector || width_ > 1) {
                    stream << "std_logic_vector("
                           << std::to_string(width_) << "-1 downto 0)";
                } else {
                    stream << "std_logic";
                }
            } else if (lang == Language::Verilog) {
                if (dir_ == Direction::In) {
                    stream << "input ";
                } else {
                    stream << "output logic ";
                }
                if (!parametricWidth_.empty()) {
                    stream << "[" << parametricWidth_ << "-1:0] ";
                } else if (wireType_ == WireType::Vector || width_ > 1) {
                    stream << "[" << std::to_string(width_ - 1) << ":0] ";
                }
                stream << name_;
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        };

        std::string name() { return name_; }

        Width width() { return {parametricWidth_, width_}; }

        WireType wireType() const { return wireType_; }

    protected:
        std::string name_;
        Direction dir_;
        std::string parametricWidth_;
        int width_;
        WireType wireType_;
    };

    class OutPort : public Port {
    public:
        OutPort(std::string name, int width = 1,
                WireType wireType = WireType::Auto)
            : Port(name, Direction::Out, width, wireType) {}
        OutPort(std::string name, std::string parametricWidth,
                WireType wireType = WireType::Auto)
            : Port(name,  Direction::Out, parametricWidth, wireType) {}

    };

    class InPort : public Port {
    public:
        InPort(std::string name, int width = 1,
               WireType wireType = WireType::Auto)
               : Port(name, Direction::In, width, wireType) {}
        InPort(std::string name, std::string parametricWidth,
               WireType wireType = WireType::Auto)
               : Port(name, Direction::In, parametricWidth, wireType) {}
    };
}
