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
 * @file ResourceMapper.cc
 *
 * Implementation of ResourceMapper class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ResourceMapper.hh"
#include "ResourceBroker.hh"
#include "ContainerTools.hh"
#include "MachinePart.hh"

using std::string;

/**
 * Constructor.
 */
ResourceMapper::ResourceMapper() {
}

/**
 * Destructor.
 */
ResourceMapper::~ResourceMapper(){
}

/**
 * Register broker as one of the elements that contribute to mapping a
 * subset of machine parts to resources.
 *
 * @param broker Broker to register.
 */
void
ResourceMapper::addResourceMap(const ResourceBroker& broker)
    throw (ObjectAlreadyExists) {

    if (ContainerTools::containsValue(brokers_, &broker)) {
        string msg = "Broker already exists in resource map!";
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__, msg);
    } else {
        brokers_.push_back(&broker);
    }
}

/**
 * Return the number of resources that correspond to the given machine part.
 *
 * @param mp Machine part.
 * @return The number of resources that correspond to the given machine part.
 */
int
ResourceMapper::resourceCount(const TTAMachine::MachinePart& mp) const {
    int count = 0;
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->hasResourceOf(mp)) {
            count++;
        }
    }
    return count;
}

/**
 * Find the resource object that corresponds to given machine part with
 * index (default 0).
 *
 * @param mp Machine part to find Scheduling Resource for
 * @param index Index of machine part, in case there are several
 * @return The resource object that corresponds to given machine part and
 * index.
 */
SchedulingResource&
ResourceMapper::resourceOf(const TTAMachine::MachinePart& mp, int index)
    const throw (KeyNotFound) {

    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->hasResourceOf(mp)) {
            if (index == 0) {
                return brokers_[i]->resourceOf(mp);
            } else {
                index--;
                continue;
            }
        }
    }
    std::string errMsg = "None of the brokers has resource for part ";
    const TTAMachine::Component * comp = 
        dynamic_cast<const TTAMachine::Component*>(&mp);
    const TTAMachine::SubComponent * subcomp = 
        dynamic_cast<const TTAMachine::SubComponent*>(&mp);

    if ( comp != NULL) {
        errMsg += ": " + comp->name();
    }
    if ( subcomp != NULL) {
        errMsg += ", some subcomponent";
    }

    throw KeyNotFound(__FILE__, __LINE__, __func__, errMsg);
}
