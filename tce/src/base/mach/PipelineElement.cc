/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file PipelineElement.cc
 *
 * Implementation of class PipelineElement.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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
    const std::string& name,
    FunctionUnit& parentUnit)
    throw (ComponentAlreadyExists, InvalidName) :
    name_(""), parent_(&parentUnit) {

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
PipelineElement::setName(const std::string& name)
    throw (ComponentAlreadyExists, InvalidName) {

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


