/*
    Copyright (c) 2002-2009 Tampere University.

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
ResourceMapper::addResourceMap(const ResourceBroker& broker) {
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
ResourceMapper::resourceOf(const TTAMachine::MachinePart& mp, int index) const {
    const unsigned int brokerSize = brokers_.size();
    for (unsigned int i = 0; i < brokerSize; i++) {
        SchedulingResource* res = brokers_[i]->resourceOf(mp);
        if (res != NULL) {
            if (index == 0) {
                return *res;
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

    if (comp != NULL) {
        errMsg += ": " + comp->name();
    }
    if (subcomp != NULL) {
        errMsg += ", some subcomponent";
    }

    throw KeyNotFound(__FILE__, __LINE__, __func__, errMsg);
}
