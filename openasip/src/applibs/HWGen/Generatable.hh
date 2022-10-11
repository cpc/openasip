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
* @file Generatable.hh
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/
#pragma once
#include <LHSValue.hh>
#include <HWGenTools.hh>
#include <HDLRegister.hh>
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
     * Base class for all generatable classes.
     */
    class Generatable {
    public:
        Generatable(std::string name) : name_(name), parent_(nullptr) {}
        virtual ~Generatable() = default;

        virtual void build() {
            forAll([this](std::shared_ptr<Generatable> c) {
                c->setParent(this);
                c->build();
            });
        }

        virtual void reads(const std::string& var) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name();
                throw std::runtime_error(err.c_str());
            }
            parent()->reads(var);
        }

        virtual void reads(const LHSValue& var) {
            std::unordered_set<std::string> readList;
            var.writeSignals(readList);
            for (auto&& r : readList) {
                reads(r);
            }
        }

        virtual void writes(const std::string& var) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name();
                throw std::runtime_error(err.c_str());
            }
            parent()->writes(var);
        }

        virtual Register& getRegister(const std::string& var) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name();
                throw std::runtime_error(err.c_str());
            }
            return parent()->getRegister(var);
        }

        virtual bool hasOption(const std::string& var) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name();
                throw std::runtime_error(err.c_str());
            }
            return parent()->hasOption(var);
        }

        virtual bool isRegister(const std::string& name) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name_;
                throw std::runtime_error(err.c_str());
            }
            return parent()->isRegister(name);
        }

        virtual bool isVariable(const std::string& name) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name_;
                throw std::runtime_error(err.c_str());
            }
            return parent()->isVariable(name);
        }

        virtual bool isConstant(const std::string& name) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name_;
                throw std::runtime_error(err.c_str());
            }
            return parent()->isConstant(name);
        }

        virtual Width width(const std::string& name) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name_;
                throw std::runtime_error(err.c_str());
            }
            return parent()->width(name);
        }

        int integerWidth(const std::string& name) {
            int intWidth = width(name).intWidth;
            if (intWidth == -1) {
                std::string err = name_
                    + std::string(" does not have an integer width value.");
                throw std::runtime_error(err.c_str());
            }
            return intWidth;
        }

        virtual WireType wireType(const std::string& name) {
            if (parent() == nullptr) {
                std::string err =
                    std::string("Couldn't find parent for ") + name_;
                throw std::runtime_error(err.c_str());
            }
            return parent()->wireType(name);
        }

        virtual Width width() {
            throw std::runtime_error("Shouldn't be here!");
        }

        virtual WireType wireType() const {
            throw std::runtime_error("Shouldn't be here!");
        }

        virtual void hdl(std::ostream& stream, Language lang, int indent) {
            (void)stream;
            (void)lang;
            (void)indent;
            std::string err = std::string("Attempted to generate ") + name_;
            throw std::runtime_error(err);
        }

        virtual void hdl(std::ostream& stream, Language lang) {
            (void)lang;
            stream << name();
        }

        virtual void implementAll(std::ostream& stream, Language lang) {
            forAll([&](
                std::shared_ptr<Generatable> c) { c->hdl(stream, lang); });
        }

        virtual void
        implementAll(std::ostream& stream, Language lang, int indent) {
            forAll([&](std::shared_ptr<Generatable> c) {
                c->hdl(stream, lang, indent);
            });
        }

        template <typename Func> void forAll(Func func) {
            for (auto&& c : components_) {
                func(c);
            }
        }

        template <typename Type, typename Func> void forAll(Func func) {
            for (auto&& c : components_) {
                Type* ptr = dynamic_cast<Type*>(c.get());
                if (ptr != nullptr)
                    func(ptr);
            }
        }

        template <class Type> bool parentIs() {
            if (parent() == nullptr) {
                return false;
            } else if (dynamic_cast<Type*>(parent()) != nullptr) {
                return true;
            } else {
                return parent()->parentIs<Type>();
            }
        }

        template <class Type> Type* parentType() {
            if (parent() == nullptr) {
                throw std::runtime_error("Couldn't find parent'");
            } else if (dynamic_cast<Type*>(parent()) != nullptr) {
                return dynamic_cast<Type*>(parent());
            } else {
                return parent()->parentType<Type>();
            }
        }

        // Badly named to avoid using the template below
        void pushComponent(std::shared_ptr<Generatable> c) {
            components_.push_back(c);
        }

        template <class Component>
        void addComponent(Component c) {
            components_.push_back(std::make_shared<Component>(c));
        }

        const std::string& name() const noexcept { return name_; }

        void setParent(Generatable* parent) noexcept { parent_ = parent; }
        Generatable* parent() const noexcept { return parent_; }

    private:
        std::string name_;
        Generatable* parent_;
        std::vector<std::shared_ptr<Generatable>> components_;
    };
}
