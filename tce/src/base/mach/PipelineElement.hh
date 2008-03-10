/**
 * @file PipelineElement.hh
 *
 * Declaration of class PipelineElement.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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

private:
    /// Name of the pipeline element.
    std::string name_;
    /// The parent unit.
    FunctionUnit* parent_;
};
}

#include "PipelineElement.icc"

#endif
