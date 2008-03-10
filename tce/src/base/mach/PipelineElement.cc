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

}
