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
* @file BinaryOps.hh
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/
#pragma once
#include <LHSValue.hh>
#include <HWGenTools.hh>
#include <iostream>
#include <string>

namespace HDLGenerator {

    class BinaryOp : public LHSValue {
    public:
        BinaryOp(const LHSValue& lhs, const LHSValue& rhs,
                 std::string VHDLOperator,
                 std::string VerilogOperator) {
            vhdl_ = "(" + lhs.vhdl() + " " + VHDLOperator
                        + " " + rhs.vhdl() + ")";
            verilog_ = "(" + lhs.verilog() + " " + VerilogOperator + " "
                           + rhs.verilog() + ")";
            lhs.writeSignals(readList_);
            rhs.writeSignals(readList_);
        }
    };

    class LogicalAnd : public BinaryOp {
    public:
        LogicalAnd(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "and", "&") {}
    };

    class BitwiseAnd : public BinaryOp {
    public:
        BitwiseAnd(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "and", "&") {}
    };

    class LogicalOr : public BinaryOp {
    public:
        LogicalOr(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "or", "||") {}
    };

    class BitwiseOr : public BinaryOp {
    public:
        BitwiseOr(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "or", "|") {}
    };

    class BitwiseXor : public BinaryOp {
    public:
        BitwiseXor(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "xor", "^") {}
    };

    class Equals : public BinaryOp {
    public:
        Equals(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "=", "==") {}
    };

    class NotEquals : public BinaryOp {
    public:
        NotEquals(const LHSValue& lhs, const LHSValue& rhs) :
          BinaryOp(lhs, rhs, "/=", "!=") {}
    };

    class UnaryOp : public LHSValue {
    public:
        UnaryOp(LHSValue val, std::string VHDLOperator,
                 std::string vlogOperator) {
            vhdl_ = "(" + VHDLOperator + " " + val.vhdl() + ")";
            verilog_ = "(" + vlogOperator + " " + val.verilog() + ")";
            val.writeSignals(readList_);
        }
    };

    class LogicalNot : public UnaryOp {
    public:
        LogicalNot(LHSValue val) : UnaryOp(val, "not", "!") {}
    };

    class BitwiseNot : public UnaryOp {
    public:
        BitwiseNot(LHSValue val) : UnaryOp(val, "not", "~") {}
    };

    class Reduce : public LHSValue {
    public:
        Reduce(LHSValue val, std::string VHDLFunction,
                 std::string VerilogOperator) {
            vhdl_ = VHDLFunction + "(" + val.vhdl() + ")";
            verilog_ = "(" + VerilogOperator + " " + val.verilog() + ")";
            val.writeSignals(readList_);
        }
    };

    class OrReduce : public Reduce {
    public:
        OrReduce(LHSValue val) : Reduce(val, "or_reduce", "|") {}
    };

    class AndReduce : public Reduce {
    public:
        AndReduce(LHSValue val) : Reduce(val, "and_reduce", "&") {}
    };

    class XorReduce : public Reduce {
    public:
        XorReduce(LHSValue val) : Reduce(val, "xor_reduce", "^") {}
    };

}
