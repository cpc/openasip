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
