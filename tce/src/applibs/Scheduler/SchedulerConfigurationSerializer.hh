/**
 * @file SchedulerConfigurationSerializer.hh
 *
 * Declaration of SchedulerConfigurationSerializer class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_CONFIGURATION_SERIALIZER_HH
#define TTA_SCHEDULER_CONFIGURATION_SERIALIZER_HH

#include "XMLSerializer.hh"
#include "SchedulingPlan.hh"

/**
 * Reads scheduler configuration files.
 */
class SchedulerConfigurationSerializer : public XMLSerializer {
public:
    SchedulerConfigurationSerializer();
    virtual ~SchedulerConfigurationSerializer();
    
    ObjectState* readState()
        throw (SerializerException);
    SchedulingPlan* readConfiguration()
        throw (SerializerException, ObjectStateLoadingException);

private:
    /// Copying forbidden.
    SchedulerConfigurationSerializer(
        const SchedulerConfigurationSerializer&);
    /// Assignment forbidden.
    SchedulerConfigurationSerializer& operator=(
        const SchedulerConfigurationSerializer&);
};

#endif
