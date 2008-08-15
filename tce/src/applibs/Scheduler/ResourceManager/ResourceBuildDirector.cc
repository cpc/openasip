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
 * @file ResourceBuildDirector.cc
 *
 * Implementation of ResourceBuildDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
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
