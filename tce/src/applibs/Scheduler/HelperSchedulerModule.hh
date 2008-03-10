/**
 * @file HelperSchedulerModule.hh
 *
 * Declaration of HelperSchedulerModule class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HELPER_SCHEDULER_MODULE_HH
#define TTA_HELPER_SCHEDULER_MODULE_HH

#include "BaseSchedulerModule.hh"

/**
 * Base class for helper scheduler plugin modules.
 */
class HelperSchedulerModule : public BaseSchedulerModule {
public:
    virtual ~HelperSchedulerModule();

    virtual bool isStartable() const;
    virtual void start()
        throw (ObjectNotInitialized, WrongSubclass, ModuleRunTimeError);

    bool isRegistered() const;
    void setParent(BaseSchedulerModule& parent);
    BaseSchedulerModule& parent() const throw (IllegalRegistration);

protected:
    HelperSchedulerModule();

private:
    /// Copying forbidden.
    HelperSchedulerModule(const HelperSchedulerModule&);
    /// Assignment forbidden.
    HelperSchedulerModule& operator=(const HelperSchedulerModule&);

    /// Parent module.
    BaseSchedulerModule* parent_;
};

#endif
