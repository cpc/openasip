/**
 * @file StartableSchedulerModule.hh
 *
 * Declaration of StartableSchedulerModule class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_STARTABLE_SCHEDULER_MODULE_HH
#define TTA_STARTABLE_SCHEDULER_MODULE_HH

#include "BaseSchedulerModule.hh"

/**
 * Base class for startable scheduler plugin modules.
 */
class StartableSchedulerModule : public BaseSchedulerModule {
public:
    virtual ~StartableSchedulerModule();

    virtual bool isStartable() const;

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

protected:
    StartableSchedulerModule();

private:
    /// Copying forbidden.
    StartableSchedulerModule(const StartableSchedulerModule&);
    /// Assignment forbidden.
    StartableSchedulerModule& operator=(const StartableSchedulerModule&);
};

#endif
