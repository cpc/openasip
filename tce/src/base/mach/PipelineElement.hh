/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file PipelineElement.hh
 *
 * Declaration of class PipelineElement.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_PIPELINE_ELEMENT_HH
#define TTA_PIPELINE_ELEMENT_HH

#include <string>

#include "Exception.hh"

namespace TTAMachine {

class FunctionUnit;

/**
 * Represents an resource inside pipeline in function unit.
 */
class PipelineElement {
public:
    PipelineElement(const std::string& name, FunctionUnit& parentUnit)
        throw (ComponentAlreadyExists, InvalidName);
    virtual ~PipelineElement();

    std::string name() const;
    void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    FunctionUnit* parentUnit() const;

    struct Comparator {
        bool operator()(
            const PipelineElement* pe1, const PipelineElement* pe2) const;
    };
private:
    /// Name of the pipeline element.
    std::string name_;
    /// The parent unit.
    FunctionUnit* parent_;
};
}

#include "PipelineElement.icc"

#endif
