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
#pragma once
#include <HWGenTools.hh>
#include <LHSValue.hh>
#include <Generatable.hh>
#include <string>

namespace HDLGenerator {
    /**
     * Unsigned extend.
     */
    class Ext : public LHSValue {
    public:
        Ext(std::string name, int extWidth, int signalWidth) {
            readList_.insert(name);
            if (extWidth > signalWidth) {
                write_rtl(name, std::to_string(extWidth),
                          std::to_string(signalWidth));
            } else {
                vhdl_ = name;
                verilog_ = name;
            }
        }
        Ext(std::string name, std::string extWidth, int signalWidth) {
            readList_.insert(name);
            write_rtl(name,  extWidth, std::to_string(signalWidth));
        }

        void write_rtl(std::string name, std::string extWidth,
                       std::string signalWidth) {
            vhdl_ = "((" + extWidth + "-1 downto " + signalWidth
                    + " => '0') & " + name + ")";
            verilog_ = "{{" + extWidth + "-" + signalWidth + "{"
                       + "1'b0}}, " + name + "}";
        }
    };

    /**
     * Splice of a vector.
     */
    class Splice : public LHSValue {
    public:
        Splice(std::string name, int upperBound, int lowerBound) {
            readList_.insert(name);
            vhdl_ = name + "(" + std::to_string(upperBound)
                    + " downto " + std::to_string(lowerBound) + ")";
            verilog_ = name + "[" + std::to_string(upperBound)
                       + ":" + std::to_string(lowerBound) + "]";
        }
    };

    /**
     * Signed extend.
     */
    class Sext : public LHSValue {
    public:
        Sext(std::string name, int extWidth, int signalWidth) {
            readList_.insert(name);
            if (extWidth > signalWidth) {
                write_rtl(name, std::to_string(extWidth), signalWidth);
            } else {
                vhdl_ = name;
                verilog_ = name;
            }
        }

        Sext(std::string name, std::string extWidth, int signalWidth) {
             readList_.insert(name);
             write_rtl(name,  extWidth, signalWidth);
        }

        void write_rtl(std::string name, std::string extWidth,
                       int signalWidth) {
            std::string sigWidth = std::to_string(signalWidth);
            vhdl_ = "((" + extWidth + "-1 downto " + sigWidth + " => " + name;
            verilog_ = "{{" + extWidth + "{" + name + "}";
            if (signalWidth > 1) {
                vhdl_ += "(" + sigWidth + "-1)";
                verilog_ += "[" + sigWidth + "-1]";
            }
            vhdl_ += ") & " + name + ")";
            verilog_ += "}}";
        }
    };
}
