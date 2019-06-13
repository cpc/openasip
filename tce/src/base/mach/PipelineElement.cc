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
 * @file PipelineElement.cc
 *
 * Implementation of class PipelineElement.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#include "PipelineElement.hh"
#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "HWOperation.hh"
#include "MachineTester.hh"

using std::string;

namespace TTAMachine {

/**
 * Constructor.
 *
 * Pipeline elements are created automatically when pipeline of operation is
 * created. Clients should not create pipeline elements.
 *
 * @param name Name of the pipeline element.
 * @param parentUnit The parent unit.
 * @exception ComponentAlreadyExists If there exists another pipeline element
 *                                   by the same name in the function unit.
 * @exception InvalidName If the given name is not a valid component name.
 */
PipelineElement::PipelineElement(
    const std::string& name, FunctionUnit& parentUnit)
    : name_(""), parent_(&parentUnit) {
    setName(name);

    // clear parent pointer to pass the assert in addPipelineElement
    parent_ = NULL;

    parentUnit.addPipelineElement(*this);
    parent_ = &parentUnit;
}

/**
 * Destructor.
 */
PipelineElement::~PipelineElement() {

    FunctionUnit* parent = parent_;
    parent_ = NULL;
    parent->deletePipelineElement(*this);

    // remove usages of this resource
    for (int i = 0; i < parent->operationCount(); i++) {
        HWOperation* operation = parent->operation(i);
        ExecutionPipeline* pLine = operation->pipeline();
        pLine->removeResourceUse(name());
    }
}


/**
 * Sets the name of the pipeline element.
 *
 * @param name The new name.
 * @exception ComponentAlreadyExists If another pipeline element exists by
 *                                   the same name in the same function unit.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
PipelineElement::setName(const std::string& name) {
    const string procName = "PipelineElement::setName";

    if (!MachineTester::isValidComponentName(name)) {
        throw InvalidName(__FILE__, __LINE__, procName);
    }

    if (name == this->name()) {
        return;
    }

    if (parentUnit()->hasPipelineElement(name)) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    name_ = name;
}

bool PipelineElement::Comparator::operator()(
    const PipelineElement* pe1, const PipelineElement* pe2) const {
    if (pe1 == NULL) {
        return false;
    }
    if (pe2 == NULL) {
        return true;
    }
    if (pe1->name() > pe2->name()) {
        return true;
    } 

    if (pe2->name() > pe1->name()) {
        return false;
    } 

    if (pe1->parentUnit()->name() > pe2->parentUnit()->name()) {
        return true;
    }
    return false;
}

}


