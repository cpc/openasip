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
* @file HDLGenerator.hh
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/
#pragma once
#include "IPXact.hh"
#include "HWGenTools.hh"
#include "LHSValue.hh"
#include "HDLRegister.hh"
#include "Generatable.hh"
#include "HDLPort.hh"
#include "StringTools.hh"
#include "LicenseGenerator.hh"
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
#include <type_traits>

namespace HDLGenerator {

    /** @page hdl_generator HDLGenerator
     *
     *  <pre>
     *  Basic idea of HDLGenerator is to construct objects (modules, ports,
     *  behavioral code constructs, etc), assign them to the object wich they
     *  logically belong to, and finally call implement for the module.
     *  Calling implement builds the code, does some limited validity checks,
     *  and spits either Verilog or VHDL.
     *
     *  Only a limited subset of code constructs are present as not all were
     *  relevant for efficient hdl.
     *
     *  Some options can be used to guide the generation:
     *  "asynchronous reset" - Self-explanatory.
     *  "synchronous reset" - Self-explanatory.
     *  "reset everything" - Without this only registers with
     *                       ResetOption::Mandatory will be assigned a value
     *                       in reset.
     *
     *  What can be assigned to:
     *    Module << Register / Wire / InPort / OutPort / Parameter / Option
     *    Module << (std::string comment) / BinaryConstant / IntegerConstant
     *    Module << Behaviour / Module
     *    Behaviour << Assign / Asynchronous / Synchronous
     *    Asynchronous / Synchronous << SequentialStatement / Variable
     *    If <<  Else / SequentialStatement
     *    Switch << Case / DefaultCase
     *    Case / DefaultCase << HDLOperation / Assign / If
     *
     *  Case clauses:
     *    Case / DefaultCase << BinaryLiteral / (int) / (std::string)
     *  </pre>
     *
     *
     *
     */

    class Module;
    class Behaviour;
    class Synchronous;
    class Asynchronous;
    class Option;
    class Assign;
    class Wire;
    class LogicVariable;
    class SignedVariable;
    class UnsignedVariable;
    class IntegerConstant;
    class CastLogicToUnsigned;
    class CastLogicToSigned;
    class BinaryConstant;
    class Parameter;
    class InPort;
    class OutPort;
    class If;
    class Else;
    class Switch;
    class Case;
    class DefaultCase;
    class Splice;
    class RawCodeLine;
    class HDLOperation;
    class CodeBlock;

    /**
     * Base class for concurrent statements and code blocks
     */
    class SequentialStatement : public Generatable {
    public:
        SequentialStatement(std::string name) : Generatable(name), altname(name) {}

        virtual void hdl(std::ostream& stream, Language lang, int level) {
            (void)stream;
            (void)lang;
            (void)level;
            std::string err =
                std::string("Attempted to generate virtual class");
            throw std::runtime_error(err);
        }
    private:
        std::string altname;
    };

    /**
     * Represents VHDL operation snippet.
     */
    class HDLOperation : public SequentialStatement {
    public:
        HDLOperation() : SequentialStatement("no-op") {}
        HDLOperation(std::string name, std::deque<std::string> impl,
                     Language lang)
            : SequentialStatement(name), impl_(std::move(impl)), lang_(lang) {}

        HDLOperation& operator<<(const std::string& rhs) {
            readList_.emplace_back(rhs);
            return *this;
        }

        HDLOperation& operator<<(const std::string&& rhs) {
            readList_.emplace_back(rhs);
            return *this;
        }

        void build() override {
            Generatable::build();
            for (auto&& r : readList_) {
                reads(r);
            }
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (lang != lang_)
                throw std::runtime_error(__PRETTY_FUNCTION__);

            for (auto&& line : impl_) {
                stream << StringTools::indent(level) <<  line << "\n";
            }
        }

    private:
        std::deque<std::string> impl_;
        std::vector<std::string> readList_;
        Language lang_;
    };


    /**
     * Raw code.
     */
    class RawCodeLine : public SequentialStatement {
    public:
        RawCodeLine(std::string vhdl, std::string verilog)
            : SequentialStatement("raw code line"),
              vhdl_(vhdl), verilog_(verilog) {}

        void hdl(std::ostream& stream, Language lang, int level) final {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) <<  vhdl_ << "\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level) <<  verilog_ << "\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        std::string vhdl_;
        std::string verilog_;
    };

    /**
     * Option.
     */
    class Option : public Generatable {
    public:
        Option(std::string name) : Generatable(name) {}
    };

    /**
     * Generic/parameter.
     */
    class Parameter : public Generatable {
    public:
        Parameter(std::string name, int value = -1)
            : Generatable(name), value_(value) {}
        Parameter(std::string name, std::string value)
            : Generatable(name), value_(-1), strValue_(value) {}

        Width width() final { return {name(), value_}; }
        std::string strValue() { return strValue_; }

        void declare(std::ostream& stream, Language lang, int level = 0) {
            stream << StringTools::indent(level);
            if (lang == Language::VHDL) {
                 stream << name() << " : integer";
            } else if (lang == Language::Verilog) {
                stream << "parameter integer " << name();
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        int value_;
        std::string strValue_;
    };

    /**
     * Binary constant.
     */
    class BinaryConstant : public Generatable {
    public:
        BinaryConstant(std::string name, int width, int value)
            : Generatable(name), width_(width), value_(value) {}

        int value() const noexcept { return value_; }

        Width width() final { return {name(), width_}; }

        void declare(std::ostream& stream, Language lang, int level) {
            std::string binVal = "";
            int tempVal = value_;
            for (int i = width_ - 1; i >= 0; --i) {
                long power = static_cast<long>(std::pow(2, i));
                if (power <= tempVal) {
                    tempVal -= power;
                    binVal += "1";
                } else {
                    binVal += "0";
                }
            }

            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "constant " << name()
                       << " : std_logic_vector(" << width_ - 1
                       << " downto 0) := \"" << binVal << "\";\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level)
                       << "localparam [" << width_-1 << ":0] "
                       << name() << " = " << width_ << "'b" << binVal << ";\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        int width_;
        int value_;
    };

    /**
     * Integer constant.
     */
    class IntegerConstant : public Generatable {
    public:
        IntegerConstant(std::string name, int value)
            : Generatable(name), value_(value) {}

        int value() const noexcept { return value_; }

        Width width() final { return {name(), value_}; }

        void declare(std::ostream& stream, Language lang, int level) {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "constant " << name()
                       << " : integer := " << value() << ";\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level) << "localparam integer "
                       << name() << " = " << value() << ";\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        int value_;
    };

    /**
     * Async signal/wire.
     */
    class Wire : public Generatable {
    public:
        Wire(std::string name, int width = 1, WireType wt = WireType::Auto)
            : Generatable(name), width_(width), wt_(wt) {}
        Wire(std::string name, std::string width)
            : Generatable(name), strWidth_(width), width_(-1) {}

        Width width() final { return {strWidth_, width_}; }

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

        WireType wireType() const final { return wt_; }

    private:
        std::string strWidth_;
        int width_;
        WireType wt_;
    };

    /**
     * Assign default value.
     */
    class DefaultAssign : public SequentialStatement {
    public:
        DefaultAssign(std::string name, std::string value)
            : SequentialStatement(name), value_(value), dontCare_(false) {}
        DefaultAssign(std::string name)
            : SequentialStatement(name), value_(""), dontCare_(true) {}

        void hdl(std::ostream& stream, Language lang, int level) override {
            Width w = width(name());
            WireType t = wireType(name());


            if (lang == Language::VHDL) {
                std::string def = "-";
                if (!dontCare_)
                    def = value_.substr(0, 1);

                std::string assign = " <= ";
                if (isVariable(name())) {
                    assign = " := ";
                }
                stream << StringTools::indent(level) << name() << assign;

                if (w.strWidth.empty() && w.intWidth == 1) {
                    char delim = t == WireType::Auto ? '\'' : '"';
                    stream << delim << def << delim << ";\n";
                } else {
                    stream << "(others => '" << def << "');\n";
                }
            } else if (lang == Language::Verilog) {
                std::string def = "x";
                if (!dontCare_)
                    def = value_.substr(0, 1);

                stream << StringTools::indent(level)
                       << name() << " = 'b" << def << ";\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        std::string value_;
        bool dontCare_;
    };


    class Variable : public Generatable {
    public:
        Variable(std::string name, int width = 1)
            : Generatable(name), strWidth_(std::to_string(width)),
              width_(width), wireType_(WireType::Auto) {}
        Variable(std::string name, std::string width)
            : Generatable(name), strWidth_(width), width_(-1),
              wireType_(WireType::Vector) {}

        Width width() final { return {strWidth_, width_}; }

        void declare(std::ostream& stream, Language lang, int level) {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "variable " << name()
                       << " : " << vhdlTypeDeclaration() << ";\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level)
                       << verilogTypeDeclaration()
                       << " " << name() << ";\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        std::string vhdlRange() {
            std::string width = strWidth_;
            if (width.empty()) {
                width = std::to_string(width_);
            }
            return "(" + width + "-1 downto 0)";
        }

        std::string verilogRange() {
            std::string width = strWidth_;
            if (width.empty()) {
                width = std::to_string(width_);
            }
            return "[" + width + "-1:0] ";
        }

        virtual std::string vhdlTypeDeclaration() {
            throw std::runtime_error(__PRETTY_FUNCTION__);
        }

        std::string verilogTypeDeclaration() {
            std::string decl = "reg";
            if (isVector()) {
                decl += " " + verilogRange();
            }
            return decl;
        }

        bool isVector() {
            return wireType_ == WireType::Vector || width_ > 1;
        }

        WireType wireType() {
            return wireType_;
        }

    private:
        std::string strWidth_;
        int width_;
        WireType wireType_;
    };

    /**
     * Variable/async reg.
     */
    class LogicVariable : public Variable {
    public:
        LogicVariable(std::string name, int width = 1)
            : Variable(name, width) {}
        LogicVariable(std::string name, std::string width)
            : Variable(name, width) {}

        std::string vhdlTypeDeclaration() {
            if (isVector()) {
                return "std_logic_vector" + vhdlRange();
            } else {
                return "std_logic";
            }
        }
    };

    /**
     * Unsigned variable/async reg.
     */
    class UnsignedVariable : public Variable {
    public:
        UnsignedVariable(std::string name, int width = 1)
            : Variable(name, width) {}
        UnsignedVariable(std::string name, std::string width)
            : Variable(name, width) {}

        std::string vhdlTypeDeclaration() {
            return "unsigned" + vhdlRange();
        }
    };

    /**
     * Signed variable/async reg signed.
     */
    class SignedVariable : public Variable {
    public:
        SignedVariable(std::string name, int width = 1)
            : Variable(name, width) {}
        SignedVariable(std::string name, std::string width)
            : Variable(name, width) {}

        std::string vhdlTypeDeclaration() {
            return "signed" + vhdlRange();
        }

        std::string verilogTypeDeclaration() {
            std::string decl = "reg signed";
            if (isVector()) {
                decl += " " + verilogRange();
            }
            return decl;
        }
    };

    /**
     * Assignment.
     */
    class Assign : public SequentialStatement {
    public:
        Assign(std::string var, LHSValue value)
            : SequentialStatement(var), index_(-1), upperBound_(-1),
              lowerBound_(-1), value_(value) {}
        Assign(std::string var, LHSValue value, int idx)
            : SequentialStatement(var), index_(idx), upperBound_(-1),
              lowerBound_(-1), value_(value) {}
        Assign(std::string var, LHSValue value, int ub, int lb)
            : SequentialStatement(var), index_(-1), upperBound_(ub),
              lowerBound_(lb), value_(value) {}

        void build() override {
            Generatable::build();
            writes(name());
            reads(value_);
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (isRegister(name()) && !parentIs<Synchronous>()) {
                throw std::runtime_error(
                    "assigning to register '" + name() +
                    "' is only allowed in synchronous context");
            } else if (isVariable(name()) && (!parentIs<Synchronous>() ||
                                                 !parentIs<Asynchronous>())) {
                throw std::runtime_error("Not allowed to assign to '" +
                                         name() + "' in this context.");
            }
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << name();
                if (upperBound_ >= 0) {
                    stream << "(" << upperBound_ << " downto "
                                  << lowerBound_ << ")";
                } else if (index_ >= 0) {
                    stream << "(" << index_ << ")";
                }
                if (isVariable(name())) {
                    stream << " := ";
                } else {
                    stream << " <= ";
                }
            } else if (lang == Language::Verilog) {
                if (!(parentIs<Synchronous>() || parentIs<Asynchronous>())) {
                    stream << StringTools::indent(level) << "always @*\n"
                           << StringTools::indent(level + 1) << name();
                } else {
                    stream << StringTools::indent(level) << name();
                }
                if (upperBound_ >= 0) {
                    stream << "[" << upperBound_ << ":" << lowerBound_ << "]";
                } else if (index_ >= 0) {
                    stream << "[" << index_ << "]";
                }
                if (isRegister(name())) {
                    stream << " <= ";
                } else {
                    stream << " = ";
                }
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
            value_.hdl(stream, lang);
            stream << ";\n";
        }

    private:
        int index_;
        int upperBound_;
        int lowerBound_;
        LHSValue value_;
    };

    /**
     * When others/default.
     */
    class DefaultCase : public Generatable {
    public:
        DefaultCase() : Generatable("defaultCase") {}

        DefaultCase& operator<<(DefaultAssign&& rhs) {
            addComponent(rhs);
            return *this;
        }

        DefaultCase& operator<<(DefaultAssign& rhs) {
            addComponent(rhs);
            return *this;
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "when others =>\n";
                implementAll(stream, lang, level);
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level) << "default: begin\n";
                implementAll(stream, lang, level);
                stream << StringTools::indent(level) << "end\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }
    };

    /**
     * Case in a switch-case.
     */
    class Case : public Generatable {
    public:
        Case() : Generatable("case") {}
        Case(std::string stringCase) : Generatable("case"),
                                       stringCase_(stringCase) {}

        template<typename SS>
        Case& operator<<(SS op) {
            std::shared_ptr<SequentialStatement> ptr = std::make_shared<SS>(op);
            pushComponent(ptr);
            return *this;
        }

        Case& operator<<(BinaryLiteral&& rhs) {
            binaryCases_.emplace_back(rhs);
            return *this;
        }

        Case& operator<<(BinaryLiteral& rhs) {
            binaryCases_.emplace_back(rhs);
            return *this;
        }

        Case& operator<<(int rhs) {
            intCases_.emplace_back(rhs);
            return *this;
        }

        Case& operator<<(std::string& rhs) {
            stringCase_ = rhs;
            return *this;
        }

        Case& operator<<(std::string&& rhs) {
            stringCase_ = rhs;
            return *this;
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "when ";
                if (!stringCase_.empty()) {
                    stream << stringCase_;
                } else if (!intCases_.empty()) {
                    std::string separator = "";
                    for (auto&& c : intCases_) {
                        stream << separator << std::to_string(c);
                        separator = " | ";
                    }
                } else if (!binaryCases_.empty()) {
                    std::string separator = "";
                    for (auto&& c : binaryCases_) {
                        stream << separator;
                        c.hdl(stream, lang);
                        separator = " | ";
                    }
                } else {
                    throw std::runtime_error("Case has no case");
                }
                stream << " =>\n";
                implementAll(stream, lang, level + 1);
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level);
                if (!stringCase_.empty()) {
                    stream << stringCase_;
                } else if (!intCases_.empty()) {
                    std::string separator = "";
                    for (auto&& c : intCases_) {
                        stream << separator << std::to_string(c);
                        separator = ", ";
                    }
                } else if (!binaryCases_.empty()) {
                    std::string separator = "";
                    for (auto&& c : binaryCases_) {
                        stream << separator;
                        c.hdl(stream, lang);
                        separator = ", ";
                    }
                } else {
                    throw std::runtime_error("Case has no case");
                }
                stream << ": begin\n";
                implementAll(stream, lang, level + 1);
                stream << StringTools::indent(level) << "end\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        std::string stringCase_;
        std::vector<int> intCases_;
        std::vector<BinaryLiteral> binaryCases_;
    };

    /**
     * Switch-case switch.
     */
    class Switch : public SequentialStatement {
    public:
        Switch(LHSValue control) : SequentialStatement("switch"),
                                   control_(control) {}

        void addCase(Case rhs) {
            addComponent(rhs);
        }

        void addCase(DefaultCase rhs) {
            addComponent(rhs);
        }

        void build() override {
            Generatable::build();
            reads(control_);
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "case ";
                control_.hdl(stream, lang);
                stream << " is\n";
                implementAll(stream, lang, level + 1);
                stream << StringTools::indent(level) << "end case;\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level) << "case (";
                control_.hdl(stream, lang);
                stream << ")\n";
                implementAll(stream, lang, level + 1);
                stream << StringTools::indent(level) << "endcase\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        LHSValue control_;
    };

    /**
     * if/elsif/else.
     */
    class If : public SequentialStatement {
    public:
        template<typename SS>
        If(LHSValue cls, SS ifBlock) : SequentialStatement("if"),
            elseBlock_(nullptr) {
            std::shared_ptr<SequentialStatement> ptr
                = std::make_shared<SS>(ifBlock);
            ifBlocks_.emplace_back(cls, ptr);
        }

        template<typename SS>
        void elseIfClause(LHSValue cls, SS ifBlock) {
            std::shared_ptr<SequentialStatement> ptr
                = std::make_shared<SS>(ifBlock);
            ifBlocks_.emplace_back(cls, ptr);
        }

        template<typename SS>
        void elseClause(SS elseBlock) {
            if (elseBlock_) {
                throw std::runtime_error("Cannot to add a second else block.");
            }
            std::shared_ptr<SequentialStatement> ptr
                = std::make_shared<SS>(elseBlock);
            elseBlock_ = ptr;
        }

        virtual void hdl(std::ostream& stream, Language lang, int level)
                override {
            if (lang == Language::VHDL) {
                for (auto iter = ifBlocks_.begin(); iter != ifBlocks_.end();
                     ++iter) {
                    if (iter == ifBlocks_.begin()) {
                        stream << StringTools::indent(level) << "if ";
                    } else {
                        stream << StringTools::indent(level) << "elsif ";
                    }
                    iter->first.hdl(stream, lang);
                    stream << " then\n";
                    iter->second->hdl(stream, lang, level + 1);
                }
                if (elseBlock_ != nullptr) {
                    stream << StringTools::indent(level) << "else\n";
                    elseBlock_->hdl(stream, lang, level + 1);
                }
                stream << StringTools::indent(level) << "end if;\n";
            } else if (lang == Language::Verilog) {
                for (auto iter = ifBlocks_.begin(); iter != ifBlocks_.end();
                     ++iter) {
                    if (iter == ifBlocks_.begin()) {
                        stream << StringTools::indent(level) << "if ";
                    } else {
                        stream << StringTools::indent(level) << "end else if ";
                    }
                    iter->first.hdl(stream, lang);
                    stream << " begin\n";
                    iter->second->hdl(stream, lang, level + 1);
                }
                if (elseBlock_ != nullptr) {
                    stream << StringTools::indent(level) << "end else begin\n";
                    elseBlock_->hdl(stream, lang, level + 1);
                }
                stream << StringTools::indent(level) << "end\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        void build() override {
            Generatable::build();
            for (auto&& block : ifBlocks_) {
                reads(block.first);
                block.second->setParent(this);
                block.second->build();
            }
            if (elseBlock_ != nullptr) {
                elseBlock_->setParent(this);
                elseBlock_->build();
            }
        }

    private:
        std::vector<std::pair<LHSValue,std::shared_ptr<SequentialStatement> > >
            ifBlocks_;
        std::shared_ptr<SequentialStatement> elseBlock_;
    };

    /**
     * Code block, i.e., a collection of concurrent statements
     */
    class CodeBlock : public SequentialStatement {
    public:
        CodeBlock() : SequentialStatement("codeBlock") {}

        template<typename SS>
        void append(SS cc) {
            std::shared_ptr<SequentialStatement> ptr = std::make_shared<SS>(cc);
            pushComponent(ptr);
        }

        void hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                implementAll(stream, lang, level);
            } else if (lang == Language::Verilog) {
                implementAll(stream, lang, level);
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }
    };

    /**
     * Async process/always.
     */
    class Asynchronous : public Generatable {
    public:

        Asynchronous(const std::string& name) : Generatable(name) {}

        template<typename SS>
        Asynchronous& operator<<(SS op) {
            std::shared_ptr<SequentialStatement> ptr = std::make_shared<SS>(op);
            pushComponent(ptr);
            return *this;
        }

        template<typename Var>
        void addVariable(Var op) {
            std::shared_ptr<Variable> ptr = std::make_shared<Var>(op);
            variables_.push_back(ptr);
        }

        virtual void reads(const std::string& var) override {
            readList_.emplace(var);
            if (parent() != nullptr) {
                parent()->reads(var);
            }
        }

        virtual void build() override; // implemented @EOF

        virtual void
        hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                stream << "\n";
                stream << StringTools::indent(level) << name() << " : process(";
                std::string separator = "";
                for (auto&& r : readList_) {
                    if (!isConstant(r)) {
                        stream << separator << r;
                        separator = ", ";
                    }
                }
                stream << ")\n";
                for (auto&& v : variables_) {
                    v->declare(stream, lang, level + 1);
                }
                stream << StringTools::indent(level) << "begin\n";
                implementAll(stream, lang, level + 1);
                stream << StringTools::indent(level) << "end process "
                       << name() << ";\n";
            } else if (lang == Language::Verilog) {
                stream << "\n";
                stream << StringTools::indent(level) << "// " << name() << "\n";
                stream << StringTools::indent(level) << "always @*";
                /**
                 * @lassetodo Sensitivity list implementation here
                 * for verilog if needed.
                 * <pre>
                 * If sensitivity list is needed for verilog replace
                 * @* with @(, uncomment following code, and add ) before
                 * begin
                 *
                 * std::string separator = "";
                 * for (auto&& r : readList_) {
                 *   stream << separator << r;
                 *   separator = " or ";
                 * }
                 * </pre>
                 */
                stream << " begin\n";
                implementAll(stream, lang, level + 1);
                stream << StringTools::indent(level) << "end\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        std::vector< std::shared_ptr<Variable> > variables_;
        std::unordered_set<std::string> readList_;
    };

    /**
     * Sync process/always.
     */
    class Synchronous : public Generatable {
    public:
        Synchronous(std::string name) : Generatable(name) {}

        template<typename SS>
        Synchronous& operator<<(SS op) {
            std::shared_ptr<SequentialStatement> ptr = std::make_shared<SS>(op);
            pushComponent(ptr);
            return *this;
        }

        template<typename Var>
        void addVariable(Var op) {
            std::shared_ptr<Variable> ptr = std::make_shared<Var>(op);
            variables_.push_back(op);
        }

        virtual void build() override; // implemented @EOF

        virtual void writes(const std::string& var) override {
            //Generatable::writes(var);
            if (isVariable(var)) {
                return;
            }
            if (!isRegister(var)) {
                std::cerr << "Trying to write nonregister " << var << "\n";
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
            registers_.emplace(var);
        }

        virtual void vhdlReset(std::ostream& stream, Language lang, int level) {
            if (hasOption("active low reset"))
                stream << StringTools::indent(level) << "if rstx = '0' then\n";
            if (hasOption("active high reset"))
                stream << StringTools::indent(level) << "if rst = '1' then\n";
            for (auto&& r : registers_) {
                Register& reg = getRegister(r);
                if (reg.resetOption() == ResetOption::Mandatory ||
                    hasOption("reset everything")) {
                    reg.reset(stream, lang, level + 1);
                }
            }
            // Leaves if clause open for else/elsif
        }

        virtual void
        hdl(std::ostream& stream, Language lang, int level) override {
            if (lang == Language::VHDL) {
                stream << "\n"
                       << StringTools::indent(level) << name() << " : process";
                if (hasOption("asynchronous reset")) {
                    stream << "(clk, rstx)\n";
                } else {
                    stream << "(clk)\n";
                }

                for (auto&& v : variables_) {
                    v->declare(stream, lang, level + 1);
                }

                stream << StringTools::indent(level) << "begin\n";
                if (hasOption("asynchronous reset")) {
                    vhdlReset(stream, lang, level + 1);
                    stream << StringTools::indent(level + 1)
                           << "elsif clk = '1' and clk'event then\n";
                    implementAll(stream, lang, level + 2);
                } else {
                    stream << StringTools::indent(level + 1)
                           << "if clk = '1' and clk'event then\n";
                    vhdlReset(stream, lang, level + 2);
                    stream << StringTools::indent(level + 2) << "else\n";
                    implementAll(stream, lang, level + 3);
                    stream << StringTools::indent(level + 2) << "end if;\n";
                }
                stream << StringTools::indent(level + 1) << "end if;\n";
                stream << StringTools::indent(level)
                       << "end process " << name() << ";\n";
            } else if (lang == Language::Verilog) {
                stream << "\n";
                stream << StringTools::indent(level) << "// " << name() << "\n";
                if (hasOption("asynchronous reset")) {
                    stream << StringTools::indent(level)
                           << "always @(posedge clk or negedge rstx) begin\n";
                } else {
                    stream << StringTools::indent(level)
                           << "always @(posedge clk) begin\n";
                }
                stream << StringTools::indent(level + 1)
                       << "if (~rstx) begin\n";
                for (auto&& r : registers_) {
                    Register& reg = getRegister(r);
                    if (reg.resetOption() == ResetOption::Mandatory ||
                        hasOption("reset everything")) {
                        reg.reset(stream, lang, level + 2);
                    }
                }
                stream << StringTools::indent(level + 1) << "end else begin\n";
                implementAll(stream, lang, level + 2);
                stream << StringTools::indent(level + 1) << "end\n";
                stream << StringTools::indent(level) << "end\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

    private:
        std::vector< std::shared_ptr<Variable> > variables_;
        std::unordered_set<std::string> registers_;
    };

    /**
     * Force newline to hdl.
     */
    class NewLine : public Generatable {
    public:
        NewLine() : Generatable("") {}

        void hdl(std::ostream& stream, Language lang, int level) final {
            (void)lang;
            (void)level;
            stream << "\n";
        }
    };

    /**
     * Wrapper for behavioral code constructs.
     */
    class Behaviour : public Generatable {
    public:
        Behaviour() : Generatable("behaviour") {}
        virtual ~Behaviour() = default;

        Behaviour& operator<<(Synchronous& rhs) {
            addComponent(std::forward<Synchronous>(rhs));
            return *this;
        }

        Behaviour& operator<<(Asynchronous& rhs) {
            addComponent(std::forward<Asynchronous>(rhs));
            return *this;
        }

        Behaviour& operator<<(Synchronous&& rhs) {
            addComponent(rhs);
            return *this;
        }

        Behaviour& operator<<(NewLine& rhs) {
            addComponent(rhs);
            return *this;
        }

        Behaviour& operator<<(NewLine&& rhs) {
            addComponent(rhs);
            return *this;
        }

        Behaviour& operator<<(RawCodeLine&& rhs) {
            addComponent(rhs);
            return *this;
        }

        Behaviour& operator<<(Assign& assignment) {
            addComponent(assignment);
            return *this;
        }

        Behaviour& operator<<(Assign&& assignment) {
            addComponent(assignment);
            return *this;
        }

        void behaviour(std::ostream& stream, Language lang,
                       int level) {
            forAll([&](std::shared_ptr<Generatable> c) {
                c->hdl(stream, lang, level);
            });
        }

    private:
    };

    /**
     * Entity/module.
     */
    class Module : public Generatable {
    public:
        Module(std::string name) : Generatable(name), prefix_(name) {
            set_header();
        }

        Module(ipxact::ModuleInfo info, int id) : Generatable(info.name),
            prefix_(info.name) {
            set_header();
            id_ = id;
            for (auto&& p : info.parameters) {
                parameters_.emplace_back(Parameter{p.name, p.value});
            }
            for (auto&& p : info.ports) {
                Direction dir = p.direction == "in" ?
                                Direction::In : Direction::Out;
                if (p.vector) {
                    *this << Port(p.name, dir, p.left + "+1");
                } else {
                    *this << Port(p.name, dir);
                }
            }
        }

        void set_header() {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            char buffer[31];
            std::stringstream ss;
            TCEString licenseStr = LicenseGenerator::generateMITLicense(
                "2023", "");
            std::vector<TCEString> licenseStrs = licenseStr.split("\n");
            for (unsigned int i = 0; i < licenseStrs.size(); i++) {
                headerComment_.emplace_back(licenseStrs.at(i));
            }
            std::strftime(buffer, 30, "%c", std::localtime(&now_c));
            ss << buffer; // std::put_time(std::localtime(&now_c), "%c");
            headerComment_.emplace_back("");
            headerComment_.emplace_back(
                std::string("Generated on ") + ss.str());
            headerComment_.emplace_back("");
        }

        void set_prefix(std::string prefix) {
            prefix_ = prefix;
        }

        Module& operator<<(RawCodeLine&& rawCodeLine) {
            rawCodeLines_.emplace_back(rawCodeLine);
            return *this;
        }

        Module& operator<<(Behaviour& rhs) {
            behaviours_.emplace_back(new Behaviour(rhs));
            return *this;
        }

        Module& operator<<(Behaviour&& rhs) {
            behaviours_.emplace_back(new Behaviour(rhs));
            return *this;
        }

        Module& operator<<(Port&& port) {
            ports_.push_back(port);
            return *this;
        }

        Module& operator<<(Parameter&& param) {
            parameters_.emplace_back(param);
            return *this;
        }

        Module& operator<<(IntegerConstant&& constant) {
            constants_.emplace_back(constant);
            return *this;
        }

        Module& operator<<(BinaryConstant&& constant) {
            binaryConstants_.emplace_back(constant);
            return *this;
        }

        Module& operator<<(Wire&& wire) {
            wires_.emplace_back(new Wire(wire));
            return *this;
        }

        Module& operator<<(Register&& reg) {
            registers_.emplace_back(reg);
            return *this;
        }

        Module& operator<<(Register& reg) {
            registers_.emplace_back(reg);
            return *this;
        }

        Module& operator<<(Option&& opt) {
            options_.emplace(opt.name());
            return *this;
        }

        Module& operator<<(Module&& rhs) {
            modules_.emplace_back(rhs);
            return *this;
        }

        Module& operator<<(Module& rhs) {
            modules_.emplace_back(rhs);
            return *this;
        }

        void build() final {
            for (auto&& b : behaviours_) {
                b->setParent(this);
                b->build();
            }
        }

        void appendToHeader(const std::string& line) {
            headerComment_.emplace_back(line);
        }

        virtual bool isRegister(const std::string& name) final {
            for (auto&& r : registers_) {
                if (r.name() == name) {
                    return true;
                }
            }
            return false;
        }

        virtual bool isConstant(const std::string& name) final {
            for (auto&& r : constants_) {
                if (r.name() == name) {
                    return true;
                }
            }
            for (auto&& r : binaryConstants_) {
                if (r.name() == name) {
                    return true;
                }
            }
            return false;
        }

        virtual bool isVariable(const std::string& name) final {
            for (auto&& v : variables_) {
                if (v->name() == name) {
                    return true;
                }
            }
            return false;
        }

        void registerVariable(const std::shared_ptr<Variable> var) {
            for (auto&& v : variables_) {
                if (v->name() == var->name()) {
                    throw std::runtime_error("tried to register variable " +
                                             var->name() + " multiple times");
                }
            }
            variables_.push_back(var);
        }

        Width width(const std::string& name) final {
            for (auto&& v : parameters_) {
                if (v.name() == name) {
                    return v.width();
                }
            }
            for (auto&& v : constants_) {
                if (v.name() == name) {
                    return v.width();
                }
            }
            for (auto&& v : registers_) {
                if (v.name() == name) {
                    return v.width();
                }
            }
            for (auto&& v : wires_) {
                if (v->name() == name) {
                    return v->width();
                }
            }
            for (auto&& v : variables_) {
                if (v->name() == name) {
                    return v->width();
                }
            }
            for (auto&& v : ports_) {
                if (v.name() == name) {
                    return v.width();
                }
            }
            // not finding a thing is an error.
            throw std::runtime_error("Couldn't find width for " + name);
        }

        WireType wireType(const std::string& name) final {
            for (auto&& v : ports_) {
                if (v.name() == name) {
                    return v.wireType();
                }
            }
            for (auto&& v : wires_) {
                if (v->name() == name) {
                    return v->wireType();
                }
            }
            for (auto&& v : variables_) {
                if (v->name() == name) {
                    return v->wireType();
                }
            }
            // not finding a thing is an error.
            throw std::runtime_error("Couldn't find wire type for " + name);
        }

        void reads(const std::string& var) final { (void)var; }

        void writes(const std::string& var) final { (void)var; }

        void declare(std::ostream& stream, Language lang, int level) {
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level) << "component "
                       << name() << " is\n";
                // - Generics
                if (!parameters_.empty()) {
                    std::string separator = "";
                    stream << StringTools::indent(level + 1) << "generic (\n";
                    for (auto&& parameter : parameters_) {
                        stream << separator << StringTools::indent(level + 2);
                        parameter.declare(stream, lang);
                        separator = ";\n";
                    }
                    stream << ");\n";
                }
                // - Ports
                if (!ports_.empty()) {
                    std::string separator = "";
                    stream << StringTools::indent(level + 1) << "port (\n";
                    for (auto&& port : ports_) {
                        stream << separator << StringTools::indent(level + 2);
                        port.declare(stream, lang);
                        separator = ";\n";
                    }
                    stream << ");\n";
                }
                stream << StringTools::indent(level) << "end component "
                       << name() << ";\n";
            } else if (lang == Language::Verilog) {
                // no declaration for verilog.
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        void instantiate(std::ostream& stream, Language lang, int level) {
            std::string instance = prefix_ + "_" + std::to_string(id_);
            if (lang == Language::VHDL) {
                stream << StringTools::indent(level)
                       << instance << " : " << name() << "\n";
                if (parameters_.size() > 0) {
                    stream << StringTools::indent(level + 1) << "generic map (\n";

                    std::string separator = "";
                    for (auto&& p : parameters_) {
                        stream << separator
                               << StringTools::indent(level + 2)
                               << p.name() << " => " << p.strValue();
                        separator = ",\n";
                    }
                    stream << ")\n";
                }
                std::string separator = "";
                stream << StringTools::indent(level + 1) << "port map (\n";
                for (auto&& p : ports_) {
                    stream << separator << StringTools::indent(level + 2)
                           << p.name() << " => ";
                    if (p.name() == "clk" || p.name() == "rstx" ||
                        p.name() == "glock_in") {
                        stream << p.name();
                    } else {
                        stream << instance << "_" <<  p.name();
                    }
                    separator = ",\n";
                }
                stream << ");\n";
            } else if (lang == Language::Verilog) {
                stream << StringTools::indent(level) << name() << " ";
                if (parameters_.size() > 0) {
                    stream << "#(";
                    std::string separator = "";
                    for (auto&& p : parameters_) {
                        stream << separator;
                        stream << "." << p.name() << "(" << p.strValue() << ")";
                        separator = ", ";
                    }
                    stream << ") ";
                }
                std::string separator = "";
                stream << instance << " (\n";
                for (auto&& p : ports_) {
                    stream << separator;
                    stream << StringTools::indent(level + 2) << "." << p.name()
                           << "(";
                    if (p.name() == "clk" || p.name() == "rstx" ||
                        p.name() == "glock_in") {
                        stream << p.name() << ")";
                    } else {
                        stream << instance << "_" << name() << ")";
                    }
                    separator = ",\n";
                }
                stream << ");\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        void implement(std::ostream& stream, Language lang, int level = 0) {
            clear();
            build();
            if (lang == Language::VHDL) {
                std::string ident = StringTools::indent(level);
                // Header comment
                for (auto&& line : headerComment_) {
                    stream << ident <<  "-- " << line << "\n";
                }
                // Libraries
                stream << ident << "\n"
                       << ident << "library ieee;\n"
                       << ident << "use ieee.std_logic_1164.all;\n"
                       << ident << "use ieee.numeric_std.all;\n"
                       << ident << "use ieee.std_logic_misc.all;\n"
                       << ident << "use STD.textio.all;\n"
                       << ident << "use ieee.std_logic_textio.all;\n"
                       << ident << "use IEEE.math_real.all;\n"
                // Entity
                       << ident << "\n"
                       << ident << "entity " << name() << " is\n";
                // - Generics
                if (!parameters_.empty()) {
                    std::string separator = "";
                    stream << StringTools::indent(level + 1) << "generic (\n";
                    for (auto&& parameter : parameters_) {
                        stream << separator;
                        parameter.declare(stream, lang, level + 2);
                        separator = ";\n";
                    }
                    stream << ");\n";
                }
                // - Ports
                if (!ports_.empty()) {
                    std::string separator = "";
                    stream << StringTools::indent(level + 1) <<  "port (\n";
                    for (auto&& port : ports_) {
                        stream << separator;
                        port.declare(stream, lang, level + 2);
                        separator = ";\n";
                    }
                    stream << ");\n";
                }
                stream << ident << "end entity " << name() << ";\n"
                // Architecture
                       << ident << "\n"
                       << ident << "architecture rtl of "
                                        << name() << " is\n";
                // constants
                if (!constants_.empty() || !binaryConstants_.empty()) {
                    stream << "\n";
                }
                for (auto&& c : constants_) {
                    c.declare(stream, lang, level + 1);
                }
                for (auto&& c : binaryConstants_) {
                    c.declare(stream, lang, level + 1);
                }
                // wires
                if (!wires_.empty()) {
                    stream << "\n";
                }
                for (auto&& w : wires_) {
                    w->declare(stream, lang, level + 1);
                }
                // registers
                if (!registers_.empty()) {
                    stream << "\n";
                }
                for (auto&& r : registers_) {
                    r.declare(stream, lang, level + 1);
                }

                // Raw code lines
                for (auto&& r : rawCodeLines_) {
                    r.hdl(stream, lang, level + 1);
                }

                // declare components
                std::vector<std::string> declared;
                for (auto&& m : modules_) {
                    if (std::find(declared.begin(), declared.end(),
                            m.name()) != declared.end()) {
                        continue;
                    }
                    stream << "\n";
                    m.declare(stream, lang, level + 1);
                    declared.emplace_back(m.name());
                }
                stream << StringTools::indent(level) << "\nbegin\n\n";
                // instantiate components
                for (auto&& m : modules_) {
                    m.instantiate(stream, lang, level + 1);
                    stream << "\n";
                }
                // actual behavioural code
                for (auto&& b : behaviours_) {
                    b->behaviour(stream, lang, level + 1);
                }
                stream << "\n";
                stream << StringTools::indent(level)
                       << "end architecture rtl;\n\n";

            } else if (lang == Language::Verilog) {
                // Header comment
                stream << StringTools::indent(level) << "/*\n";
                for (auto&& line : headerComment_) {
                    stream << StringTools::indent(level)
                           << " * " << line << "\n";
                }
                stream << StringTools::indent(level) << " */\n";
                // Module
                stream << StringTools::indent(level) << "\n";
                stream << StringTools::indent(level) << "module " << name();
                // - Parameters
                if (!parameters_.empty()) {
                    std::string separator = "";
                    stream << " #(\n";
                    for (auto&& parameter : parameters_) {
                        stream << separator;
                        parameter.declare(stream, lang, level + 2);
                        separator = ",\n";
                    }
                    stream << ")";
                }
                // - Ports
                if (!ports_.empty()) {
                    std::string separator = "";
                    stream << " (\n";
                    for (auto&& port : ports_) {
                        stream << separator;
                        port.declare(stream, lang, level + 2);
                        separator = ",\n";
                    }
                    stream << ")";
                }
                // end module interface
                stream << ";\n";
                // constants
                if (!constants_.empty() || !binaryConstants_.empty()) {
                    stream << "\n";
                }
                for (auto&& c : constants_) {
                    c.declare(stream, lang, level + 1);
                }
                for (auto&& c : binaryConstants_) {
                    c.declare(stream, lang, level + 1);
                }
                // wires
                if (!wires_.empty()) {
                    stream << "\n";
                }
                for (auto&& w : wires_) {
                    w->declare(stream, lang, level + 1);
                }
                // variables
                if (!variables_.empty()) {
                    stream << "\n";
                    for (auto&& v : variables_) {
                        v->declare(stream, lang, level + 1);
                    }
                }
                // registers
                if (!registers_.empty()) {
                    stream << "\n";
                }
                for (auto&& r : registers_) {
                    r.declare(stream, lang, level + 1);
                }

                // Raw code lines
                stream << "\n";
                for (auto&& r : rawCodeLines_) {
                    r.hdl(stream, lang, level + 1);
                }

                // instantiate stuff
                for (auto&& m : modules_) {
                    stream << "\n";
                    m.instantiate(stream, lang, level + 1);
                }
                // actual code
                stream << "\n";
                for (auto&& b : behaviours_) {
                    b->behaviour(stream, lang, level + 1);
                }
                // end module
                stream << "\n";
                stream << StringTools::indent(level) << "endmodule\n\n";
            } else {
                throw std::runtime_error(__PRETTY_FUNCTION__);
            }
        }

        bool hasOption(const std::string& var) final {
            return std::find(options_.begin(), options_.end(), var) !=
                   options_.end();
        }

        Register& getRegister(const std::string& var) final {
            for (auto&& r : registers_) {
                if (r.name() == var) {
                    return r;
                }
            }
            throw std::runtime_error("Couldn't find register '" + var + "'");
        }

    private:
        void clear() {
            // clear variables.
            variables_.clear();
            // Remove duplicate registers.
            registers_.erase(std::unique(registers_.begin(), registers_.end(),
                                 [](Register l, Register r) {
                                     return l.name() == r.name();
                                 }),
                registers_.end());
        }
        int id_ = 0;
        std::string prefix_;
        std::vector<RawCodeLine> rawCodeLines_;
        std::unordered_set<std::string> options_;
        std::vector<std::string> headerComment_;
        std::vector<Parameter> parameters_;
        std::vector<Port> ports_;
        std::vector<IntegerConstant> constants_;
        std::vector<BinaryConstant> binaryConstants_;
        std::vector<std::shared_ptr<Wire>> wires_;
        std::vector<Register> registers_;
        std::vector<std::shared_ptr<Variable> > variables_;
        std::vector<std::shared_ptr<Behaviour>> behaviours_;
        std::vector<Module> modules_;
    };
}
