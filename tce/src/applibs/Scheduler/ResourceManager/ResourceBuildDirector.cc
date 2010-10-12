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
 * @file ResourceBuildDirector.cc
 *
 * Implementation of ResourceBuildDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "ResourceBuildDirector.hh"
#include "ResourceBroker.hh"
#include "ContainerTools.hh"
#include "Exception.hh"
#include "SequenceTools.hh"
/**
 * Constructor.
 */
ResourceBuildDirector::ResourceBuildDirector() {
}

/**
 * Destructor.
 */
ResourceBuildDirector::~ResourceBuildDirector(){
    SequenceTools::deleteAllItems(brokers_);
}

/**
 * Add given broker to the list of brokers responsible for resource
 * construction.
 *
 * The order in which brokers are added is irrelevant.
 *
 * @param broker Broker to add.
 */
void
ResourceBuildDirector::addBroker(ResourceBroker& broker)
    throw (ObjectAlreadyExists) {

    if (ContainerTools::containsValue(brokers_, &broker)) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__);
    } else {
        brokers_.push_back(&broker);
    }
}

/**
 * Build a complete resource object model for given target processor.
 *
 * @machine Machine to build from.
 */
void
ResourceBuildDirector::build(const TTAMachine::Machine& machine) {

    // build primary resources
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        brokers_[i]->buildResources(machine);
        mapper_.addResourceMap(*brokers_[i]);
    }

    // setup dependent and related resource links
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        brokers_[i]->setupResourceLinks(mapper_);
    }
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        // broker throws in case problem is found
        brokers_[i]->validateResources();
    }
}

/**
 * Clears all resources of the resouce manager so that the RM can be reused.
 */
void 
ResourceBuildDirector::clear() {
    for (size_t i = 0; i < brokers_.size(); i++) {
        brokers_[i]->clear();
    }
}
