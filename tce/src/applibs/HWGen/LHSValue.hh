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
#include "HWGenTools.hh"
#include <iostream>
#include <string>
#include <unordered_set>

namespace HDLGenerator {
    /**
     * Base class for left-hand side values (assignments, If clauses)
     */
    class LHSValue {
    public:
        LHSValue() {}
        void hdl(std::ostream& stream, Language lang, int level);
        void hdl(std::ostream& stream, Language lang);
        void writeSignals(std::unordered_set<std::string>& readList) const;
        std::string vhdl() const { return vhdl_; }
        std::string verilog() const { return verilog_; }

        // Operator overloading for more readable code
        LHSValue operator||(LHSValue rhs);
        LHSValue operator|(LHSValue rhs);
        LHSValue operator&&(LHSValue rhs);
        LHSValue operator&(LHSValue rhs);
        LHSValue operator^(LHSValue rhs);
        LHSValue operator!();
        LHSValue operator~();

    protected:
        std::unordered_set<std::string> readList_;
        std::string vhdl_;
        std::string verilog_;
    };

    /**
     * Base class for left-hand side values (assignments, If clauses)
     */
    class LHSSignal : public LHSValue {
    public:
        LHSSignal(std::string name);
    };

    class BinaryLiteral : public LHSValue {
    public:
        BinaryLiteral(std::string value);
        BinaryLiteral(int value, int width, bool signExtend);
        std::string name() { return value_; }
    private:
        std::string value_;
    };
}
