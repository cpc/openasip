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
