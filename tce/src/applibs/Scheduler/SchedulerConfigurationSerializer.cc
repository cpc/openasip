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
 * @file SchedulerConfigurationSerializer.cc
 *
 * Implementation of SchedulerConfigurationSerializer class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "SchedulerConfigurationSerializer.hh"
#include "SchedulerPluginLoader.hh"

/**
 * Constructor.
 */
SchedulerConfigurationSerializer::SchedulerConfigurationSerializer():
    XMLSerializer() {
}

/**
 * Destructor.
 */
SchedulerConfigurationSerializer::~SchedulerConfigurationSerializer() {
}

/**
 * Reads the options from the current XML file set and creates an
 * ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while reading the file.
 */
ObjectState*
SchedulerConfigurationSerializer::readState()
    throw (SerializerException) {

    ObjectState* state = XMLSerializer::readState();
    return state;
}

/**
 * Reads the current configuration file set and creates a SchedulingPlan
 * according to it.
 *
 * @return The newly created SchedulingPlan.
 * @exception SerializerException If an error occurs while reading the
 * configuration file.
 * @exception ObjectStateLoadingException If an error occurs while creating
 * the SchedulingPlan.
 */
SchedulingPlan*
SchedulerConfigurationSerializer::readConfiguration()
    throw (SerializerException, ObjectStateLoadingException) {

    ObjectState* configuration = readState();
    SchedulingPlan* schedulingPlan = new SchedulingPlan();
    schedulingPlan->build(*configuration);
    delete configuration;
    return schedulingPlan;
}
