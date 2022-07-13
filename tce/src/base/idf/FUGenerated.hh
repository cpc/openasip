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
 * @file FUGenerated.hh
 *
 * Declaration of FUGenerated.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#pragma once

#include "ObjectState.hh"
#include "Serializable.hh"

#include <string>
#include <vector>

namespace IDF {

    class FUGenerated : public Serializable {
    public:
        struct Info {
            std::string operationName;
            std::string hdb;
            int id;
            int latency;
        };
        struct DAGOperation {
            std::string operationName;
            std::vector<Info> suboperations;
        };


        FUGenerated() = default;
        FUGenerated(const std::string& name);
        virtual ~FUGenerated() = default;

        void loadState(const ObjectState* state) override;
        ObjectState* saveState() const override;

        std::string name() const;
        void name(const std::string& newName);
        std::vector<Info>& operations();
        const std::vector<std::string>& dagOperations() const;
        const std::vector<Info>& operations() const;
        void addOperation(const Info& op);
        void addOperation(const DAGOperation& op);
        const std::vector<std::string>& options() const;

    private:
        std::string name_;
        std::vector<Info> operations_;
        std::vector<std::string> dagOperations_;
        std::vector<std::string> options_;
    };
}