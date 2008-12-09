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
 * @file SchedulerConfigurationSerializer.cc
 *
 * Implementation of SchedulerConfigurationSerializer class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma-no.spam-tut.fi)
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
